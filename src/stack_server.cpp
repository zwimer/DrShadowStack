#include "stack_server.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <stack>
#include <map>

#include "utilities.hpp"
#include "message.hpp"
#include "get_tid.hpp"
#include "proc_rc.hpp"
#include "group.hpp"

// Remove assert macro
#undef assert

// For brevity
using Continue = Message::Continue;
using Call = Message::Call;
using Ret = Message::Ret;


// The type of a stack used to hold all the pointers
typedef std::stack<const char *> pointer_stack;

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


// Should never be called, will crash the program
void continue_handler(pointer_stack &, const char * const, const int) {
	Utilities::err("Continue should not be recieved by the server");
}

// Called when a 'call' was detected
void call_handler(pointer_stack & stk, const char * const buffer, const int) {
	const char * const addr = * ((char **) buffer);
	Utilities::log("TID %d: (server) Push(%p)", get_tid(), addr);
	stk.push(addr);
}

// Called when a 'ret' was detected
void ret_handler(pointer_stack & stk, const char * const buffer, const int sock) {

	// Log the address
	const char * const addr = * ((char **) buffer);
	Utilities::log("TID %d: (server) Pop(%p)\n", get_tid(), addr);

	// If the stack is empty, error
	if ( stk.empty() ) {
		Utilities::log_error( 	"*** Shadow stack mistmach detected! ***\n"
								"Attempting to return to %p\n"
								"\tShadow Stack is empty.\n", addr );
		Group::terminate(nullptr);
	}
	
	// If the return address is incorrect, error
	if ( addr != stk.top() ) {
		Utilities::log_error(	"*** Shadow stack mistmach detected! ***\n"
								"Attempting to return to %p\n"
								"\tTop of shadow stack is %p\n",
								addr, stk.top() );
		Group::terminate(nullptr);
	}

	// If everything is valid, pop the stack
	stk.pop();

	// Tell the client proccess it may continue
	const int bytes_sent = write(sock, Continue::message, Continue::size);
	Utilities::assert( bytes_sent == Continue::size, "write() failed." );
}

// The shadow stack function
// Communicates with the unix socket server file descriptor sock
void start_shadow_stack( const int sock ) {
	TerminateOnDestruction tod;

	// Declare this as a valid process
	prc->inc();

	// Create the message handling function map and populate it
	std::map<std::string, message_handler> call_correct_function {
		{ std::string( Continue::header ), continue_handler },
		{ std::string( Call::header ), call_handler },
		{ std::string( Ret::header ), ret_handler }
	};

	// Create the 2 dimensional map of stacks
	// This map exists so different threads / processes don't use the same stack
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
			Utilities::message("Client Disconnected.");
			break;
		}

		// Verify the message is valid then call the appropriate function
		const std::string message_type(buffer, MESSAGE_HEADER_LENGTH);
		const auto function_ptr = call_correct_function[message_type];
		Utilities::assert( function_ptr != nullptr, "Sever recieved wrong type of data!" );
		function_ptr(stk, & buffer[MESSAGE_HEADER_LENGTH], sock);
	}

	// The process died (we think, but if not this is safe still)
	// decrement the reference count of processes
	prc->dec();

	// If the program reached this point, another
	// thread / process must be active, gracefully return
	tod.disable();
}
