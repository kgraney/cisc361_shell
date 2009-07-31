#include "builtins.h"
#include "get_path.h"
#include "alias.h"

//------------------------------------------------------------------------------
//-- The following constants define the built-in commands.  The commands are
//-- matched to the function pointers with a one-to-one matching done in order.
//------------------------------------------------------------------------------

// defines the built in commands
const char* BUILT_IN_COMMANDS[] = {
    "exit",
    "which",
    "where",
    "cd",
    "pwd",
    "list",
    "pid",
    "kill", 
    "prompt",
    "printenv",
    "alias", 		// Not required, but useful.
    "unalias",
    "history", 
    "setenv"
#ifdef DEBUG		// Various built ins defined for debugging purposes
	,
    "_db_tokenizer",
    "_db_kgenv",
    "_db_path",
    "_db_history"
#endif //DEBUG
};

// Defines a function pointer to the function to execute for the built-in
// command
void (*BUILT_IN_FUNCS[])(kgenv*, int, char**) = {
    bic_exit, 
    bic_which, 
    bic_where, 
    bic_cd,
    bic_pwd, 
    bic_list,
    bic_pid, 
    bic_kill, 
    bic_prompt, 
    bic_printenv, 
    bic_alias,
    bic_unalias,
    bic_history, 
    bic_setenv
#ifdef DEBUG		// various built ins defined for debugging purposes
    	,
    _db_tokenizer,
    _db_kgenv,
    _db_path,
    _db_history
#endif //DEBUG
};

/* short int is_builtin(char* command)
 *
 * returns 0 if the given command is not built in
 * returns a positive value that is 1 greater than the index of the function in
 * the builtin definition arrays (BUILT_IN_COMMANDS and BUILT_IN_FUNCS)
 */
short int is_builtin(char* command){
    for(int i=0; i < NUM_BUILTINS; i++){
	int result = strcmp(command, BUILT_IN_COMMANDS[i]);
	if(result == 0)
	    return i + 1;
    }
    
    return 0;
}

//------------------------------------------------------------------------------
//-- Definitions of the various built in functions.
//------------------------------------------------------------------------------

void bic_exit(kgenv* env, int argc, char* argv[]){
    exit(0);
}

void bic_which(kgenv* env, int argc, char* argv[]){
    if(argc == 1){
	fprintf(stderr, "which: too few arguments\n");
	return;
    }

    // Loop through each argument and display the path
    for(int i = 1; i < argc; i++){
	char* path = which(argv[i], env->path);

	if(path != NULL){
	    printf("%s\n", path);
	    free(path);
	}
    }
}

void bic_where(kgenv* env, int argc, char* argv[]){
    if(argc == 1){
	fprintf(stderr, "where: Too few arguments.\n");
	return;
    }

    // For loop executed once for each argument
    for(int i = 1; i < argc; i++){
	pathList* pl = env->path;
	char* cmd = argv[i];
	
	// While loop executed once for ach directory in the path
	while(pl != NULL){
	    DIR* dirp = opendir(pl->element);
	    if(dirp != NULL){
		struct dirent* dp = readdir(dirp);

		// While loop executed once for each file in directory
		while(dp != NULL){
		    if(strcmp(dp->d_name, cmd) == 0){
			printf("%s/%s\n", pl->element, cmd);
		    }
		    dp = readdir(dirp);
		}
		closedir(dirp);
	    }
	    pl = pl->next;
	}
    }
}

void bic_cd(kgenv* env, int argc, char* argv[]){
    //** Does nothing if executed with more than one argument 
    if(argc > 2){
	fprintf(stderr, "cd: Too many arguments.\n");
	return;
    }

    //** When called with no arguments cd to home directory
    if(argc == 1){

	// Free up the previous directory and set previous to current 
	if(env->pwd != NULL) free(env->pwd);
	env->pwd = env->cwd;

	// Set the current directory to the home directory
	env->cwd = (char*)malloc(strlen(env->homedir) + 1);
	strcpy(env->cwd, env->homedir);
	
	// Use chdir to change the working directory
	if(chdir(env->cwd) != 0) 
	    perror("Error in chdir");

    } 
    
    else 

    //** If called as "cd -", cd to the previous directory (pwd in kgenv)
    if(strcmp(argv[1], "-") == 0){

	// Swap the current working directory with the previous working
	// directory
	char* temp = env->cwd;
	env->cwd = env->pwd;
	env->pwd = temp;

	// Use chdir to change the working directory
	if(chdir(env->cwd) != 0) 
	    perror("Error in chdir");

    }

    else

    //** Otherwise we have either a relative or absolute path to a directory
    {
	// Change to the path specified in the argument
	if(chdir(argv[1]) != 0){
	    perror(NULL);
	    return;
	}

	// Free up the previous directory and set previous to current 
	if(env->pwd != NULL) free(env->pwd);
	env->pwd = env->cwd;

	// Set the current working directory string.  Using getcwd allows us to
	// avoid having to resolve an absolute path if the argument is relative.
	env->cwd = getcwd(NULL, CWD_BUFFER_SIZE);
    }
}

void bic_pwd(kgenv* env, int argc, char* argv[]){

    // Print the current working directory
    printf("%s\n", env->cwd);

}

// TODO: reverse order of printout
void bic_list(kgenv* env, int argc, char* argv[]){
    DIR* dirp;			// directory pointer
    bool cwd_mode = false;	// true if passed with no args

    // If called with no arguments we just add an argument that is the current
    // working directory.
    if(argc == 1){	
	argc++;
	argv[1] = env->cwd;
	cwd_mode = true;
    }


    // Loop over the argument list and print each directory listing.
    for(int i=1; i < argc; i++){
	// Only print the directory name if we are processing arguments.
	if(!cwd_mode)
	    printf("\n%s:\n", argv[i]);

	dirp = opendir(argv[i]);
	if(dirp == NULL){
	    fprintf(stderr, "Error opening directory!\n");

	} else {

	    // This loop iterates through the directory stream.
	    struct dirent* dp = readdir(dirp);
	    while(dp != NULL){
		printf("%s\n", dp->d_name);
		dp = readdir(dirp);
	    }

	    closedir(dirp);
	}

    }
}

