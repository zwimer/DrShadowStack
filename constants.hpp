#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__


/*********************************************************/
/*                                                       */
/*	  Global constants needed across multiple programs	 */
/*                                                       */
/*********************************************************/


// The default log and error files
#define LOG_FILE stdout
#define ERROR_FILE stderr

// The size of a pointer (8 for 64 bit programs, 4 for 32 bit)
#define POINTER_SIZE 8

// The number of characters a message header can be
#define MESSAGE_HEADER_LENGTH 4

// The message 'headers'
#define CALL "CALL"
#define RET "RET-"
#define NEW_PROC "NEWP"

// The continue message
#define CONTINUE "CONT"


/*********************************************************/
/*                                                       */
/*	  				   Due to the OS					 */
/*                                                       */
/*********************************************************/


// TODO: check to see if compatible with mac
#ifdef __APPLE__

	// The max number of signals on linux allowed
	#define _NSIG 65
#endif


#endif
