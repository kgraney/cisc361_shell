/*
 * CISC361: Operating Systems (Fall 2009)
 * Instructor: Ben Miller
 *
 * Project 2
 * Kevin Graney
 */

/** 
 * @file util.c
 * @brief Definitions of utility functions.
 * @author Kevin Graney
 * @version v0.1
 * @date 2009-08-02
 */
#include "util.h"
#include "alias.h"
#include "builtins.h"
#include "get_path.h"
#include "wildcard.h"
#include "redirection.h"
#include "ipc.h"

#include <stdio.h>
#include <unistd.h>
#include <readline/history.h>

/** 
 * @brief Returns the location of an executable in the PATH.
 *
 * Loops through the path linked list and returns the location of the first
 * file named command in the path directories with execute permissions.  Does
 * not go into sub-directories in search of an executable.
 * 
 * @param command Name of the executable to search for.
 * @param pathlist The path list to search.  Usually the one stored in the
 * global ::kgenv environment structure.
 * 
 * @return 
 */
char* which(const char* command, pathList* pathlist){
    pathList* pl = pathlist;

    // Loop to iterate over every directory in the path
    while(pl != NULL){

        DIR* dirp = opendir(pl->element);

        if(dirp == NULL){

            perror("Error in which");
            return NULL;

        } else {

            struct dirent* dp = readdir(dirp);        //TODO: errno check?

            // Iterate over every file in the directory
            while(dp != NULL){

                // Determine if the filename matches the command
                if(strcmp(dp->d_name, command) == 0){
                    closedir(dirp);

                    // Generate an absolute path for the file that was found
                    char* full_path = malloc(strlen(command) 
                            + strlen(pl->element) + 2);                
                    if(full_path == NULL){
                        perror("Error in which");
                        return NULL;
                    }

                    sprintf(full_path, "%s/%s", pl->element, command);

                    // Check for execute permissions on the file found
                    if(access(full_path, X_OK) == 0){
                        return full_path;
                    } else {
                        //TODO: Verify we don't need a perror here.  This should
                        //be silent if an error condition occurs.

                        // Free the memory if we're not returning it
                        free(full_path);
                    }
                }
                dp = readdir(dirp);
            }
            if(closedir(dirp) == -1){
                perror("Error in which");
            }
        }
        pl = pl->next;
    }
    return NULL;
}


/** 
 * @brief Adds a command to the history list.
 * 
 * @param command The command to be added to the list.
 * @param env The global ::kgenv environment object.  Needed to access the
 * global history list.
 */
void add_to_history(char* command, kgenv* env){

    //## Add the command to readline's history
    add_history(command);

    //## Add the command to kgsh history
    histList* new_item;
    new_item = malloc(sizeof(histList));
    if(new_item == NULL){
        perror("Error adding to history");
        return;
    }

    new_item->command = (char*)malloc(strlen(command) + 1);
    if(new_item->command == NULL){
        perror("Error adding to history");
        return;
    }

    strcpy(new_item->command, command);
    new_item->next = env->hist;

    if(env->hist != NULL){
        new_item->num = env->hist->num + 1;
    } else {
        new_item->num = 1;
    }

    env->hist = new_item;
}


/** 
 * @brief Executes a command.
 *
 * Forks the shell process and executes the given command in the child process.
 * Passes all environment variables. 
 * 
 * @param cmd The command to be exectued.
 * @param argv Argument array for the command.
 * @param background True if the job needs to be backgrounded
 * 
 * @return The exit status of the command.
 */
int exec_cmd(char* cmd, char** argv, bool background){

    //TODO: Print absolute path even if relative is passed?
    #ifdef O_VERBOSE_EXE

    // Print out what's being executed and if it is backgrounded
    printf("Executing %s%s\n", cmd, (background ? " in background":""));
    fflush(stdout);
    #endif //O_VERBOSE_EXE

    pid_t child_pid = fork();
    int child_status;

    if(child_pid == 0){                        //** Executed in child process

        execve(cmd, argv, environ);
        
        // Exec commands only return if there's an error
        perror("Error in exec");        
        
        // We exit since the process image will be replaced with itself here and
        // we will need to enter "exit" twice to truely exit.
        exit(0);        

    } else if(child_pid > 0) {                //** Executed in parent process

        if(!background){

            // If the job isn't backgrounded, wait for child process to return
            if(!waitpid(child_pid, &child_status, 0)){
                perror("Error in waidpid");
            }

        } else {

            // TODO: add signal handler for SIGCHLD
            if(waitpid(child_pid, &child_status, WNOHANG | WNOWAIT) == -1){
                perror("Error in backgrounding waitpid");
            }

        }

        // Print out the exit status if it is non-zero
        //if(WEXITSTATUS(child_status) != 0){
        //    printf("Exit %d\n", WEXITSTATUS(child_status));
        //}

    } else {                                //** Didn't fork properly 

        perror("Fork failed\n");

    }

    return child_status;
}


