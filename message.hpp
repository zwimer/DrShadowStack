/** @file */
#ifndef __MESSAGE_HPP__
#define  __MESSAGE_HPP__

#include <string.h>


/*********************************************************/
/*                                                       */
/*						 Constants						 */ 
/*                                                       */
/*********************************************************/


/// The size of a pointer in the target program
/** To run on 32 bit programs, change this value! */
#define POINTER_SIZE 8

/** The number of characters a message header can be */
#define MESSAGE_HEADER_LENGTH 4


/*********************************************************/
/*                                                       */
/*					  Message classes					 */ 
/*                                                       */
/*********************************************************/


/** A namespace for messages */
namespace Message {


	/** A function used to check if a char * may represent a message of type T */
	template<class T> bool is_a_valid(const char * const buffer) {
		return ! memcmp(buffer, T::header, MESSAGE_HEADER_LENGTH);
	}


	// Note: Even though not all functions are needed, each
	// message class implements the same functions for consistency


	/// The 'continue' message
	/** This message has no body and contains only the header.
	 *  This message is sent by the shadow stack when a ret is detected
	 *  if no address mis-match is detected. It allows the client program
	 *  to continue operation */
	struct Continue final {

		// Statics

		/** The size of the message */
		static constexpr int size = MESSAGE_HEADER_LENGTH;

		/** The message header */
		static const char header[];

		// Non-statics
		
		/** The constructor */
		Continue();

		/** The buffer that holds the message */
		char message[size];
	};


	/// The 'call instruction about to run' message
	/** This message has a body of size POINTER_SIZE
	 *  This message is sent from the client to the shadow stack
	 *  This content of the body is current value of the PC */
	struct Call final {

		// Statics

		/** The size of the message */
		static constexpr int size = MESSAGE_HEADER_LENGTH + POINTER_SIZE;

		/** The message header */
		static const char header[];

		// Non-statics
		
		/** The constructor */
		Call(const char * const ptr);

		/** The buffer that holds the message */	
		char message[size];
	};


	/// The 'ret instruction about to run' message
	/** This message has a body of size POINTER_SIZE
	 *  This message is sent from the client to the shadow stack
	 *  This content of the body is the value stored at the top
	 *  of the client's stack; the address ret will be returning to */
	struct Ret final {

		// Statics

		/** The size of the message */
		static constexpr int size = MESSAGE_HEADER_LENGTH + POINTER_SIZE;

		/** The message header */
		static const char header[];

		// Non-statics
		
		/** The constructor */
		Ret(const char * const ptr);

		/** The buffer that holds the message */
		char message[size];
	};


#if 0
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
#endif

}

#endif
