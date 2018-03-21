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
using namespace Message;


// The type of a stack used to hold all the pointers
typedef std::stack<std::string> pointer_stack;

// The type of a message handling function
// It will take in the message send and the socket of the client
// It will return a message to end the program with or nullptr if it should continue
typedef void (*message_handler) (pointer_stack & stk, std::string & str, const int sock);


/*********************************************************/
/*                                                       */
/*                    Not in header file				 */
/*                                                       */
/*********************************************************/


// Should never be called, will crash the program
void continue_handler(pointer_stack &, std::string &, const int) {
	Utilities::err("Continue should not be recieved by the server");
}

// Called when a 'call' was detected
void call_handler(pointer_stack & stk, std::string & ptr, const int) {
	Utilities::log("TID %d: Push %.*s", get_tid(), POINTER_SIZE, ptr.c_str());
	stk.push(ptr);
}

// Called when a 'ret' was detected
void ret_handler(pointer_stack & stk, std::string & ptr, const int sock) {
	Utilities::log("TID %d: Pop %.*s", get_tid(), POINTER_SIZE, ptr.c_str());

	// If the stack is empty or the top of the stack doesn't match ptr, kill all
Utilities::log("Server %d", __LINE__);
	Utilities::assert( ( ! stk.empty() ) && ( stk.top() == ptr ), 
						"Shadow stack mistmach detected!");

Utilities::log("Server %d", __LINE__);
	// Otherwise, just pop the stack
	stk.pop();
Utilities::log("Server %d", __LINE__);

	// Tell the child proccess it may continue
	static const Continue cont;
Utilities::log("Server %d", __LINE__);
	const int bytes_sent = write(sock, cont.message, cont.size);
Utilities::log("Server %d", __LINE__);
	Utilities::assert( bytes_sent == cont.size, "write() failed." );
Utilities::log("Server %d", __LINE__);
}

#if 0
// Called when a fork event occurs
const char * fork_handler(pointer_stack & stk, std::string & ptr, const int sock) {
	return nullptr;
	// TODO: write
}


// Called when a thread event occurs
const char * thread_handler(pointer_stack & stk, std::string & ptr, const int sock) {
	return nullptr;
	// TODO: write
}
#endif

// The shadow stack function
// Communicates with the unix socket server file descriptor sock
void start_shadow_stack( const int sock ) {
	TerminateOnDestruction tod;

	// Declare this as a valid process
	prc.inc();

	// Create the message handling function map and populate it
	std::map<std::string, message_handler> call_correct_function {
		{ Continue::header, continue_handler },
		{ Call::header, call_handler },
		{ Ret::header, ret_handler }
		/* { THREAD, thread_handler }, */
		/* { FORK, fork_handler }, */
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
			msg = "Client Disconnected.";
			break;
		}

		// Read the message
		const std::string message_type(buffer, MESSAGE_HEADER_LENGTH);
		std::string pointer(buffer + MESSAGE_HEADER_LENGTH, POINTER_SIZE);
		Utilities::assert( call_correct_function.find(message_type) != call_correct_function.end(),
			"Sever recieved wrong type of data!" );

		// Call the appropriate function
		call_correct_function[message_type](stk, pointer, sock);
	}

	// Print the reason the shadow stack is ending
	// This prints to the error stream, but is not for sure an error
	Utilities::log_error("TID %d: %s", get_tid(), msg);

	// The process died, decrement the 
	// reference count of processes
	prc.dec();

	// If the program reached this point, another
	// thread / process must be active, gracefully return
	tod.disable();
}
