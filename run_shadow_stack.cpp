#include "run_shadow_stack.hpp"
#include "stack.hpp"

extern "C" {
#include "quick_socket.h"
#include "utilities.h"
#include "group.h"
}

#include <unistd.h>
#include <signal.h>


// TODO: delete
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
extern "C" {
	#include "constants.h"
}

void snd(std::string s, int set_s = -1) {
	static int sock = -1; if ( set_s != -1 ) { sock = set_s; return; }
	if ( s.size() != 12 ) {
		std::cout << "Got s = " << s.size() << " bytes" << std::endl;
		std::cout << "s = " << s << std::endl;
		assert(false, "wrong size");
	}
	assert( write(sock, s.c_str(), s.size()), "write() failed!");
}

void snd2(std::string pre, std::string s) {
	while (s.size() != 8) {
		s = std::string("0", 1) + s;
	}
	snd(pre + s);
}

void call(std::string s) {
	snd2("CALL", s);
}

void ret(std::string s) {
	snd2("RET-", s);
}

// Start's the program passed in via drrun
void start_program(char ** argv, int sock) {
	// TODO: write
	snd("", sock);

	for(int i = 0; i < 5; ++i) {
		call( std::string(1, std::string("123456789")[i]) );
	}

	const int num_bytes = sizeof(CONTINUE);
	char buffer[num_bytes];
	for(int i = 4; i > 0; --i) {
		ret( std::string(1, std::string("123456789")[i]) );
		const int bytes_recv = recv( sock, buffer, num_bytes, MSG_WAITALL );
		assert( bytes_recv == num_bytes, "recv() failed" );
		assert( memcmp(buffer, CONTINUE, num_bytes) == 0, "CONT is wrong!");
		std::cout << "CONT" << std::endl;
		sleep(1);
	}

	ret("Fail");

	/* while(1) { */
	/* 	std::cout << "Waiting..." << std::endl; */
	/* 	std::string line; */
	/* 	std::getline(std::cin, line); */
	/* 	std::cout << "SENDING: " << line << "\n\tBytes = " << line.size() << std::endl; */
	/* } */
	fprintf(stderr, "Process Send death\n");
}

// Main function
int main(int argc, char * argv[]) {

	// TODO: remove
setvbuf(stdout, NULL, _IONBF, 0);


	// TODO: run everything in 1, try catch block and 2, with exception handlers to kill all if error

	// Create a new process group by starting a new session
	// Many terminals will automatically do this, but just in case...
	// Also changes many default signal handlers to kill the process group
	setup_group();

	// We check for the return statuses of functions, so ignore sigpipe
	signal(SIGPIPE, SIG_IGN);

	// Setup a unix server
	char * const server_name = gen_new_filename( 50 );
	const int sock = create_server(server_name);
	std::cout << server_name << std::endl;

	// Create a client then accept the client
	const int client_sock = create_client(server_name);
	const int server_sock = accept_client(sock);

	// Fork
	// Note that the server belongs to the parent, the client is the child
	const pid_t pid = fork();
	assert( pid != -1, "fork() failed" );

	// TODO: delete
	std::cout << getpid() << std::endl;

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