void bic_pid(kgenv* env, int argc, char* argv[]){
    //TODO: add error handling for getpid
    printf("%d\n", getpid());
}

void bic_kill(kgenv* env, int argc, char* argv[]){
    //TODO: bic_kill

    if(argc == 1){
	fprintf(stderr, "kill: Too few arguments.\n");
    }

    else if(argc == 1){
	int pid = atoi(argv[1]);
	//sigsend(P_PID, pid, SIGTERM);
	if(kill(pid, SIGTERM) != 0){
	    perror("kill");
	}
    }


}

void bic_prompt(kgenv* env, int argc, char* argv[]){
    char* new_prompt;		// the new prompt string

    // Case where we are passed arguments.
    if(argc > 1){	
	new_prompt = argv[1];
	strcpy(env->prompt, new_prompt);
	return;
    }

    // Case where we prompt user for input.
    printf("New prompt prefix: ");
    char* prompt_in = (char*)malloc(LINE_BUFFER_SIZE);
    fgets(prompt_in, LINE_BUFFER_SIZE, stdin);

    // Need to remove trailing newline from input.
    if(prompt_in[strlen(prompt_in) - 1] == '\n'){
	prompt_in[strlen(prompt_in) - 1] = '\0';
    }

    // Save some heap by re-allocating only what's needed.
    new_prompt = (char*)malloc(strlen(prompt_in) + 1);
    strcpy(new_prompt, prompt_in);
    env->prompt = new_prompt;
    free(prompt_in);
}

void bic_printenv(kgenv* env, int argc, char* argv[]){

    // Called with no arguments, print entire environment
    if(argc == 1){
	char** i = environ;
	while(*i != NULL){
	    printf("%s\n", *i);
	    i++;
	}
    }

    // Called with one argument, print the value
    else if(argc == 2){
	char* value = getenv(argv[1]);
	if(value != NULL){
	    printf("%s\n", value);
	} 
    }


    // Called with more than one argument
    else {
	fprintf(stderr, "printenv: Too many arguments.\n");
    }

}

void bic_alias(kgenv* env, int argc, char* argv[]){

    // If no arguments are passed print the alias list
    if(argc == 1){
	aliasList* a = env->aliases;
	while(a != NULL){
	    //TODO: update to print entire argv array
	    printf("%s\t(%s)\n", a->name, a->string);
	    a = a->next;
	}
	return;
    }

    // Add the alias to the list.  We need to decrement argc by 2 (command and
    // alias name).
    add_alias(env, argv[1], argc - 2, &argv[2]);
}

void bic_unalias(kgenv* env, int argc, char* argv[]){
    //TODO: support multiple arguments
    if(argc == 2)
	remove_alias(env, argv[1]);
}

//TODO: fix history
void bic_history(kgenv* env, int argc, char* argv[]){
    int num_items = 0;	// Number of commands to print

    // We default to printing 10 commands if no argument is passed
    if(argc == 1){
	num_items = 10;
    } else {
	num_items = atoi(argv[1]);
    }

    // Output ordered pointers; we allocate space for num_items pointers even
    // if they aren't all going to be used.  Point to the histelement struct for
    // the given command.
    histList* outbuf[num_items];	

    histList* h = env->hist;
    int j=num_items - 1;

    // Loop through the last 
    while(h != NULL && j >= 0){
	outbuf[j] = h;
	h = h->next;
        j--;
    }
    
    j++;	// Need to increment j to adjust for final decrement
    for(int i=j; i < num_items; i++){
	printf("%d: %s\n", outbuf[i]->num, outbuf[i]->command);
    }
}

void bic_setenv(kgenv* env, int argc, char* argv[]){
    //TODO: bic_setenv

    // Called with no arguments, print entire environment
    if(argc == 1){
	char** i = environ;
	while(*i != NULL){
	    printf("%s\n", *i);
	    i++;
	}
    }

    // Called with one argument, set variable to null
    else if(argc == 2){
	set_environment(env, argv[1], "");
    }

    // Called with two arguments, set variable to 2nd argument
    else if(argc == 3){
	set_environment(env, argv[1], argv[2]);
    }

    // Called with too many arguments
    else {
	fprintf(stderr, "setenv: Too many arguments.\n");
    }
}

//------------------------------------------------------------------------------
//-- Definitions of debug functions
//------------------------------------------------------------------------------

#ifdef DEBUG
void _db_tokenizer(kgenv* env, int argc, char* argv[]){
    printf("argc = %d\n", argc);
    for(int i=0; i<argc; i++){
	printf("argv[%d] = %s\n", i, argv[i]);
    }
}

void _db_kgenv(kgenv* env, int argc, char* argv[]){

    printf("uid=%d\n", env->uid);
    printf("homedir=%s\n", env->homedir);

    printf("cwd=%s\n", env->cwd);
    printf("pwd=%s\n", env->pwd);
    
    printf("prompt=%s\n", env->prompt);
}

void _db_path(kgenv* env, int argc, char* argv[]){
    pathList* p = env->path;
    while (p != NULL){
	printf("%s\n", p->element);
	p = p->next;
    }
}

void _db_history(kgenv* env, int argc, char* argv[]){
    histList* h = env->hist;
    while (h != NULL){
	printf("%d: %s\n", h->num, h->command);
	h = h->next;
    }
}

#endif //DEBUG
