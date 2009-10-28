/*
 * CISC361: Operating Systems (Fall 2009)
 * Instructor: Ben Miller
 *
 * Project 2
 * Kevin Graney
 */

/** 
 * @file builtins.c
 * @brief Definitions of builtin functions.
 * @author Kevin Graney
 * @version v0.1
 * @date 2009-08-02
 */
#include "builtins.h"
#include "get_path.h"
#include "alias.h"
#include "wildcard.h"
#include "watchmail.h"

#include <readline/readline.h>

extern int errno;

//------------------------------------------------------------------------------
//-- The following constants define the built-in commands.  The commands are
//-- matched to the function pointers with a one-to-one matching done in order.
//------------------------------------------------------------------------------

/** 
 * @brief Stores the commands that map to the built-in functions.
 *
 * These strings are what, if entered as the zeroth argument (argv[0]) in a
 * command will execute a built-in function.
 */
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
    "alias",
    "unalias",         // Not a requirement, but easy to add. 
    "history", 
    "setenv",
    "lsbuiltins",
    "watchmail",
    "noclobber",
    "vimode",
    "emacsmode"
#ifdef DEBUG        // Various built ins defined for debugging purposes.
    ,
    "_db_tokenizer",
    "_db_kgenv",
    "_db_path",
    "_db_history",
    "_db_wc_contains",
    "_db_wc_expand"
#endif //DEBUG
};


/** 
 * @brief An array of function pointers for built-in commands.
 
 * These function pointers map one-to-one in order with the command strings in 
 * ::BUILT_IN_COMMANDS.  Each built-in command function has the same prototype.
 * Setting the prototypes up in this way allows us to write each built-in as if
 * it were a "main" functions of a seperate program with access to the ::kgenv
 * structure.  Adding new built-ins is very easy.
 * 
 * @param env The global ::kgenv structure is the first argument to every 
 * built-in command.
 * @param argc The second argument is always the argument count of the command 
 * being proccessed. 
 * @param argv The third argument is always the argument value array of the 
 * command being processed.
 */
void (*BUILT_IN_FUNCS[])(kgenv* env, int argc, char** argv) = {
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
    bic_setenv,
    bic_lsbuiltins,
    bic_watchmail,
    bic_noclobber,
    bic_vimode,
    bic_emacsmode
#ifdef DEBUG        // various built ins defined for debugging purposes
        ,
    _db_tokenizer,
    _db_kgenv,
    _db_path,
    _db_history,
    _db_wc_contains,
    _db_wc_expand
#endif //DEBUG
};


