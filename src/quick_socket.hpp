/** @file */
#ifndef __QUICK_SOCKET_HPP__
#define __QUICK_SOCKET_HPP__


/*********************************************************/
/*                                                       */
/*                		 Functions						 */
/*                                                       */
/*********************************************************/


// Protect the global namespace
namespace QS {

	/// Create a unix socket at fname, and a server for it
	/** Returns the server file descriptor */
	int create_server(const char * fname);

	/// Create a client for a unix socket
	/** Joins the unix socked located at sock_name
	 *  Returns the file descriptor for the client */
	int create_client(const char * const sock_name);

	/// Wait for a client to connect to sock
	/** Once the client connects, accept then return the file descriptor */
	int accept_client(const int sock);

};

#endif
