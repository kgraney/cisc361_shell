/*
 * CISC361: Operating Systems (Fall 2009)
 * Instructor: Ben Miller
 *
 * Project 1
 * Kevin Graney
 */

/** 
 * @file kgsh.c
 * @brief kgsh main file
 * @author Kevin Graney
 * @version v0.1
 * @date 2009-08-02
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <limits.h>

#include <unistd.h>	// for access
#include <signal.h>

#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "builtins.h"
#include "util.h"
#include "get_path.h"

void initialize_environment(kgenv* env);

void sig_interrupt(int signal);

int main(int argc, char* argv[]){

    kgenv global_env; // The global environment structure
    
    // Populate the global environment for the first time
    initialize_environment(&global_env);

    sigset(SIGINT, sig_interrupt);	// Interrupt function for Ctrl-C 
    sigignore(SIGTSTP); 		// Ignore Ctrl-Z
    sigignore(SIGTERM);

    char*  line_in = NULL; // Stores the command entered (pointed to by in_argv)

    // The main loop that is executed once for each command prompt.
    while(1){

	// Free up memory from the previous iteration.
	if(line_in != NULL)
	    free(line_in);

	//## Print the shell prompt
	printf("%s %s> ", global_env.prompt, global_env.cwd);


	//## Read the a line from the shell
	// NOTE: line_in can not be freed until after the loop is done
	// executing since in_argv points to memory allocated here.  Memory
	// allocation is only done _once_ for the input string.
	line_in = (char*)calloc(LINE_BUFFER_SIZE, sizeof(char));
	if(!line_in){
	    perror("Not enough heap");
	    exit(1);
	}
	fgets(line_in, LINE_BUFFER_SIZE, stdin);

	//## Parse the command and execute the appropriate action
	process_command_in(line_in, &global_env, false);
    }
}

/** 
 * @brief Initializes the kgenv global environment.
 *
 * This function is only called once at startup to populate the singleton
 * instance of the kgenv stuct.
 * 
 * @param env A pointer to the global environment instance.
 */
void initialize_environment(kgenv* env){

    char* cwd;
    cwd = getcwd(NULL , CWD_BUFFER_SIZE);

    if(cwd == NULL){
	perror("Can't get current working directory\n");
	exit(2);
    }

    env->cwd = cwd;
    env->pwd = "";
    env->prompt = "";
    env->uid = getuid();
    env->pword_entry = getpwuid(env->uid);
    env->homedir = env->pword_entry->pw_dir;
    env->path = get_path();
    env->hist = NULL;
    env->aliases = NULL;

}

/** 
 * @brief Function executed when SIGINT (Ctrl-C) is caught. 
 * 
 * @param signal Signal passed in.  Currently it's always SIGINT.
 */
void sig_interrupt(int signal){
    printf("\n");
    // TODO: determine if anything special needs to be done to forward SIGINT to
    // a child process
}
