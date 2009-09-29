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

int control_watchmail(char* file, bool disable){

    static pthread_t thread;

    if(!disable){
        pthread_create(&thread, NULL, watchmail_thread, (void*)file);
    }

    return 0;
}


void* watchmail_thread(void* param){

    while(1) {
        printf("watchmail running...\n");
        sleep(5);
    }

    return NULL;
}
