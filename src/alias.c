/*
 * CISC361: Operating Systems (Fall 2009)
 * Instructor: Ben Miller
 *
 * Project 1
 * Kevin Graney
 */

/** 
 * @file alias.c
 * @brief Definition of alias functions.
 * @author Kevin Graney
 * @version v0.1
 * @date 2009-08-02
 */
#include "alias.h"
#include "util.h"

/** 
 * @brief Adds an alias to the alias list.
 *
 * Inserts an alias into the linked list structure that stores aliases
 * (::aliasList).  A call to detokenize() is made since aliases are stored as
 * unparsed commands.  Any existing alias with the same name will be overwritten
 * when this function is called.
 * 
 * @param env           A pointer to the global ::kgenv object.
 * @param name          The name of the alias.
 * @param cmd_argc      The argument count for the command the alias points to. 
 * @param cmd_argv[]    The argument values for the command the alias points to.
 */
void add_alias(kgenv* env, char* name, int cmd_argc, char* cmd_argv[]){

    // Allocate space for the new alias
    aliasList* new_alias = malloc(sizeof(aliasList));
    if(new_alias == NULL){
	perror("Failed to add alias");
	return;
    }

    // Delete any existing alias with the same name
    remove_alias(env, name);

    // Copy over the alias name
    new_alias->name = (char*)malloc(strlen(name) + 1);
    if(new_alias->name == NULL){
	perror("Failed to add alias");
	return;
    }
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
    if(new_alias->string == NULL){
	perror("Failed to add alias");
	return;
    }

    memcpy(new_alias->string , *cmd_argv, line_length);

    detokenize(new_alias->string, line_length);

    // Add the link to the next node
    new_alias->next = env->aliases;
   

    env->aliases = new_alias;
}



/** 
 * @brief Checks if a command entered is an alias.
 * 
 * @param env   A pointer to the global ::kgenv object.
 * @param name  The name to check.  This should be argv[0] of the command that's
 * been entered.  This function does not parse an entire command line.
 * 
 * @return  If an alias exists with the name, a pointer to the ::aliasList node
 * where the alias is stored is returned.  Otherwise NULL is returned if the
 * alias does not exist.
 */
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

/** 
 * @brief Removes an alias if it exists.
 *
 * Steps through the alias list (::aliasList) stored in the global ::kgenv
 * object.  If an alias of the specified name is found, it is removed from the
 * list.  No action is taken if an alias with the name is not found.
 * 
 * @param env   The global ::kgenv environment object.
 * @param name  The name of the alias to remove.
 *
 * @return  True if an alias was removed.  False otherwise.
 */
bool remove_alias(kgenv* env, char* name){

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
            return true;

        }

        prev = a;
        a = a->next;
    }

    return false;
}
