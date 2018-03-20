#include "stack_server.hpp"
#include "utilities.hpp"
#include "constants.hpp"
#include "get_tid.hpp"
#include "proc_rc.hpp"
#include "group.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <stack>
#include <map>


// The type of a stack used to hold all the pointers
typedef std::stack<std::string> pointer_stack;

// The type of a message handling function
// It will take in the message send and the socket of the client
// It will return a message to end the program with or nullptr if it should continue
typedef const char * (*message_handler)
	(pointer_stack & stk, std::string & str, const int sock);


/*********************************************************/
/*                                                       */
/*                    Not in header file				 */
/*                                                       */
/*********************************************************/


// Called when a 'call' was detected
const char * call_handler(pointer_stack & stk, std::string & ptr, const int sock) {
	ss_log("TID %d: Push %.*s", gettid(), POINTER_SIZE, ptr.c_str());
	stk.push(ptr);
	return nullptr;
}

// Called when a 'ret' was detected
const char * ret_handler(pointer_stack & stk, std::string & ptr, const int sock) {
	ss_log("TID %d: Pop %.*s", gettid(), POINTER_SIZE, ptr.c_str());

	// If the stack is empty or the top of the stack doesn't match ptr, kill all
	if ( stk.empty() || ( stk.top() != ptr ) ) {
		program_err( "Shadow stack mistmach detected!");
	}

	// Otherwise, just pop the stack
	stk.pop();

	// If the stack is now empty, the program should exit
	if ( stk.empty() ) {
		return "Program ran out of stack frames to pop.";
	}

	// Tell the child proccess it may continue
	ss_assert( write(sock, CONTINUE, sizeof(CONTINUE)) == sizeof(CONTINUE),
		"write() failed." );

	// Do not exit the program
	return nullptr;
}

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

// The shadow stack function
// Communicates with the unix socket server file descriptor sock
void start_shadow_stack( const int sock ) {
	TerminateOnDestruction tod;

	// Create the message handling function map and populate it
	std::map<std::string, message_handler> call_correct_function {
		{ THREAD, thread_handler },
		{ FORK, fork_handler },
		{ CALL, call_handler },
		{ RET, ret_handler }
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
		ss_assert( (bytes_recv == num_bytes) || (bytes_recv == 0), "recv() failed" );

		// If the client disconnected, break
		if (bytes_recv == 0) {
			msg = "Client Disconnected";
			break;
		}

		// Read the message
		const std::string message_type(buffer, MESSAGE_HEADER_LENGTH);
		std::string pointer(buffer + MESSAGE_HEADER_LENGTH, POINTER_SIZE);
		ss_assert( call_correct_function.find(message_type) != call_correct_function.end(),
			"Sever recieved wrong type of data!" );

		// Call the appropriate function and note the message returned
		msg = call_correct_function[message_type](stk, pointer, sock);
	}

	// Print the reason the shadow stack is ending
	ss_log("%s", msg);

	// The process died, decrement the 
	// reference count of processes
	prc.dec();

	// If the program reached this point, another
	// thread / process must be active, gracefully
	// exit the current process without terminating the group
	tod.disable();
	exit(EXIT_SUCCESS);
}
