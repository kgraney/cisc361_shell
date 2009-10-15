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

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

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
                (void*)(new_node->filename));

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

        // Case where a thread doesn't exist for the file
        if(curr == NULL){
            fprintf(stderr, "No watchmail thread for %s exists!\n", file);
            return -1;
        }

        pthread_cancel(curr->thread);

        // Delete the node from the watchmails linked list
        if(prev != NULL){
            prev->next = curr->next;
        } else {
            env->watchmails = curr->next;
        }

        // Free the memory allocated for the node in the linked list
        free(curr->filename);
        free(curr);
            
    }


    return 0;
}


void* watchmail_thread(void* param){

    char* filename = (char*)param;

    struct stat stat_info;
    off_t last_size;          ///< The file size from the last iteration

    stat(filename, &stat_info);
    last_size = stat_info.st_size;

    // Closed loop to monitor file
    while(1) {
        stat(filename, &stat_info);

        if(stat_info.st_size > last_size){

            struct timeval tp;
            gettimeofday(&tp, NULL);

            printf("\n\aYou have new mail in %s at %s\n", filename, 
                    ctime(&(tp.tv_sec)));

        }

        last_size = stat_info.st_size;
        sleep(1);
    }

    return NULL;
}
