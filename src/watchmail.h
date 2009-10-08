/*
 * CISC361: Operating Systems (Fall 2009)
 * Instructor: Ben Miller
 *
 * Project 2
 * Kevin Graney
 */

/** 
 * @file watchmail.h
 * @brief Contains prototypes for watchmail builtin functions
 * @author Kevin Graney
 * @version v0.1
 * @date 2009-09-29
 */
#ifndef _WATCHMAIL_INC
#define _WATCHMAIL_INC

int control_watchmail(char* file, bool disable, kgenv* env);

void* watchmail_thread(void*);

#endif //_BUILTINS_INC
