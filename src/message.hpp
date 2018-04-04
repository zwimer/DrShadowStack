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
#define POINTER_SIZE ( sizeof(void *) )

/** The number of characters a message header can be */
#define MESSAGE_HEADER_LENGTH 4

/** The size of a message */
#define MESSAGE_SIZE (POINTER_SIZE + MESSAGE_HEADER_LENGTH)


/*********************************************************/
/*                                                       */
/*					  Helper functions					 */
/*                                                       */
/*********************************************************/


/** Copies n bytes from src into dst during static initil
 *  If src is less than n bytes, fills the rest of dst with zeros.
 *  It will return dst so that dst can be assigned to a static initalization */
const char * set_length( char * const dst, const char * const src, const int n) {
	return strncpy((char *) memset((void *)dst, 0, n), src, n);
}


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


	// Note: **ALL** messages will be of the same size.
	// Header only messages - the contents of the body do not matter.


	/** A specification for header only messages */
	template<typename Info>
	struct MessageType<true, Info> final {
		
		/** Disable construction of the class */
		MessageType<true, Info>() = delete;

		/** The size of the message */
		static const constexpr int size = MESSAGE_SIZE;

		/** The header of the message */
		static const constexpr char * const header = Info::header;

		/// Verify the size of the header.
		/** Note C++ allocates an extra character for the null terminator
		 *  this is fine so long as we make sure not to send it */
		static_assert( strlen(Info::header) == MESSAGE_HEADER_LENGTH,
			"Header is of wrong size." );

		/** The message of this type of message is just the header
		 *  This is **NOT** null terminated */
		static const char * const message;

	private:

		/** An internal buffer pointed to by message */
		static char internal[size];
	};

	/** A specification for non-header only messages */
	template<typename Info>
	struct MessageType<false, Info> {

		/** The size of the message */
		static const constexpr int size = MESSAGE_SIZE;

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

	// General Headers

	/** A class containing the header of NewSignal message */
	struct NewSignalInfo final {
		/** The header of the NewSignal message */
		static constexpr const char * const header = "NEWS";
	};

	/** A class containing the header of Continue message */
	struct ContinueInfo final {
		/** The header of the Continue message */
		static constexpr const char * const header = "CONT";
	};

	/** A class containing the header of Call message */
	struct CallInfo final {
		/** The header of the Call message */
		static const constexpr char * const header = "CALL";
	};

	/** A class containing the header of Call message */
	struct RetInfo final {
		/** The header of the Ret message */
		static const constexpr char * const header = "RET-";
	};

	/** A class containing the header of Fork message */
	struct ForkInfo final {
		/** The header of the Fork message */
		static const constexpr char * const header = "FORK";
	};

	/** A class containing the header of Thread message */
	struct ThreadInfo final {
		/** The header of the Thread message */
		static const constexpr char * const header = "THRD";
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

	/** A typedef for the new signal message */
	typedef MessageType<true, NewSignalInfo> NewSignal;
	/** A typedef for the fork message */
	typedef MessageType<true, ForkInfo> Fork;
	/** A typedef for the thread message */
	typedef MessageType<true, ThreadInfo> Thread;
};


// Initalize MessageType<true, Info>::internal
template<typename T> char Message::MessageType<true, T>::internal[size] = {};

// Initalize MessageType<true, Info>::message
template<typename T> const char * const Message::MessageType<true, T>::message
	= set_length(	Message::MessageType<true, T>::internal, 
					Message::MessageType<true, T>::header, 
					Message::MessageType<true, T>::size );

#endif
