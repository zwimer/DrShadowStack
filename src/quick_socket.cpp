#include "quick_socket.hpp"
#include "utilities.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <string.h>


// Undefine macro assert, it clobbers the class member function assert
#undef assert


/*********************************************************/
/*                                                       */
/*                	  Helper Functions					 */
/*                                                       */
/*********************************************************/


// Create a unix server with name fname
static struct sockaddr_un make_unix_server(const char * const fname) {
	struct sockaddr_un server;
	server.sun_family = AF_UNIX;
 	strcpy(server.sun_path, fname);
	return server;
}


/*********************************************************/
/*                                                       */
/*                	   Header functions					 */
/*                                                       */
/*********************************************************/


// Create a unix socket, and a server for it
// Returns the server file descriptor
int QS::create_server(const char * const fname) {

	// Create the server
	const int server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	Utilities::assert( server_sock != -1, "socket() failed" );

	// Define the server
	struct sockaddr_un server = make_unix_server(fname);

	// Bind the server to the socket
	const int rv = bind(server_sock, (struct sockaddr *) &server, 
		sizeof(struct sockaddr_un));
	Utilities::assert( rv != -1, "bind() failed" );

	// Begin listening for exactly one client
	Utilities::assert( listen(server_sock, 1) != -1, "listen() failed." );
	Utilities::log("Created server ", fname, "\n\t- Listening for one client...");

	// Return the server and client sockets
	return server_sock;
}

// Create a client for a unix socket
// Joins the socket located at sock_name
// Returns the file descriptor for the client
int QS::create_client(const char * sock_name) {

	// Create the client
	const int client = socket(AF_UNIX, SOCK_STREAM, 0);
	Utilities::assert( client != -1, "socket() failed" );

	// Connect the client. This is NOT blocking IF listen() was called
	struct sockaddr_un server = make_unix_server(sock_name);
	Utilities::assert( connect(client, (struct sockaddr *) &server, 
				sizeof(struct sockaddr_un)) == 0, "connect() failed" );
	Utilities::log("New client connected to ", sock_name);

	// Return the client
	return client;
}

// Wait for a client to connect to sock
// Once the client connects, accept then return the file descriptor
int QS::accept_client(const int sock) {
	const int accepted_sock = accept(sock, 0, 0);
	Utilities::assert( accepted_sock != -1, "accept() failed" );
	Utilities::log("Server on fd ", sock, " accepted one client");
	return accepted_sock;
}
