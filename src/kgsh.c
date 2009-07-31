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
#include <errno.h>	// for extern errno

#include "builtins.h"
#include "util.h"
#include "get_path.h"


extern int errno;	// From errno.h 

void initialize_environment(kgenv* env);

void sig_interrupt(int signal);

int main(int argc, char* argv[]){

    // The global environment structure
    kgenv global_env;
    
    // Populate the global environment for the first time
    initialize_environment(&global_env);

    // Set the interrupt function to catch SIGINT (Ctrl-C)
    sigset(SIGINT, sig_interrupt);
    sigignore(SIGTSTP); 		// Ignore Ctrl-Z
    sigignore(SIGTERM);

    char*  line_in = NULL; // Stores the command entered (pointed to by in_argv)

    // The main loop that is executed once for each command prompt.
    while(1){

	// Free up memory from the previous iteration.  Done here since we have
	// various continues throughout the loop.
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
	process_command_in(line_in, &global_env);
    }
}

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

void sig_interrupt(int signal){
    printf("\n");
    // TODO: determine if anything special needs to be done to forward SIGINT to
    // a child process
}