/** 
 * @brief Checks if a command is a built-in command.
 * 
 * Loops through ::BUILT_IN_COMMANDS comparing the command parameter to each
 * string of ::BUILT_IN_COMMANDS.  When a match is found the index plus one is
 * returned.
 *
 * @param command The command to check.
 * 
 * @return Returns 0 if the command is not built-in, and a positive value that
 * is one greather than the index of the function in the built-in definitions
 * arrays (::BUILT_IN_COMMANDS and ::BUILT_IN_FUNCS) if the command is built-in.
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

/** 
 * @brief Built-in exit command.
 *
 * Exits with status 0;
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
void bic_exit(kgenv* env, int argc, char* argv[]){
    exit(0);
}

/** 
 * @brief Built-in which command.
 *
 * Displays the full path to the executable that will be executed for each
 * command that is given as an argument.  The path printed is the first one that
 * occurs in the PATH environment variable that contains a file of the correct
 * name with execute permissions.
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
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

/** 
 * @brief Built-in where command.
 *
 * Same as the which command, but displays all of the possible paths where a
 * file of the correct name with executable permissions exist within the PATH
 * envrionment variable list of paths.
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
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
        if(dirp == NULL){
        perror("Error in where");
        return;
        } else {
        struct dirent* dp = readdir(dirp); //TODO: check errno?

        // While loop executed once for each file in directory
        while(dp != NULL){
            if(strcmp(dp->d_name, cmd) == 0){
            printf("%s/%s\n", pl->element, cmd);
            }
            dp = readdir(dirp);
        }

        if(closedir(dirp) == -1){
            perror("Error in where");
            return;
        }
        }
        pl = pl->next;
    }
    }
}

/** 
 * @brief Built-in cd command.
 *
 * Changes the current working directory using the chdir library function.  When
 * called with no arguments, changes to the user's home directory.  When called
 * as "cd -", changes to the previous directory. 
 *
 * Before switching, the previous directory and the current directory are set in
 * the global ::kgenv environment object.  The user's home directory is also
 * retrievable from this object.
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
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
    if(env->cwd == NULL){
        perror("Error in cd");
        return;
    }
    strcpy(env->cwd, env->homedir);
    
    // Use chdir to change the working directory
    if(chdir(env->cwd) != 0) {
        perror("Error in chdir");
    }

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
    if(chdir(env->cwd) != 0) {
        perror("Error in chdir");
    }

    }

    else

    //** Otherwise we have either a relative or absolute path to a directory
    {
    // Change to the path specified in the argument
    if(chdir(argv[1]) != 0){
        perror("Error in chdir");
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

/** 
 * @brief Built-in pwd command.
 *
 * Prints the current working directory to stdout.
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
void bic_pwd(kgenv* env, int argc, char* argv[]){

    // Print the current working directory
    printf("%s\n", env->cwd);

}

/** 
 * @brief Built-in list command.
 *
 * Lists files in the directores specified as arguments.
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
// TODO: reverse order of printout
void bic_list(kgenv* env, int argc, char* argv[]){
    DIR* dirp;            // directory pointer
    bool cwd_mode = false;    // true if passed with no args

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
        perror("Error in list");
        return;

    } else {

        // This loop iterates through the directory stream.
        struct dirent* dp = readdir(dirp);    //TODO: check errno?
        while(dp != NULL){
        printf("%s\n", dp->d_name);
        dp = readdir(dirp);
        }

        if(closedir(dirp) == -1){
        perror("Error in list");
        return;
        }
    }

    }
}


/** 
 * @brief Built-in pid command.
 *
 * Prints the process id (pid) of the shell.
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
void bic_pid(kgenv* env, int argc, char* argv[]){
    pid_t pid = getpid();
    if(pid == -1){        //TODO: check error condition
    perror("Error in pid");
    return;
    }

    printf("%d\n", pid);
}


/** 
 * @brief Built-in kill command.
 * 
 * Sends a SIGTERM signal to the pid specified in the arguments.  If a -n is
 * passed, the signal number n is passed to the specified process.
 *
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
void bic_kill(kgenv* env, int argc, char* argv[]){
    int pid;            ///< PID of the process to send signal to
    int signal = SIGTERM;    ///< Default signal is SIGTERM

    errno = 0;

    // Called with no arguments
    if(argc == 1){
    fprintf(stderr, "kill: Too few arguments.\n");
    return;
    }

    if(argc == 2){        // Called with just a pid

        pid = atoi(argv[1]);

    if(errno != 0){
        perror("Error in kill");
        return;
    }

    } else if(argc == 3){    // Called with a signal specified

        pid = atoi(argv[2]);
        signal = atoi(argv[1] + 1);    // Add one to remove hyphen

        if(errno != 0){
            perror("Error in kill");
            return;
        }

    } else {            // Called with too many arguments
        fprintf(stderr, "kill: Too many arguments.\n");
        return;
    }    

    sigsend(P_PID, pid, signal);
    //printf("Sending code %d to pid %d\n", signal, pid);

    // Send the kill signal
    if(kill(pid, signal) == -1){
        perror("Error in kill");
    }

}


/** 
 * @brief Built-in prompt command.
 *
 * Changes the prompt prefix to the specified argument.  If no argument is
 * passed, prompts the user for a prefix.
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
void bic_prompt(kgenv* env, int argc, char* argv[]){
    char* new_prompt;        // the new prompt string

    // Case where we are passed arguments.
    if(argc > 1){    
    new_prompt = argv[1];
    strcpy(env->prompt, new_prompt);
    return;
    }

    // Case where we prompt user for input.
    printf("New prompt prefix: ");
    char* prompt_in = (char*)malloc(LINE_BUFFER_SIZE);
    if(prompt_in == NULL){
    perror("Error in prompt");
    return;
    }

    fgets(prompt_in, LINE_BUFFER_SIZE, stdin);

    // Need to remove trailing newline from input.
    if(prompt_in[strlen(prompt_in) - 1] == '\n'){
    prompt_in[strlen(prompt_in) - 1] = '\0';
    }

    // Save some heap by re-allocating only what's needed.
    new_prompt = (char*)malloc(strlen(prompt_in) + 1);
    if(new_prompt == NULL){
    perror("Error in prompt");
    return;
    }

    strcpy(new_prompt, prompt_in);
    env->prompt = new_prompt;
    free(prompt_in);
}


/** 
 * @brief Built-in printenv command.
 *
 * Prints out a list of environment variables and their values.
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
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
    } else {
        fprintf(stderr, "%s was not found in the current environment\n",
            argv[1]);
    }
    }


    // Called with more than one argument
    else {
    fprintf(stderr, "printenv: Too many arguments.\n");
    }

}


/** 
 * @brief Built-in alias command.
 *
 * When run with no arguments prints a list of aliases currently in the alias
 * list.  When ran with arguments sets the alias name in the first argument to
 * the command specified in subsequent arguments.
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
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


/** 
 * @brief Built-in unalias command.
 *
 * Removes an alias from the alias list. 
 *
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
void bic_unalias(kgenv* env, int argc, char* argv[]){
    //TODO: support multiple arguments
    if(argc == 2){
    remove_alias(env, argv[1]);
    }
}


/** 
 * @brief Built-in history command.
 * 
 * When run with no arguments, prints out the last 10 commands run.  When an
 * argument is passed, that number of commands is printed.
 *
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
//TODO: fix history
void bic_history(kgenv* env, int argc, char* argv[]){
    int num_items = 0;    // Number of commands to print

    // We default to printing 10 commands if no argument is passed
    if(argc == 1){
        num_items = 10;
    } else {
        errno = 0;
        num_items = atoi(argv[1]);
        if(errno != 0){
            perror("Error in history");
            return;
        }
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
    
    j++;    // Need to increment j to adjust for final decrement
    for(int i=j; i < num_items; i++){
    printf("%d: %s\n", outbuf[i]->num, outbuf[i]->command);
    }
}


/** 
 * @brief Built-in setenv command.
 *
 * When run with no arguments prints a list of environment variables and values.
 * When run with two arguments, sets the variable in the first argument equal to
 * the value in the second argument.
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
void bic_setenv(kgenv* env, int argc, char* argv[]){

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

/** 
 * @brief Built-in lsbuiltins command.
 *
 * Lists all built-in functions.  Ignores any arguments passed.
 * (Not a project requirement.)
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
void bic_lsbuiltins(kgenv* env, int argc, char* argv[]){

    for(int i=0; i < NUM_BUILTINS; i++){
        printf("%s\n", BUILT_IN_COMMANDS[i]);
    }

}

/** 
 * @brief Built-in watchmail command.
 *
 * Watches for new mail in the specified file.  Prints a message and beeps when
 * mail is received.
 * //TODO: add more detailed documentation
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
void bic_watchmail(kgenv* env, int argc, char* argv[]){

    char* file;            //<< The file to watch

    int action;            //<< The new stop/start action on the file
    enum { START, STOP };    //<< Two states for the action variable


    //## Print out the help message if run with no arguments
    if(argc == 1 || argc > 3){
        printf("watchmail:\n\n\twatchmail [file] [on/off]\n\n");
        return;
    }

    file = (char*)malloc(strlen(argv[1]) + 1);    // Aliased in watchmails list 
                        // (watch free)
    strcpy(file, argv[1]);

    //## If run with only one argument, we're starting a watchmail on the file
    if(argc == 2){
        action = START;
    }
    
    //## If run with two arguments, parse the second to find action needed    
    else if(argc == 3){

    if(strcmp(argv[2], "off") == 0){
        action = STOP;
    } else if(strcmp(argv[2], "on") == 0){
        action = START;
    } else {
        fprintf(stderr, "watchmail: Invalid command.\n");
        return;
    }

    }


    //## Take the action to start/stop watchmail    
    if(action == START){

        // Start watching the file
    printf("Starting watchmail for %s\n", file);
        control_watchmail(file, false, env);

    } else if(action == STOP){

        // Stop watching the file
    printf("Stopping watchmail for %s\n", file);
        control_watchmail(file, true, env);

    }

}

/** 
 * @brief Built-in noclobber command.
 *
 * Changes the value of the clobber variable.
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
void bic_noclobber(kgenv* env, int argc, char* argv[]){
    env->noclobber = !env->noclobber;
    printf("Clobbering is now %s. (value is %d)\n", 
            env->noclobber ? "off" : "on",
            env->noclobber);
}

/** 
 * @brief Sets command line editing to vi mode.
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
void bic_vimode(kgenv* env, int argc, char* argv[]){
    rl_editing_mode = 0;
}

/** 
 * @brief Sets command line editing to emacs mode. 
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
void bic_emacsmode(kgenv* env, int argc, char* argv[]){
    rl_editing_mode = 1;
}

//------------------------------------------------------------------------------
//-- Definitions of debug functions
//------------------------------------------------------------------------------

#ifdef DEBUG

/** 
 * @brief Debugs the tokenizer by showing argument count and argument values for
 * the arguments passed to ::_db_tokenizer.
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
void _db_tokenizer(kgenv* env, int argc, char* argv[]){
    printf("argc = %d\n", argc);
    for(int i=0; i<argc; i++){
        printf("argv[%d] = %s\n", i, argv[i]);
    }
}


/** 
 * @brief Prints out some of the variables in the global ::kgenv environment
 * object for debugging purposes.
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
void _db_kgenv(kgenv* env, int argc, char* argv[]){

    printf("uid=%d\n", env->uid);
    printf("homedir=%s\n", env->homedir);

    printf("cwd=%s\n", env->cwd);
    printf("pwd=%s\n", env->pwd);
    
    printf("prompt=%s\n", env->prompt);
}


/** 
 * @brief Prints out path list for debugging purposes.
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
void _db_path(kgenv* env, int argc, char* argv[]){
    pathList* p = env->path;
    while (p != NULL){
        printf("%s\n", p->element);
        p = p->next;
    }
}


/** 
 * @brief Prints out entire history list for debugging purposes.
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
void _db_history(kgenv* env, int argc, char* argv[]){
    histList* h = env->hist;
    while (h != NULL){
        printf("%d: %s\n", h->num, h->command);
        h = h->next;
    }
}


/** 
 * @brief Prints "true" if the first argument contains a wildcard and "false" if
 * it does not.  Used to debug ::contains_wildcards.
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
void _db_wc_contains(kgenv* env, int argc, char* argv[]){
    printf("%s\n", contains_wildcards(argv[1]) ? "true":"false");
}


/** 
 * @brief Prints the expanded version of the first argument.  Used to debug
 * ::expand_argument.
 * 
 * @param env A pointer to the global ::kgenv environment object.
 * @param argc The argument count for the command entered.    
 * @param argv[] The argument values for the command entered.
 */
void _db_wc_expand(kgenv* env, int argc, char* argv[]){
    printf("%s\n", expand_argument(argv[1]));
}

#endif //DEBUG
