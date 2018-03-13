#include "stack.hpp"

extern "C" {
#include "utilities.h"
#include "constants.h"
}

#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <stack>
#include <map>


// The type of a stack used to hold all the pointers
typedef std::stack<std::string> pointer_stack;

// The type of a message handling function
typedef void (*message_handler)(pointer_stack & stk, std::string & str);


/*********************************************************/
/*                                                       */
/*                    Not in header file				 */
/*                                                       */
/*********************************************************/


// Called when a 'call' was detected
void call_handler(pointer_stack & stk, std::string & ptr) {
	stk.push(ptr);
}

// Called when a 'ret' was detected
void ret_handler(pointer_stack & stk, std::string & ptr) {

	// If the stack is empty or the top of the stack doesn't match ptr, kill all
	if ( stk.empty() || ( stk.top() != ptr ) ) {
		program_err( "Shadow stack mistmach detected!");
	}

	// Otherwise, just pop the stack
	stk.pop();
}

// Called when a fork or thread event occurs
void new_proc_handler(pointer_stack & stk, std::string & ptr) {
	// TODO: write
}

// The shadow stack function
// Communicates with the unix socket server file descriptor sock
void start_shadow_stack( const int sock ) {

	// Create the message handling function map and populate it
	std::map<std::string, message_handler> call_correct_function;
	call_correct_function[NEW_PROC] = new_proc_handler;
	call_correct_function[CALL] = call_handler;
	call_correct_function[RET] = ret_handler;

	// Create the 2 dimensional map of stacks
	// This map exists so different threads / processes don't use the same stack
	pointer_stack stk;
	
	// The buffer used to receive messages
	// The first few bytes will contain the type of message
	// The last few bytes will contain the pointer itself (or 0)
	const int num_bytes = MESSAGE_HEADER_LENGTH + POINTER_SIZE;
	char buffer[ num_bytes ];

	// Loop until the child sends 0 bytes
	while (true) {

		// Read num_bytes bytes, wait until all bytes have been read.
		const int bytes_recv = recv( sock, buffer, num_bytes, MSG_WAITALL );
		assert( (bytes_recv == num_bytes) || (bytes_recv == -1), "recv() failed" );

		// If the client disconnected, break
		if (bytes_recv == -1) {
			break;
		}

		// Read the message
		const std::string message_type(buffer, MESSAGE_HEADER_LENGTH);
		std::string pointer(buffer + MESSAGE_HEADER_LENGTH, POINTER_SIZE);
		assert( call_correct_function.find(message_type) != call_correct_function.end(),
			"Sever recieved wrong type of data!" );

		// Call the appropriate function
		call_correct_function[message_type](stk, pointer);
	}

	// Just in case, kill the grandchildren
	program_err( "Program disconnected" );
}
