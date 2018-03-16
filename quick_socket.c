#include "quick_socket.h"
#include "utilities.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <string.h>
#include <stdlib.h>


/*********************************************************/
/*                                                       */
/*                	  Helper Functions					 */
/*                                                       */
/*********************************************************/


// Create a unix server with name fname
struct sockaddr_un make_unix_server(const char * const fname) {
	struct sockaddr_un server;
	server.sun_family = AF_UNIX;
 	strcpy(server.sun_path, fname);
	return server;
}

// Generate a temp filename of max length size - 1
char * gen_tmp_filename(const unsigned int size) {

	// Valid characters for the file name
	const static char * lib = 	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
								"abcdefghijklmnopqrstuvwxyz"
								"1234567890 -=_+.";
	const unsigned int lib_size = strlen(lib);

	// Allocate the filename and initalize it
	assert(size >= 7, "Filename size too short." );
	char * ret = (char *) safe_malloc( 1, size );
	strcpy(	ret, "/tmp/" );

	// Fill the rest of the filename with random characters
	for ( unsigned int i = strlen(ret); (i + 1) < size; ++i ) {
		ret[i] = lib[rand() % lib_size];
	}

	// Return the filename
	ret[size - 1] = 0;
	return ret;
} 


/*********************************************************/
/*                                                       */
/*                	   Header functions					 */
/*                                                       */
/*********************************************************/


// Generate an unused temp filename of max length size - 1
// Returns the file name, dynamically allocated
char * gen_new_filename(const unsigned int size) {

	// Construct the filename
	assert(size <= 107, "too cowardly to generate a filename"
		"longer than 108 chars. Bind will fail if so");
	char * const ret = gen_tmp_filename(size);

	// If the file exists, try again
	if ( access ( ret, F_OK ) == 0 ) {
		free( ret );
		return gen_new_filename(size);
	}

	// Return the result
	return ret;
}

// Create a unix socket, and a server for it
// Returns the server file descriptor
int create_server(const char * const fname) {

	// Create the server
	int server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	assert( server_sock != -1, "socket() failed" );

	// Define the server
	struct sockaddr_un server = make_unix_server(fname);

	// Bind the server to the socket
	int rv = bind(server_sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un));
	assert( rv != -1, "bind() failed" );

	// Begin listening for exactly one client
	listen(server_sock, 1);

	// Return the server and client sockets
	return server_sock;
}

// Create a client for a unix socket
// Joins the socket descriped by the file descriptor fd
// Returns the file descriptor for the client
int create_client(const char * sock_name) {

	// Create the client
	int client = socket(AF_UNIX, SOCK_STREAM, 0);
	assert( client != -1, "socket() failed" );

	// Connect the client. This is NOT blocking IF listen() was called
	struct sockaddr_un server = make_unix_server(sock_name);
	const int rv = connect(client, (struct sockaddr *) &server, sizeof(struct sockaddr_un));
	assert( rv != -1, "connect() failed" );

	// Return the client
	return client;
}

// Wait for a client to connect to sock
// Once the client connects, accept then return the file descriptor
int accept_client(const int sock) {
	const int accepted_sock = accept(sock, 0, 0);
	assert( accepted_sock != -1, "accept() failed" );
	return accepted_sock;
}
