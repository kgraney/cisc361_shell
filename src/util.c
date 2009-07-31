#include "util.h"
#include "alias.h"
#include "builtins.h"
#include "get_path.h"
#include "wildcard.h"

char* which(const char* command, pathList* pathlist){
    pathList* pl = pathlist;

    // Loop to iterate over every directory in the path
    while(pl != NULL){

	DIR* dirp = opendir(pl->element);

	if(dirp != NULL){
	    struct dirent* dp = readdir(dirp);

	    // Iterate over every file in the directory
	    while(dp != NULL){

		// Determine if the filename matches the command
		if(strcmp(dp->d_name, command) == 0){
                    closedir(dirp);

		    // Generate an absolute path for the file that was found
		    char* full_path = malloc(strlen(command) 
			    + strlen(pl->element) + 2);                
                    sprintf(full_path, "%s/%s", pl->element, command);

		    // Check for execute permissions on the file found
		    if(access(full_path, X_OK) == 0){
			return full_path;
		    } else {

			// Free the memory if we're not returning it
			free(full_path);
		    }
		}
		dp = readdir(dirp);
	    }
	    closedir(dirp);
	}
	pl = pl->next;
    }
    return NULL;
}


void add_to_history(char* command, kgenv* env){
    histList* new_item;
    new_item = malloc(sizeof(histList));

    new_item->command = (char*)malloc(strlen(command) + 1);
    strcpy(new_item->command, command);
    new_item->next = env->hist;

    if(env->hist != NULL){
	new_item->num = env->hist->num + 1;
    } else {
	new_item->num = 1;
    }

    env->hist = new_item;
}


int exec_cmd(char* cmd, char** argv){

    //TODO: Print absolute path even if relative is passed?
    #ifdef O_VERBOSE_EXE
    printf("Executing %s\n", cmd);	// Print out what's being executed
    fflush(stdout);
    #endif //O_VERBOSE_EXE

    pid_t child_pid = fork();
    int child_status;

    if(child_pid == 0){			//** Executed in child process

	execve(cmd, argv, environ);
	
	// Exec commands only return if there's an error
	perror("Error in exec");	
	
	// We exit since the process image will be replaced with itself here and
	// we will need to enter "exit" twice to truely exit.
	exit(0);	

    } else if(child_pid > 0) {		//** Executed in parent process

	waitpid(child_pid, &child_status, 0);

	// Print out the exit status if it is non-zero
	if(WEXITSTATUS(child_status) != 0)
	    printf("Exit %d\n", WEXITSTATUS(child_status));

    } else {				//** Didn't fork properly 

	perror("Fork failed\n");

    }

    return child_status;
}


int process_command_in(char* line_in, kgenv* global_env){

    int    in_argc;		// argc for the command being processed
    char** in_argv;	        // argv for the command being processed
    int    line_length; 	// The length of the input line



    line_length = strlen(line_in);
    if(line_in[line_length - 1] == '\n')      // Remove trailing newline
	line_in[line_length -1] = '\0';

    //## Capture an EOF with no prefix 
    if(feof(stdin)){
	printf("\nUse \"exit\" to leave kgsh.\n");
    }
    //TODO: Add EOF listing capability


    //## Add the line to the history stack
    if(line_in[0] != '\0')	// don't add blank lines
	add_to_history(line_in, global_env);
    

    //## Expand wildcards
    //char* line_in_unexpanded = line_in;
    //line_in = expand_wildcards(line_in, global_env);
    //free(line_in_unexpanded);


    //## Tokenize the line
    //TODO: free in_argv
    in_argv = (char**)calloc(MAX_TOKENS_PER_LINE, sizeof(char));
    if(!in_argv){
	perror("Not enough heap");
	exit(1);
    }

    if(!parse_line(&in_argc, &in_argv, line_in)){
	return line_length;	// continue if the line is blank
    }

    //## Check for aliases (Do before builtins to allow for aliasing
    //## builtin commands.
    aliasList* alias_ptr = is_alias(global_env, in_argv[0]);
    if(alias_ptr){
	int length = process_command_in(alias_ptr->string, global_env);
	detokenize(alias_ptr->string, length);
	return line_length;
    }


    //## Process built in commands
    int builtin_code = is_builtin(in_argv[0]);
    if(builtin_code){
	#ifdef O_VERBOSE_EXE
	printf("Executing builtin %s\n", in_argv[0]);
	#endif //O_VERBOSE_EXE
	(*BUILT_IN_FUNCS[--builtin_code])(global_env, in_argc, in_argv);
	return line_length;
    }

    //## process absolute and relative paths
    // TODO: cleanup this logic
    if( (in_argv[0][0] == '/') ||
	((in_argv[0][0] == '.') && ((in_argv[0][1] == '/') ||
		 (in_argv[0][1] == '.') && (in_argv[0][2] == '/')))){

	// Execute the file if it's executable
	if(access(in_argv[0], X_OK) == 0){
	    exec_cmd(in_argv[0], in_argv);
	    return line_length;
	}
    }


    //## process commands in the path
    char* exe_path = which(in_argv[0], global_env->path);
    if(exe_path != NULL){

	exec_cmd(exe_path, in_argv);
	return line_length;

    }

    //## command not found
    fprintf(stderr, "%s: Command not found.\n", in_argv[0]);

}

int parse_line(int* argc, char*** argv, char* line){
    char* strtok_ptr = NULL;
    char* token = strtok_r(line, " \n", &strtok_ptr);

    // If the line is blank, the first token will be the null string.
    if(token == '\0')
	return 0;	

    *argv[0] = token;	// argv[0] is the command name

    for(int i = 1; token != NULL && i < MAX_TOKENS_PER_LINE; i++){
	token = strtok_r(NULL, " \t", &strtok_ptr);
	(*argv)[i] = token;
	*argc = i;
    }

    return 1;
}


void detokenize(char* str, int length){
    for(int i=0; i < length - 1; i++){
	if(str[i] == '\0'){
	    str[i] = ' ';
	}
    }
}

void set_environment(kgenv* env, char* name, char* value){

	//TODO: check for possible memory leaks here
	char* str = malloc(strlen(name) + strlen(value) + 2);
	sprintf(str, "%s=%s", name, value);
	putenv(str);

	// Handle a change to HOME
	if(strcmp(name, "HOME") == 0){
	    //TODO: improve?
	    env->homedir = str + 5;
	}

	// Handle a change to PATH
	else if(strcmp(name, "PATH") == 0){
	    //TODO: check for memory leaks here
	    pathList* p = env->path;
	    pathList* old;

	    // Only free the first one since they are malloced together.
	    free(p->element);
	    while(p != NULL){
		old = p;
		p = p->next;
		free(old);
	    }

	    env->path = get_path();
	}
}
