#include "shadow_stack.hpp"
#include "external_stack_server.hpp"
#include "quick_socket.hpp"
#include "parse_args.hpp"
#include "constants.hpp"
#include "utilities.hpp"
#include "group.hpp"

#include <boost/filesystem.hpp> // TODO
#include <unistd.h>
#include <signal.h>
#include <sstream>
#include <random>
#include <vector>


// Undefine macro assert, it clobbers the class member function assert
#undef assert


// Return a non-existent filename
std::string temp_name() {

boost::filesystem::unique_path();
	// The desired size file name
	static const int size = 23;

	// Check lenth of string. Note, bind will not accept 
	// file names longer than something like 108 characters
	static_assert( size < 100, "file name too long!" );

	// The desired location
	static char where[] = "/tmp/";
	static const int len_where = strlen(where);

	// Characters which may be used in the file name
	static char lib[] =	"abcdefghijklmnopqrstuvwxyz"
						"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
						"1234567890";
	static const int lib_size = strlen(lib);

	// Until an un-seen filename is generated, loop
	char fname[size + 1];
	std::random_device rd;
	while (true) {

		// Generate a randome filename
		strncpy(fname, where, len_where);
		for ( int i = len_where; i < size; ++i ) {
			fname[i] = lib[rd() % lib_size];
		}
		fname[size] = '\0';

		// We temprarily borrow errno below		
		int old_errno = errno;
		errno = 0;

		// If the file does not exists, restore errno and return it
		if ( access( fname, F_OK ) == -1 ) {
			Utilities::assert( errno == ENOENT, "access failed()." );
			errno = old_errno;
			return fname;
		}
	}
}

// Start's the program passed in via drrun
void start_program( const Args input_args, char * socket_path ) {

	// Construct args to give to exec
	std::vector<const char *> exec_args;
	
	// Drrun a client
	exec_args.push_back(input_args.drrun.c_str());
	exec_args.push_back("-c");

	// ShadowStack dynamorio client + args
	exec_args.push_back(DYNAMORIO_CLIENT_SO);
	exec_args.push_back(input_args.is_internal ? INTERNAL_MODE_FLAG : EXTERNAL_MODE_FLAG);
	exec_args.push_back(socket_path);

	// Specify target a.out
	exec_args.push_back("--");
	exec_args.push_back(input_args.target.c_str());

	// Add a.out's args
	for(int i = 0; i < input_args.target_args.size(); ++i ) {
		exec_args.push_back(input_args.target_args[i].c_str());
	}

	// Null terminate the array
	exec_args.push_back(nullptr);

	// Log the action then flush the buffers
	std::stringstream pnt;
	pnt << "Starting dr_run\nCalling execvp on: ";
	for ( unsigned long i = 0; i < exec_args.size() - 1; ++i ) {
		pnt << exec_args[i] << ' ';
	}
	Utilities::log(pnt.str().c_str());
	Utilities::message(pnt.str().c_str());
	fflush(NULL);

	// Start drrun
	execvp(input_args.drrun.c_str(), (char **) exec_args.data());
	Utilities::err("execvp() failed.");
}


// Main function
int main(int argc, char * argv[]) {

	// Handle arguments
	const Args args = parse_args(argc, argv);

	// Create a new process group by starting a new session
	// Many terminals will automatically do this, but just in case...
	// Also changes many default signal handlers to kill the process group
	Group::setup();

	// We check for the return statuses of functions, so ignore sigpipe
	Utilities::assert( signal(SIGCHLD, SIG_IGN) != SIG_ERR, "signal() failed." );
	Utilities::assert( signal(SIGPIPE, SIG_IGN) != SIG_ERR, "signal() failed." );
	Utilities::log("Sigpipe and Sigchild ignored");

	// Setup complete, allow forking / threading beyond this point
	Utilities::log("DrShadowStack initalized");
	Utilities::enable_multi_thread_or_process_mode();

	// If the shadow stack should be internal, start it
	if (args.is_internal) {
		char buf[10];
		start_program( args, buf );
	}

	// Setup a unix server
	// Technically, between generating the name name and the
	// server starting, the file could have been created.
	// However, this is safe as the program will crash if so
	const std::string server_name = temp_name();
	const int sock = QS::create_server(server_name.c_str());

	// Just in case an exception occurs, setup a class
	// whose desctructor will terminate the group
	TerminateOnDestruction tod;

	// Fork
	// Note that the server belongs to the parent, the client is the child
	Utilities::log("Starting initial fork...");
	const pid_t pid = fork();
	Utilities::assert( pid != -1, "fork() failed" );
	
	// If this is the child process,
	// start the program to be protected
	if (pid == 0) {
		Utilities::log("Forming drrun args...");
		start_program( args, (char *) server_name.c_str() );
	}

	// Otherwise, this is the parent process
	else {

		// Wait for the client then start the shadow stack
		Utilities::log("Waiting for client");
		const int client_sock = QS::accept_client(sock);

		// Start the shadow stack server
		start_external_shadow_stack(client_sock);

		// If the program made it to this point, nothing
		// went wrong, gracefully exit
		tod.disable();
		exit(EXIT_SUCCESS);
	}
}
