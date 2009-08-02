#include "util.h"
#include "wildcard.h"

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

char* expand_wildcards(char* line, kgenv* env){

    //## Parse the line into arguments
    int    argc;
    char** argv;

    argv = (char**)calloc(MAX_TOKENS_PER_LINE, sizeof(char*));
    parse_line(&argc, &argv, line);

    //## Expand all the arguments individually
    char** expanded_argv = (char**)calloc(argc, sizeof(char*));
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

char* expand_argument(char* argument){

    glob_t pglob;

    if(glob(argument, 0, NULL, &pglob) == 0){

	//## If no wildcard in the argument return a copy of itself
	if(pglob.gl_pathc == 0){
	    char* argument_copy = malloc(strlen(argument) + 1);
	    strcpy(argument_copy, argument);
	    return argument_copy;
	}

	//## Determine total length of expanded argument
	int length = 0;
	for(int i=0; i < pglob.gl_pathc; i++){
	    length += strlen(pglob.gl_pathv[i]) + 1;
	}

	//## Allocate new space for the expanded argument
	char* expanded_arg = calloc(length, sizeof(char));

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
	strcpy(argument_copy, argument);
	return argument_copy;
    }
}
