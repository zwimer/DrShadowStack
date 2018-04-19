/** @file */
#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

#include "utilities.hpp"

#include <sys/socket.h>
#include <unistd.h>
#include <string.h>


/*********************************************************/
/*                                                       */
/*                        Constant                       */
/*                                                       */
/*********************************************************/


/** The size of a pointer in the target program
 *  To run on 32 bit programs, change this value! */
#define POINTER_SIZE ( sizeof( void * ) )

/** The number of characters a message header can be */
#define MESSAGE_HEADER_LENGTH 4

/** The size of a message */
#define MESSAGE_SIZE ( POINTER_SIZE + MESSAGE_HEADER_LENGTH )


/*********************************************************/
/*                                                       */
/*                     Message classes                   */
/*                                                       */
/*********************************************************/


/** A class used for defining all message types
 *  Messages come in two forms. Ones that are only
 *  headers, and ones that pass a pointer as a body */
class Message final {

	/** Defines the types of messages which can be sent */
	class Msg final {

/** The internals of a message. We use a macro to enforce consistency */
/* clang-format off */
#define MESSAGE_INTERNALS( HEADER_ONLY )                                                      \
	/** Declares if the message is header only */                                             \
	static const constexpr bool header_only = HEADER_ONLY;                                    \
	/** Define the size of the message */                                                     \
	static const constexpr int size = MESSAGE_SIZE;                                           \
	/** The header of the message */                                                          \
	static const constexpr char *const header = Info::header;                                 \
	/** Verify the size of the header.                                                        \
	 *  Note C++ allocates an extra character                                                 \
	 *  for the null terminator this is fine so                                               \
	 *  long as we make sure not to send it */                                                \
	static_assert( strlen( Info::header ) == MESSAGE_HEADER_LENGTH,                           \
				   "Header is of wrong size." );                                              \
	/** Delete the default constructor */                                                     \
	MessageType<HEADER_ONLY, Info>() = delete;
		/* clang-format on */

		/** Copies n bytes from src into dst during static initilization
		 *  If src is less than n bytes, fills the rest of dst with zeros.
		 *  It will return dst so that dst can be assigned to a static initalization */
		static const char *set_length( char *const dst, const char *const src,
		                               const int n );

		/** A templated message class
		 *  A valid message is defined by constructing a MessageType around it */
		template <bool only_header, typename Info> struct MessageType;

		// Note: **ALL** messages will be of the same size.
		// Header only messages - the contents of the body do not matter.

		/** A specification for header only messages */
		template <typename Info> struct MessageType<true, Info> final {

			// Setup the internals of the message
			MESSAGE_INTERNALS( true )

			/** The message of this type of message is just the header
			 *  This is **NOT** null terminated */
			static const char *const message;

		  private:
			/** An internal buffer pointed to by message */
			static char internal[size];
		};

		/** A specification for non-header only messages */
		template <typename Info> struct MessageType<false, Info> final {

			// Setup the internals of the message
			MESSAGE_INTERNALS( false )

			/** The constructor */
			explicit MessageType<false, Info>( const char *const ptr ) {
				memcpy( message, header, MESSAGE_HEADER_LENGTH );
				memcpy( &message[MESSAGE_HEADER_LENGTH], ptr, POINTER_SIZE );
			}

			/** The message an instanation of the class holds
			 * This message is NOT null terminated! */
			char message[size];
		};

// Remove macros
#undef MESSAGE_INTERNALS

	  public:
		/** A typedef for header only type */
		template <typename T> using HeaderOnly = MessageType<true, T>;

		/** A typedef for non-header only type */
		template <typename T> using WithBody = MessageType<false, T>;
	};


	// General Headers

	/** A class containing the header of NewSignal message */
	struct NewSignalInfo final {
		/** The header of the NewSignal message */
		static constexpr const char *const header = "NEWS";
	};

	/** A class containing the header of Continue message */
	struct ContinueInfo final {
		/** The header of the Continue message */
		static constexpr const char *const header = "CONT";
	};