/** 
 * @brief Processes an input command line.
 * 
 * Processes an input command line entered at the shell prompt from tokenizing
 * through execution.  Handles wildcards, aliases, built-in commands, relative
 * and absolute paths, and any other command line syntax.
 *
 * This function is called primarily through the closed prompt loop in ::main.
 * Memory allocation and deallocation of line_in is handled by ::main. 
 *
 * @param line_in The command line entered at the shell prompt.
 * @param global_env The ::kgenv global environment structure.
 * @param deref_alias True if being called on an expanded alias.  False
 * otherwise.  Needed to allow aliases to override commands without causing
 * circular references.
 * 
 * @return The length of the line processed.
 */
int process_command_in(char* line_in, kgenv* global_env, bool deref_alias,
        bool blocking){ 

    int    in_argc;             // argc for the command being processed
    char** in_argv;             // argv for the command being processed
    int    line_length;         // The length of the input line
    bool   background = false;  // True if the command needs to be backgrounded
    int fid;

    line_length = strlen(line_in);
    if(line_in[line_length - 1] == '\n')      // Remove trailing newline
        line_in[line_length -1] = '\0';

    //## Capture an EOF with no prefix 
    if(feof(stdin)){
        //printf("\nUse \"exit\" to leave kgsh.\n");
        //TODO: Fix this feature.
    }

    //## Add the line to the history stack
    if(line_in[0] != '\0'        // don't add blank lines
            && !deref_alias){    // don't add the second call for an alias
        add_to_history(line_in, global_env);
    }
    

    //## Expand wildcards
    if(contains_wildcards(line_in)){
        char* line_in_original = line_in;
        line_in = expand_wildcards(line_in);
        free(line_in_original);
    }

    //## Process redirection operators
    char* command_line = NULL;
    char* redirect_file = NULL;
    int redirection_type = parse_redirection(&command_line, &redirect_file, 
            line_in);

    //TODO: free redirect_file
    if(redirection_type >= 0){
        // Appending or clobbering, or file doesn't exist
        if(!global_env->noclobber || redirection_type == RD_ALL_APPEND || 
                redirection_type == RD_STDOUT_APPEND || 
                redirection_type == RD_STDIN || 
                access(redirect_file, F_OK) == -1 ){

            perform_redirection(&fid, redirect_file, redirection_type);

        // File exists and not appending or clobbering    
        } else {
            printf("File %s exists.  Overwrite? (y/n) ", redirect_file);
            char c = getchar();
            getchar();
            if(c == 'y' || c == 'Y'){
                if(remove(redirect_file) == -1){
                    perror("Error removing existing redirect file");
                }
                perform_redirection(&fid, redirect_file, redirection_type);
            } else {
                return line_length;
            }
        }

        // Remove the redirection part of the command before continuing
        char* line_in_original = line_in;
        line_in = command_line;
        free(line_in_original);
    }

    //## Process IPC
    if(contains_ipc(line_in)){
        char *left, *right;
        enum ipc_opcodes ipc_type;
        ipc_type = parse_ipc_line(&left, &right, line_in);

        //printf("Piping '%s' to '%s'\n", left, right);
        perform_ipc(left, right, ipc_type, global_env);

        free(left);
        free(right);
        return 0;

    }

    //## Tokenize the line
    //TODO: free in_argv
    in_argv = (char**)calloc(MAX_TOKENS_PER_LINE, sizeof(char*));
    if(in_argv == NULL){
        perror("Error processing command");
        return 0;
    }

    if(!parse_line(&in_argc, &in_argv, &background, line_in)){
        reset_redirection(&fid, redirection_type);
        free(in_argv);
        free(line_in);
        return line_length;        // continue if the line is blank
    }

    //## Check for aliases (Do before builtins to allow for aliasing
    //## builtin commands.
    if(!deref_alias){
        aliasList* alias_ptr = is_alias(global_env, in_argv[0]);
        if(alias_ptr){
            char* new_line_in = (char*)malloc(strlen(alias_ptr->string) + 1);
            strcpy(new_line_in, alias_ptr->string);

            int length = process_command_in(new_line_in, global_env, true, 
                    blocking);
            detokenize(alias_ptr->string, length);

            reset_redirection(&fid, redirection_type);
            free(in_argv);
            free(line_in);
            return line_length;
        }
    }


    //## Process built in commands
    int builtin_code = is_builtin(in_argv[0]);
    if(builtin_code){
        #ifdef O_VERBOSE_EXE
        printf("Executing builtin %s\n", in_argv[0]);
        #endif //O_VERBOSE_EXE
        (*BUILT_IN_FUNCS[--builtin_code])(global_env, in_argc, in_argv);

        reset_redirection(&fid, redirection_type);
        free(in_argv);
        free(line_in);
        return line_length;
    }

    //## Process absolute and relative paths
    // TODO: cleanup this logic
    if( (in_argv[0][0] == '/') ||
        ((in_argv[0][0] == '.') && ((in_argv[0][1] == '/') ||
             (in_argv[0][1] == '.') && (in_argv[0][2] == '/')))){

        // Execute the file if it's executable
        if(access(in_argv[0], X_OK) == 0){
            exec_cmd(in_argv[0], in_argv, background || !blocking);

            reset_redirection(&fid, redirection_type);
            free(in_argv);
            free(line_in);
            return line_length;
        }
    }


    //## Process commands in the path
    char* exe_path = which(in_argv[0], global_env->path);
    if(exe_path != NULL){

        exec_cmd(exe_path, in_argv, background || !blocking);

        reset_redirection(&fid, redirection_type);
        free(in_argv);
        free(line_in);
        free(exe_path);
        return line_length;

    }

    //## Command not found
    fprintf(stderr, "%s: Command not found.\n", in_argv[0]);

    reset_redirection(&fid, redirection_type);
    free(in_argv);
    free(line_in);
    return line_length;
}

