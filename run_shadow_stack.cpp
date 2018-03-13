#include "run_shadow_stack.hpp"
#include "stack.hpp"

extern "C" {
#include "quick_socket.h"
#include "utilities.h"
#include "group.h"
}

#include <unistd.h>


// Start's the program passed in via drrun
void start_program(char ** a, int b) {
	sleep(10);
	// TODO: WRITE
}

// Main function
int main(int argc, char * argv[]) {

	// Create a new process group by starting a new session
	setup();

	// Setup a unix server
	char * const server_name = gen_new_filename( 50 );
	const int sock = create_server(server_name);

	// Create a client then accept the client
	const int client_sock = create_client(server_name);
	const int server_sock = accept_client(sock);

	// Fork
	// Note that the server belongs to the parent, the client is the child
	const pid_t pid = fork();
	assert( pid != -1, "fork() failed" );

	// If this is the child process,
	// start the program to be protected
	if (pid == 0) {
		assert( close(server_sock) != -1 , "close() failed" );
		start_program(argv, client_sock);
	}

	// Otherwise, this is the parent process,
	// start the shadow stack
	else {
		assert( close(client_sock) != -1 , "close() failed" );
		start_shadow_stack(server_sock);
	}
}
