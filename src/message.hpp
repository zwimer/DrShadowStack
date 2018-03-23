/** @file */
#ifndef __MESSAGE_HPP__
#define  __MESSAGE_HPP__

#include <string.h>

/* TODO: remove  */ void f();
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

/// A class used for defining all message types
/** Messages come in two forms. Ones that are only
 *  headers, and ones that pass a pointer as a body */
class Message final {

	/** A templated message class
	 *  A valid message is defined by constructing a MessageType around it */
	template<bool only_header, typename Info> struct MessageType;

	/** A specification for header only messages */
	template<typename Info>
	struct MessageType<true, Info> final {
		
		/** Disable construction of the class */
		MessageType<true, Info>() = delete;

		/** The size of header only messages */
		static const constexpr int size = MESSAGE_HEADER_LENGTH;

		/** The header of the message */
		static const constexpr char * const header = Info::header;

		/** The message of this type of message is just the header */
		static const constexpr char * const message = header;

		/// Verify the size of the header.
		/** Note C++ allocates an extra character for the null terminator
		 *  this is fine so long as we make sure not to send it */
		static_assert( strlen(Info::header) == size, "Header is of wrong size." );
	};

	/** A specification for non-header only messages */
	template<typename Info>
	struct MessageType<false, Info> {

		/** The size of header only messages */
		static const constexpr int size = MESSAGE_HEADER_LENGTH + POINTER_SIZE;

		/** The header of the message */
		static const constexpr char * const header = Info::header;

		/** Verify the size of the header. */
		static_assert( strlen(header) == MESSAGE_HEADER_LENGTH, 
			"Header is of wrong size." );

		/** Delete the default constructor */
		MessageType<false, Info>() = delete;

		/** The constructor */
		explicit MessageType<false, Info>(const char * const ptr) {
			memcpy(message, header, MESSAGE_HEADER_LENGTH);
			memcpy( & message[MESSAGE_HEADER_LENGTH], ptr, POINTER_SIZE);
		}

		/// The message an instanation of the class holds
		/* This message is NOT null terminated! */
		char message[size];
	};


	/** A class containing the header of Continue message */
	struct ContinueInfo final {

		/** Disable construction */
		ContinueInfo() = delete;

		/** The header of the Continue message */
		static constexpr const char * const header = "CONT";
	};


	/** A class containing the header of Call message */
	struct CallInfo final {
		/** Disable construction */
		CallInfo() = delete;
		/** The header of the Call message */
		static const constexpr char * const header = "CALL";
	};


	/** A class containing the header of Call message */
	struct RetInfo final {
		/** Disable construction */
		RetInfo() = delete;
		/** The header of the Call message */
		static const constexpr char * const header = "RET-";
	};

public:

	/** A function used to check if a char * may represent a message of type T */
	template<class T> static bool is_a_valid(const char * const buffer) {
		return ! memcmp(buffer, T::header, MESSAGE_HEADER_LENGTH);
	}

	/** A typedef for the continue message */
	typedef MessageType<true, ContinueInfo> Continue;

	/** A typedef for the call message */
	typedef MessageType<false, CallInfo> Call;

	/** A typedef for the ret message */
	typedef MessageType<false, RetInfo> Ret;
};


#endif