/** 
 * @brief Parses a command line into an argument (argv) array.
 * 
 * @param argc Will be set to the number of arguments in the command string.
 * @param argv Will be set to point to the array of arguments in the command 
 * string.  This argument should be preallocated to be an array of pointers.
 * The returned array will point to memory locations inside of line, so it's
 * important that line is not deleted before appropriate action is taken.
 * @param background Will be set to true if the job needs to be backgrounded
 * (i.e. if an & is the last character on the line).
 * @param line The input line to parse.
 * 
 * @return 1 if the command was successfully parsed, and 0 if the line is blank.
 */
int parse_line(int* argc, char*** argv, bool* background, char* line){
    int line_length = strlen(line);

    //## Check if job needs to be backgrounded
    if(line[line_length - 1] == '&'){
        line[line_length - 1] = '\0';        // Remove the '&' character
        *background = true;
    } else {
        *background = false;
    }

    //## Tokenize the command into the argv array
    char* strtok_ptr = NULL;
    char* token = strtok_r(line, " \n", &strtok_ptr);


    // If the line is blank, the first token will be the null string.
    if(token == '\0')
        return 0;        

    *argv[0] = token;        // argv[0] is the command name

    for(int i = 1; token != NULL && i < MAX_TOKENS_PER_LINE; i++){
        token = strtok_r(NULL, " \t", &strtok_ptr);
        (*argv)[i] = token;
        *argc = i;
    }

    return 1;
}

/** 
 * @brief Detokenizes a string that was tokenized using ::strtok.
 *
 * Used primarily by alias functions to detokenize the alias string before 
 * storing it in the alias linked list.  For this function to work, all tokens
 * must still be stored sequentially in memory as they are after a call to
 * ::strtok.
 * 
 * @param str Pointer to the start of the string.
 * @param length The length of the string in characters.
 */
void detokenize(char* str, int length){
    for(int i=0; i < length - 1; i++){
        if(str[i] == '\0'){
            str[i] = ' ';
        }
    }
}

/** 
 * @brief Sets an environment variable.
 * 
 * Sets an environment variable in the ::kgenv global environment structure's
 * internal environment string.  Special action is taken if either the HOME or
 * PATH environment variables change since other data structres need to be
 * updated.
 *
 * @param env The global ::kgenv environment structure.
 * @param name The name of the environment variable to set.
 * @param value The value (string) to set the environment variable to.
 */
void set_environment(kgenv* env, char* name, char* value){

    // Store the new environment variable
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
