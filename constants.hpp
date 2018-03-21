/** @file */
#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__


/*********************************************************/
/*                                                       */
/*					Libraries for drrun					 */ 
/*                                                       */
/*********************************************************/


// The .so of the shadow stack DynamoRIO client.
// This should be defined by the cmake on compilation
/* #define DYNAMORIO_CLIENT_SO "/path/to/.so/" */


/*********************************************************/
/*                                                       */
/*						 General						 */ 
/*                                                       */
/*********************************************************/


// The name of the program
// This should be defined by the cmake on compilation
/* #define PROGRAM_NAME "DrShadowStack" */

// The default log file. 
// This fill may be nullptr, but may not be NULL 
// This should be defined by the cmake on compilation
/* #define LOG_FILE stdout */

/** The default error log file (important events are also noted here)
 *  This fill may be nullptr, but may not be NULL */
#define ERROR_FILE stderr


/// The size of a pointer in the target program
/** To run on 32 bit programs, change this value! */
#define POINTER_SIZE sizeof(void *)


/*********************************************************/
/*                                                       */
/*				Message header information				 */
/*                                                       */
/*********************************************************/


/** The number of characters a message header can be */
#define MESSAGE_HEADER_LENGTH 4

// TODO: change descriptions after implementation

/// The 'continue' message 'header'
/** This message has no body and contains only the header.
 *  This message is sent by the shadow stack when a ret is detected
 *  if no address mis-match is detected. It allows the client program
 *  to continue operation */
#define CONTINUE "CONT"

/// The 'threading event' message header
/** This message has no body and contains only the header.
 *  This message is sent by the client program to the shadow
 *  stack when a threading event occurs */
#define THREAD "THDE"

/// The 'fork event' message header
/** This message has no body and contains only the header.
 *  This message is sent by the client program to the shadow
 *  stack when a fork event occurs */
#define FORK "FORK"

/// The 'call instruction about to run' message header
/** This message has a body of size POINTER_SIZE
 *  This message is sent from the client to the shadow stack
 *  This content of the body is current value of the PC */
#define CALL "CALL"

/// The 'ret instruction about to run' message header
/** This message has a body of size POINTER_SIZE
 *  This message is sent from the client to the shadow stack
 *  This content of the body is the value stored at the top
 *  of the client's stack; the address ret will be returning to */
#define RET "RET-"


/*********************************************************/
/*                                                       */
/*	  				   Due to the OS					 */
/*                                                       */
/*********************************************************/


/** If this is on a mac */
#ifdef __APPLE__

/** The max number of signals on linux allowed */
#define _NSIG 65
#endif


#endif
