/*
 * CISC361: Operating Systems (Fall 2009)
 * Instructor: Ben Miller
 *
 * Project 2
 * Kevin Graney
 */

/** 
 * @file watchmail.c
 * @brief Contains functions to provide functionality for watchmail builtin
 * @author Kevin Graney
 * @version v0.1
 * @date 2009-09-29
 */
#include "types.h"
#include "watchmail.h"

#include <pthread.h>

//TODO: error checking
int control_watchmail(char* file, bool disable, kgenv* env){

    if(!disable){

        // Create a new node in the watchmails linked list
        watchmailList* new_node = (watchmailList*)malloc(sizeof(watchmailList));
        new_node->filename = file; 
        new_node->next = env->watchmails;
        env->watchmails = new_node;

        // Spawn a thread to monitor the new file
        pthread_create(&(new_node->thread), NULL, watchmail_thread, 
                (void*)file);

    } else {
        
        // Loop through watchmail linked list to find the pthread_t record
        watchmailList* prev = NULL;
        watchmailList* curr = env->watchmails;

        while(curr != NULL){
            if(!strcmp(curr->filename, file)){
                break;
            }

            prev = curr;
            curr = curr->next;
        }

        pthread_cancel(curr->thread);

        // Delete the node from the watchmails linked list
        if(prev != NULL){
            prev->next = curr->next;
        } else {
            env->watchmails = curr->next;
        }
            
    }


    return 0;
}


void* watchmail_thread(void* param){

    while(1) {
        printf("\nwatchmail running for file %s...\n", (char*)param);
        sleep(5);
    }

    return NULL;
}
