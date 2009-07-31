#include "alias.h"
#include "util.h"

void add_alias(kgenv* env, char* name, int cmd_argc, char* cmd_argv[]){

    // Allocate space for the new alias
    aliasList* new_alias = malloc(sizeof(aliasList));

    // Delete any existing alias with the same name
    remove_alias(env, name);

    // Copy over the alias name
    new_alias->name = (char*)malloc(strlen(name) + 1);
    strcpy(new_alias->name, name);

    // Copy over the argv arrray and reconstruct the command line string so
    // the recursive calls work out correctly when processing the alias.  We're
    // doing some unecessary processing using this method, but the
    // implementation is neater.
    int line_length = 0;
    for(int i=0; i < cmd_argc; i++){
	line_length += strlen(cmd_argv[i]);
	line_length++;	// For null character
    }
    
    new_alias->string = malloc(line_length);
    memcpy(new_alias->string , *cmd_argv, line_length);

    detokenize(new_alias->string, line_length);

    // Add the link to the next node
    new_alias->next = env->aliases;
   

    env->aliases = new_alias;
}



aliasList* is_alias(kgenv* env, char* name){
    aliasList* a = env->aliases;

    while(a != NULL){

	if(strcmp(name, a->name) == 0){
	    return a;
	}

	a = a->next;
    }
    return NULL;
}

void remove_alias(kgenv* env, char* name){

    aliasList* a = env->aliases;
    aliasList* prev = NULL;

    while(a != NULL){

	if(strcmp(name, a->name) == 0){

	    if(prev != NULL){
		prev->next = a->next;
	    } else {
		env->aliases = a->next;
	    }

	    free(a->name);
	    free(a->string);
	    free(a);
	    return;

	}

	prev = a;
	a = a->next;
    }
}
