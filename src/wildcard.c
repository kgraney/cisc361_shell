#include "util.h"
#include "wildcard.h"

bool contains_wildcards(char* line){

    for(int i=0; WILDCARD_CHARS[i] != '\0'; i++){
	if(strchr(line, WILDCARD_CHARS[i]) != NULL){
	    return true;
	}
    }	

    return false;
}

char* expand_wildcards(char* line, kgenv* env){

    int    argc;
    char** argv;

    argv = (char**)calloc(MAX_TOKENS_PER_LINE, sizeof(char*));
    parse_line(&argc, &argv, line);

    // Expand all the arguments
    char** expanded_argv = (char**)calloc(argc, sizeof(char*));
    for(int i=0; i < argc; i++){
	expanded_argv[i] = expand_argument(argv[i]);
    }

    // Find the total length of the expanded line 
    int length = 0;
    for(int i=0; i < argc; i++){
	length += strlen(expanded_argv[i]) + 1;
    }

    // Form expanded line
    char* expanded = calloc(length, sizeof(char));
    for(int i=0; i < argc; i++){
	strcat(expanded, " ");
	strcat(expanded, expanded_argv[i]);
	// TODO: free
    }

    //char* expanded = malloc(strlen(line) + 1);
    //strcpy(expanded, line);
    return expanded;
}

char* expand_argument(char* argument){

    glob_t pglob;


    printf("globbing %s\n", argument);
    if(glob(argument, 0, NULL, &pglob) == 0){

	// If no wildcard in the argument return itself
	if(pglob.gl_pathc == 0){
	    return argument;
	}

	// Determine total length of expanded argument
	int length = 0;
	for(int i=0; i < pglob.gl_pathc; i++){
	    length += strlen(pglob.gl_pathv[i]) + 1;
	}

	// Allocate new space for the expanded argument
	char* expanded_arg = calloc(length, sizeof(char));

	// Form expanded argument string
	for(int i=0; i < pglob.gl_pathc; i++){
	    strcat(expanded_arg, " ");
	    strcat(expanded_arg, pglob.gl_pathv[i]);
	} 
	printf("expanded = %s\n", expanded_arg);

	// Free glob space
	globfree(&pglob);

	return expanded_arg;

    }
}
