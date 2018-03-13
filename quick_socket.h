#ifndef __QUICK_SOCKET_H__
#define __QUICK_SOCKET_H__

#include "utilities.h"


/*********************************************************/
/*                                                       */
/*                		 Functions						 */
/*                                                       */
/*********************************************************/


// Create a unix socket at fname, and a server for it
// Returns the server file descriptor
const int create_server(const char * fname);

// Create a client for a unix socket
// Joins the unix socked located at sock_name
// Returns the file descriptor for the client
const int create_client(const char * const sock_name);

// Wait for a client to connect to sock
// Once the client connects, accept then return the file descriptor
const int accept_client(const int sock);

// Generate an unused temp filename of max length size - 1
// Returns the file name, dynamically allocated
char * gen_new_filename(const unsigned int size);


#endif
