#include "wildcard.h"


char* expand_wildcards(char* line, kgenv* env){
    char* expanded = malloc(strlen(line) + 1);
    strcpy(expanded, line);
    return expanded;
}



bool check_wildcard(char* pattern, char* string){

    //** Tokenize the pattern by the wildcard character to determine what 
    //** substrings we're searching for, and the category of each substring
    //** (single or all).

    char* substring[MAX_WILDCARDS + 1];

    char* strtok_ptr = strtok(pattern, WILDCARD_CHARACTER);
    for(int i=0; i < MAX_WILDCARDS + 1 && strtok_ptr != NULL; i++){

	substring[i] = strtok_ptr;
	strtok_ptr = strtok(pattern, WILDCARD_CHARACTER);

    }


    return false;

}
