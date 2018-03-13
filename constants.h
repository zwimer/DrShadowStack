#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__


/*********************************************************/
/*                                                       */
/*	  Global constants needed across multiple programs	 */
/*                                                       */
/*********************************************************/


// The size of a pointer (8 for 64 bit programs, 4 for 32 bit)
#define POINTER_SIZE 8

// The number of characters a message header can be
#define MESSAGE_HEADER_LENGTH 4

// The message 'headers'
#define CALL "CALL"
#define RET "RET-"
#define NEW_PROC "NEWP"


#endif
