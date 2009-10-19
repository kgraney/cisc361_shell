/*
 * CISC361: Operating Systems (Fall 2009)
 * Instructor: Ben Miller
 *
 * Project 2
 * Kevin Graney
 */

/** 
 * @file types.h
 * @brief Type declarations.
 * @author Kevin Graney
 * @version v0.1
 * @date 2009-08-02
 */
#ifndef _TYPES_INC
#define _TYPES_INC

#include "get_path.h"

#define false 0     ///< C++ style false keyword
#define true  1     ///< C++ style true keyword
typedef char bool;  ///< Fake boolean in C++ style


//------------------------------------------------------------------------------
//-- Linked List Types
//------------------------------------------------------------------------------


/** 
 * @brief Struct to represent each node in the history linked list.
 */
struct histelement {
    int	   num;			///< Command number
    char*  command;		///< Command string
    struct histelement* next;	///< Pointer to next node (previous command)
};

/** 
 * @brief Typedef to refer to the history linked list. 
 */
typedef struct histelement histList;

/** 
 * @brief Structure to represent each node in the aliases linked list.  Take
 * note that the commands are stored in their unparsed condition to make
 * the code cleaner.
 */
struct aliaselement {
    char*  name;		///< The name of the alias
    char*  string;		///< Command string alias refers to
    struct aliaselement* next;	///< Pointer to next node
};

/**
 * @brief Typedef to refer to the alias linked list.
 */
typedef struct aliaselement aliasList;

/** 
 * @brief List of files and pthread_t structures that are currently being
 * watched by the ::watchmail builtin.
 */
struct watchmailelement {
    char* filename;             ///< Path to the file being watched
    pthread_t thread;           ///< ::thread_t structure for ::watchmail_thread
    struct watchmailelement* next; ///< Pointer to next node
};

/**
 * @brief Typedef to refer to the watchmail linked list.
 */

typedef struct watchmailelement watchmailList;
/** 
 * @brief A typedef is defined for the ::pathelement struct to be consistent
 * with the other linked lists.
 */
typedef struct pathelement pathList;


//------------------------------------------------------------------------------
//-- Environment Types
//------------------------------------------------------------------------------


/** 
 * @brief Global environment structure.
 *
 * The kgenv type will contain our current environment.  If this were being done
 * in C++ it would be a singleton class since we only ever create one variable 
 * of this type.  Basically, we're encapsulating all our would be global 
 * variables into a nice neat structure.
 */
typedef struct {
    int uid;			        ///< User ID 
    char* homedir;		        ///< Home directory path
    struct passwd *pword_entry;		///< Passwd entry info (not needed?)

    char* cwd;	  		        ///< Current working directory
    char* pwd;    		        ///< Prior working directory

    bool  clobber;              ///< Clobber variable for file redirection

    char* prompt; 		        ///< Prompt prefix string
    pathList* path;		        ///< Path list pointer
    histList* hist;		        ///< History list pointer
    aliasList* aliases;		    	///< Alias list pointer
    watchmailList* watchmails;	    	///< Watchmail list pointer
} kgenv;


//------------------------------------------------------------------------------
//-- Function Types
//------------------------------------------------------------------------------

/**
 * @brief This is the generic function type for a built in function.  It's used 
 * to setup the function pointer arrays.
 */
typedef void (*bicfunc)(kgenv*, int, char*);

#endif //_TYPES_INC
