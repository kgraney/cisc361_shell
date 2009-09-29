/*
 * CISC361: Operating Systems (Fall 2009)
 * Instructor: Ben Miller
 *
 * Project 2
 * Kevin Graney
 */

/** 
 * @file wildcard.c
 * @brief Definitions of wildcard functions.
 * @author Kevin Graney
 * @version v0.1
 * @date 2009-08-02
 */
#include "util.h"
#include "wildcard.h"


/** 
 * @brief Determines if an input string contains a wildcard.
 *
 * Used in process_command_in() to determine if wildcard processing can be 
 * bypassed or not.  Wildcards checked for are defined in ::WILDCARD_CHARS.
 * 
 * @param line The input command line to check for a wildcard in.
 * 
 * @return True if a wildcard is present.  False otherwise.
 */
bool contains_wildcards(char* line){

    //## Search the line for any of the wildcard characters.  Return true at the
    //## first match.
    for(int i=0; WILDCARD_CHARS[i] != '\0'; i++){
        if(strchr(line, WILDCARD_CHARS[i]) != NULL){
            return true;
        }
    }	

    return false;	// We didn't find any wildcards.
}


/** 
 * @brief Expands the wildcards present in an input string.
 *
 * Expands all the wildcards present in the input string based on the current
 * working directory.  First the line is parsed into an argv array and each
 * argument is expanded individually using glob(3C).  Next the expanded
 * arguments are combined back in order to form a single expanded string.
 *
 * \note The return value from this function is a pointer to the heap.  The
 * returned pointer should be freed when not needed anymore.
 *
 * @param line The line to expand.
 * 
 * @return The expanded version of line.
 */
char* expand_wildcards(char* line){

    //## Parse the line into arguments
    int    argc;
    char** argv;
    bool   background;

    argv = (char**)calloc(MAX_TOKENS_PER_LINE, sizeof(char*));
    if(argv == NULL){
	perror("Error while expanding wilcards");
	return NULL;
    }
    parse_line(&argc, &argv, &background, line);

    //## Expand all the arguments individually
    char** expanded_argv = (char**)calloc(argc, sizeof(char*));
    if(expanded_argv == NULL){
	perror("Error while expanding wilcards");
	return NULL;
    }

    for(int i=0; i < argc; i++){
        expanded_argv[i] = expand_argument(argv[i]);
    }

    //## Find the total length the expanded line will be
    int length = 0;
    for(int i=0; i < argc; i++){
        length += strlen(expanded_argv[i]) + 1;
    }

    //## Form expanded line by concatenating all the expanded arguments
    char* expanded = calloc(length, sizeof(char));
    if(expanded == NULL){
	perror("Error while expanding wilcards");
	return NULL;
    }

    for(int i=0; i < argc; i++){
        strcat(expanded, " ");
        strcat(expanded, expanded_argv[i]);
        free(expanded_argv[i]);
    }

    //## Free up memory
    free(expanded_argv);
    free(argv);

    return expanded;
}



/** 
 * @brief Expands wildcards in a single argument string.
 *
 * Called by expand_wildcards(), this function expands a single argument in the
 * argv array by calling glob(3C).
 *
 * \note This function returns a pointer to the heap, therefore the pointer must
 * be freed after use.  A copy of the input argument is returned if no wildcards
 * are present to prevent issues with deallocating memory that is allocated
 * outside this function.
 * 
 * @param argument The argument string to be expanded.
 * 
 * @return The expanded argument.  If no wildcards are present in the string a
 * copy of the argument parameter is returned.
 */
char* expand_argument(char* argument){

    glob_t pglob;

    if(glob(argument, 0, NULL, &pglob) == 0){	//TODO: errno handling

        //## If no wildcard in the argument return a copy of itself
        if(pglob.gl_pathc == 0){
            char* argument_copy = malloc(strlen(argument) + 1);
	    if(argument_copy == NULL){
		perror("Error expanding argument");
		globfree(&pglob);
		return NULL;
	    }

            strcpy(argument_copy, argument);
	    globfree(&pglob);		// Free up memory
            return argument_copy;
        }

        //## Determine total length of expanded argument
        int length = 0;
        for(int i=0; i < pglob.gl_pathc; i++){
            length += strlen(pglob.gl_pathv[i]) + 1;
        }

        //## Allocate new space for the expanded argument
        char* expanded_arg = calloc(length, sizeof(char));
	if(expanded_arg == NULL){
	    perror("Error expanding argument");
	    globfree(&pglob);
	    return NULL;
	}

        //## Form expanded argument string
        for(int i=0; i < pglob.gl_pathc; i++){
            strcat(expanded_arg, " ");
            strcat(expanded_arg, pglob.gl_pathv[i]);
        } 

        //## Free up memory 
        globfree(&pglob);

        return expanded_arg;

    } else {

        //## Make a copy of the argument and return
        char* argument_copy = malloc(strlen(argument) + 1);
	if(argument_copy == NULL){
	    perror("Error expanding argument");
	    globfree(&pglob);
	    return NULL;
	}

	//## Free up memory
        globfree(&pglob);

        strcpy(argument_copy, argument);
        return argument_copy;
    }
}
