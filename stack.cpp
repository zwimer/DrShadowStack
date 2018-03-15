#include "stack.hpp"

extern "C" {
#include "utilities.h"
#include "constants.h"
#include "group.h"
}

#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <stack>
#include <map>

// TODO: delete
#include <iostream>
using namespace std;


// The type of a stack used to hold all the pointers
typedef std::stack<std::string> pointer_stack;

// The type of a message handling function
// It will take in the message send and the socket of the client
// It will return a message to end the program with or NULL if it should continue
typedef const char * (*message_handler)
	(pointer_stack & stk, std::string & str, const int sock);


/*********************************************************/
/*                                                       */
/*                    Not in header file				 */
/*                                                       */
/*********************************************************/


// Called when a 'call' was detected
const char * call_handler(pointer_stack & stk, std::string & ptr, const int sock) {
	std::cout << "Push " << ptr.c_str() << std::endl;
	stk.push(ptr);
	return NULL;
}

// Called when a 'ret' was detected
const char * ret_handler(pointer_stack & stk, std::string & ptr, const int sock) {
	std::cout << "Pop  " << ptr.c_str() << std::endl;

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
	assert( write(sock, CONTINUE, sizeof(CONTINUE)) == sizeof(CONTINUE),
		"write() failed." );

	// Do not exit the program
	return NULL;
}

// Called when a fork or thread event occurs
const char * new_proc_handler(pointer_stack & stk, std::string & ptr, const int sock) {
	return NULL;
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
	const char * msg = NULL;
	while (msg == NULL) {

		// Read num_bytes bytes, wait until all bytes have been read.
		const int bytes_recv = recv( sock, buffer, num_bytes, MSG_WAITALL );
		assert( (bytes_recv == num_bytes) || (bytes_recv == 0), "recv() failed" );

		// If the client disconnected, break
		if (bytes_recv == 0) {
			msg = "Client Disconnected";
			break;
		}

		// Read the message
		const std::string message_type(buffer, MESSAGE_HEADER_LENGTH);
		std::string pointer(buffer + MESSAGE_HEADER_LENGTH, POINTER_SIZE);
		assert( call_correct_function.find(message_type) != call_correct_function.end(),
			"Sever recieved wrong type of data!" );

		// Call the appropriate function and note the message returned
		msg = call_correct_function[message_type](stk, pointer, sock);
	}

	// Print the reason the shadow stack is ending
	printf("%s\n", msg);

	// TODO: do not make children kill parents. Maybe kill all if reference count hits 0?
	// Just in case, kill all the grand children
	terminate_group();
}
