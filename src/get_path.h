/*
 * CISC361: Operating Systems (Fall 2009)
 * Instructor: Ben Miller
 *
 * Project 2
 * Kevin Graney
 */

/* 
  get_path.h
  Ben Miller

*/
#ifndef _GET_PATH_INC
#define _GET_PATH_INC

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/* function prototype.  It returns a pointer to a linked list for the path
   elements. */
struct pathelement *get_path();

struct pathelement
{
  char *element;			/* a dir in the path */
  struct pathelement *next;		/* pointer to next node */
};


#endif //_GET_PATH_INC
