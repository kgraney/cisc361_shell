#ifndef _TYPES_INC
#define _TYPES_INC

#include "get_path.h"

// We'll fake some C++ style booleans.
#define false 0
#define true  1
typedef char bool;


//------------------------------------------------------------------------------
//-- Linked List Types
//------------------------------------------------------------------------------


// Struct to represent each node in the history linked list.
struct histelement {
    int	   num;			// Command number
    char*  command;		// Command string
    struct histelement* next;	// Pointer to next node (previous command)
};
typedef struct histelement histList;

// The pathelement struct is defined in get_path.h.  We defined a typedef to be
// consistent with the other linked lists. 
typedef struct pathelement pathList;

// Struct to represent each node in the aliases linked list.  Note the commands
// are stored in their parsed condition to avoid reparsing.
struct aliaselement {
    char*  name;		// The name of the alias
    char*  string;		// Input string alias is to
    struct aliaselement* next;	// Pointer to next node (previous command)
};
typedef struct aliaselement aliasList;


//------------------------------------------------------------------------------
//-- Environment Types
//------------------------------------------------------------------------------


// The kgenv type will contain our current environment.  If this were being done
// in C++ it would be a singleton class since we only ever create one variable 
// of this type.  Basically, we're encapsulating all our would be global 
// variables into a nice neat structure.
typedef struct {
    int uid;			// user id
    char* homedir;		// home directory path
    struct passwd *pword_entry;	// passwd entry info

    char* cwd;	  		// current working directory
    char* pwd;    		// prior working directory

    char* prompt; 		// prompt prefix string
    pathList* path;		// path list
    histList* hist;		// history list
    aliasList* aliases;		// alias list
} kgenv;


//------------------------------------------------------------------------------
//-- Function Types
//------------------------------------------------------------------------------

// This is the generic function type for a built in function.  It's used to
// setup the function pointer arrays.
typedef void (*bicfunc)(kgenv*, int, char*);

#endif //_TYPES_INC
