#include "external_stack_server.hpp"
#include "data_stack.hpp"
#include "constants.hpp"
#include "utilities.hpp"
#include "message.hpp"
#include "group.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <map>

// Remove assert macro
#undef assert

// For brevity
using NewSignal = Message::NewSignal;
using Continue = Message::Continue;
using Thread = Message::Thread;
using Fork = Message::Fork;
using Call = Message::Call;
using Ret = Message::Ret;


// The type of a stack used to hold all the pointers
typedef DataStack::Stack<const char *> pointer_stack;

// The type of a message handling function
// It will take in the message send and the socket of the client
// It will return a message to end the program with or nullptr if it should continue
typedef void (*message_handler) (pointer_stack & stk, 
	const char * const buffer, const int sock);


/*********************************************************/
/*                                                       */
/*                    Not in header file				 */
/*                                                       */
/*********************************************************/


// Called whenever a signal is sent to the client
// Signal handlers have no 'call', so we add a wildcard
void add_wildcard(pointer_stack & stk, const char * const, const int) {
	stk.push( (char *) WILDCARD );	
}

// Called when a 'call' was detected
void call_handler(pointer_stack & stk, const char * const buffer, const int) {
	const char * const addr = * ((char **) buffer);
	Utilities::verbose_log("(server) Push(", (void *) addr, ")");
	stk.push(addr);
}

// Called when a 'ret' was detected
void ret_handler(pointer_stack & stk, const char * const buffer, const int sock) {

	// Log the address
	const char * const addr = * ((char **) buffer);
	Utilities::verbose_log("(server) Pop(", (void *) addr,")\n");

	// If the stack is empty, error
	if ( stk.empty() ) {
		Utilities::log_error( 	"*** Shadow stack mistmach detected! ***\n"
								"Attempting to return to ", (void *) addr, 
								"\n\tShadow Stack is empty.\n" );
		Group::terminate(nullptr);
	}
	
	// If the top of the stack is a wildcard, 
	// we are returning from a signal handler
	const char * const top = stk.top();
	if ( top == (char *) WILDCARD ) {
		Utilities::verbose_log("Wildcard detected, returning from signal handler allowed.");
	}

	// If the return address is incorrect, error
	else if ( addr != top ) {
		Utilities::log_error(	"*** Shadow stack mistmach detected! ***\n"
								"Attempting to return to ", (void *) addr,
								"\n\tTop of shadow stack is ", (void *) top, "\n" );
		Group::terminate(nullptr);
	}

	// If everything is valid, pop the stack
	stk.pop();

	// Tell the client proccess it may continue
	const int bytes_sent = write(sock, Continue::message, Continue::size);
	Utilities::assert( bytes_sent == Continue::size, "write() failed." );
}

// Called when the process forks
void fork_handler(pointer_stack & stk, const char * const buffer, const int) {
	// TODO
	/* new_proc_handler(); */
	Utilities::log_error("fork_handler");
	Utilities::log_error("FORK_HANDLER");
}

// Called when the process starts a new thread
void thread_handler(pointer_stack & stk, const char * const buffer, const int) {
	// TODO
	/* while ( ! stk.empty() ) { */
	/* 	stk.pop(); */
	/* } */
	/* new_proc_handler(); */
	Utilities::log_error("THREAD_HANDLER");
}


// The external shadow stack function
// Communicates with the unix socket server file descriptor sock
void start_external_shadow_stack( const int sock ) {
	TerminateOnDestruction tod;

	// Create the message handling function map and populate it
	std::map<std::string, message_handler> call_correct_function {
		{ std::string( NewSignal::header ), add_wildcard },
		{ std::string( Thread::header ), thread_handler },
		{ std::string( Fork::header ), fork_handler },
		{ std::string( Call::header ), call_handler },
		{ std::string( Ret::header ), ret_handler }
	};

	// Create the shadow stack
	pointer_stack stk;
	
	// The buffer used to receive messages
	// The first few bytes will contain the type of message
	// The last few bytes will contain the pointer itself (or 0)
	const int num_bytes = MESSAGE_HEADER_LENGTH + POINTER_SIZE;
	char buffer[ num_bytes ];

	// Loop until the child sends 0 bytes
	const char * msg = nullptr;
	while (msg == nullptr) {

		// Read num_bytes bytes, wait until all bytes have been read.
		const int bytes_recv = recv( sock, buffer, num_bytes, MSG_WAITALL );
		Utilities::assert( (bytes_recv == num_bytes) || (bytes_recv == 0), "recv() failed" );

		// If the client disconnected, break
		if (bytes_recv == 0) {
			Utilities::log("Client Disconnected.");
			break;
		}

		// Verify the message is valid then call the appropriate function
		const std::string message_type(buffer, MESSAGE_HEADER_LENGTH);
		const auto function_ptr = call_correct_function[message_type];
		Utilities::verbose_log("Got message header: ", std::string(buffer, MESSAGE_HEADER_LENGTH));
		Utilities::assert( function_ptr != nullptr, "Sever recieved wrong type of message!" );
		function_ptr(stk, & buffer[MESSAGE_HEADER_LENGTH], sock);
	}

	// If the program reached this point, another
	// thread / process must be active, gracefully return
	tod.disable();
}
