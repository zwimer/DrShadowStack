#include "shadow_stack.hpp"
#include "external_stack_server.hpp"
#include "quick_socket.hpp"
#include "parse_args.hpp"
#include "constants.hpp"
#include "utilities.hpp"
#include "group.hpp"

#include <boost/filesystem.hpp>
#include <unistd.h>
#include <signal.h>
#include <vector>


// Undefine macro assert, it clobbers the class member function assert
#undef assert


// Return a non-existent filename
std::string temp_name() {

	// For brevity
	namespace fs = boost::filesystem;

	// Create the model to be used
	static constexpr const int basename_size = 50;
	static const fs::path tmp_dir = fs::temp_directory_path();
	static const fs::path model(tmp_dir.string() + "/" + std::string(50, '%').c_str());

	// Generate the unique path
	try {
		const std::string ret = fs::unique_path(model).string();
		return std::move(ret);
	}

	// If failure, note so
	catch (...) {
		Utilities::err("boost::filesystem::unique_path() failed.");
	}
}

// Start's the program passed in via drrun
void start_program( const Args input_args, char * socket_path ) {

	// Construct args to give to exec
	std::vector<const char *> exec_args;
	
	// Drrun a client
	exec_args.push_back(DRRUN_PATH);
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
	Utilities::log(pnt.str());
	fflush(NULL);

	// Start drrun
	execvp(DRRUN_PATH, (char **) exec_args.data());
	Utilities::err("execvp() failed.");
}


// Main function
int main(int argc, char * argv[]) {

	// Setup utilities
	Utilities::setup(true);
	Utilities::log("DrShadowStack initalized");

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
