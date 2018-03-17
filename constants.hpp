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

// The size of a pointer in the target program
// To run on 32 bit programs, change this value!
#define POINTER_SIZE sizeof(void *)

// The number of characters a message header can be
#define MESSAGE_HEADER_LENGTH 4

// The message 'headers'
#define CONTINUE "CONT"
#define THREAD "PTHD"
#define FORK "FORK"
#define CALL "CALL"
#define RET "RET-"


/*********************************************************/
/*                                                       */
/*	  				   Due to the OS					 */
/*                                                       */
/*********************************************************/


// The max number of signals on linux allowed
#ifdef __APPLE__
	#define _NSIG 65
#endif


#endif