	/** A class containing the header of Call message */
	struct CallInfo final {
		/** The header of the Call message */
		static const constexpr char *const header = "CALL";
	};

	/** A class containing the header of Call message */
	struct RetInfo final {
		/** The header of the Ret message */
		static const constexpr char *const header = "RET-";
	};

	/** A class containing the header of Fork message */
	struct ForkInfo final {
		/** The header of the Fork message */
		static const constexpr char *const header = "FORK";
	};

	/** A class containing the header of Thread message */
	struct ThreadInfo final {
		/** The header of the Thread message */
		static const constexpr char *const header = "THRD";
	};

  public:
	/** A function used to check if a char * may represent a message of type T */
	template <class T> static bool is_a_valid( const char *const buffer ) {
		return !memcmp( buffer, T::header, MESSAGE_HEADER_LENGTH );
	}

	/** A typedef for the continue message */
	typedef Msg::HeaderOnly<ContinueInfo> Continue;
	/** A typedef for the call message */
	typedef Msg::WithBody<CallInfo> Call;
	/** A typedef for the ret message */
	typedef Msg::WithBody<RetInfo> Ret;

	/** A typedef for the new signal message */
	typedef Msg::HeaderOnly<NewSignalInfo> NewSignal;
	/** A typedef for the fork message */
	typedef Msg::HeaderOnly<ForkInfo> Fork;
	/** A typedef for the thread message */
	typedef Msg::HeaderOnly<ThreadInfo> Thread;
};


// Initalize MessageType<true, Info>::internal
template <typename T> char Message::Msg::MessageType<true, T>::internal[size] = {};

// Initalize MessageType<true, Info>::message
template <typename T>
const char *const Message::Msg::MessageType<true, T>::message =
    set_length( Message::Msg::MessageType<true, T>::internal,
                Message::Msg::MessageType<true, T>::header,
                Message::Msg::MessageType<true, T>::size );


/*********************************************************/
/*                                                       */
/*            Used to send / recieve messages            */
/*                                                       */
/*********************************************************/


/** Sends a header only Msg to sock */
template <typename Msg> void send_msg( const int sock ) {
	static_assert( Msg::header_only == true, "wrong send_msg called." );
	const int bytes_sent = write( sock, Msg::header, Msg::size );
	Utilities::assert( bytes_sent == Msg::size, "write() failed!" );
}

/** Sends a non-header only Msg with body bdy to sock */
template <typename Msg> void send_msg( const int sock, const char *const bdy ) {
	static_assert( Msg::header_only == false, "wrong send_msg called." );
	Msg to_send( bdy );
	const int bytes_sent = write( sock, to_send.message, to_send.size );
	Utilities::assert( bytes_sent == to_send.size, "write() failed!" );
}

/** Reads a header only Msg from sock */
template <typename Msg> void recv_msg( const int sock ) {
	static_assert( Msg::header_only == true, "wrong recv_msg called." );
	char buffer[Msg::size];
	const int bytes_recv = recv( sock, buffer, Msg::size, MSG_WAITALL );
	Utilities::assert( bytes_recv == Msg::size, "Did not get full size message!" );
	const constexpr auto is_msg = Message::is_a_valid<Message::Continue>;
	Utilities::assert( is_msg( buffer ), "Received incorrect message!" );
}

/** Reads a only Msg from sock
 *  This function is **NOT** reentrant */
template <typename Msg> const char *recv_msg_and_body( const int sock ) {
	static_assert( Msg::header_only == false, "wrong recv_msg called." );
	static char buffer[Msg::size];
	const int bytes_recv = recv( sock, buffer, Msg::size, MSG_WAITALL );
	Utilities::assert( bytes_recv == Msg::size, "Did not get full size message!" );
	const constexpr auto is_msg = Message::is_a_valid<Message::Continue>;
	Utilities::assert( is_msg( buffer ), "Received incorrect message!" );
	return buffer;
}


#endif
