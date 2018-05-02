#include "dr_shadow_stack_client.hpp"
#include "dr_external_ss_events.hpp"
#include "quick_socket.hpp"
#include "utilities.hpp"
#include "constants.hpp"
#include "message.hpp"

#include "drmgr.h"

#include <syscall.h>


// The socket to be used by this client
static int sock = -1;


// The call handler.
// This function is called whenever a call instruction is about
// to execute. This function is static for optimization reasons */
static void on_call( const app_pc ret_to_addr ) {
	Utilities::verbose_log( "(client) Call @ ", (void *) ret_to_addr, " - 0x5" );
	send_msg<Message::Call>( sock, (char *) &ret_to_addr );
}

// The ret handler.
// This function is called whenever a ret instruction is about
// to execute. This function is static for optimization reasons */
static void on_ret( const app_pc instr_addr, const app_pc target_addr ) {
	Utilities::verbose_log( "(client) Ret to ", (void *) target_addr );
	send_msg<Message::Ret>( sock, (char *) &target_addr );
	(void) recv_msg<Message::Continue>( sock );
}

// Called whenever a signal is called. Adds a wildcard to the shadow stack
// Note: the reason we use this instead of the signal event is this ignores ignored
// signals
static void on_signal() { send_msg<Message::NewSignal>( sock ); }


/*********************************************************/
/*                                                       */
/*                 Hooking syscall functions             */
/*                                                       */
/*********************************************************/


// This function dictates what syscall is interesting
static bool syscall_filter( void *drcontext, int sysnum ) {
	switch ( sysnum ) {
		/* case SYS_fork: */
		/* case SYS_vfork: */
		/* case SYS_clone: */
		case SYS_execve:
			return true;
		default:
			return false;
	};
}

// Called before execve is called
static inline void on_execve( void *drcontext, bool ) {

	// Send the execve message
	send_msg<Message::Execve>( sock );

	// Get the enviornment
	const char **const env = (const char **) dr_syscall_get_param( drcontext, 2 );

	// Locate the DR_SS_ENV_FD
	const char **next;
	const size_t len = strlen( DR_SS_ENV_FD );
	for ( next = env; strncmp( *next, DR_SS_ENV_FD, len ) != 0; ++next ) {
	}

	// Replace the variable's value with our desired value
	const char *const fd_str = getenv( DR_SS_ENV_FD );
	Utilities::assert( fd_str != nullptr, "DR_SS_ENV_FD not set on call to execve" );
	std::stringstream s;
	s << DR_SS_ENV_FD << "=" << fd_str;
	*next = strdup( s.str().c_str() );

	// Update the syscall's arguments
	dr_syscall_set_param( drcontext, 2, (reg_t) env );
}


// Called whenever an interesting syscall is found
// This just delegates to the syscall specific function
static inline void syscall_event( void *drcontext, const int sysnum, const bool pre ) {
	switch ( sysnum ) {
		case SYS_execve:
			on_execve( drcontext, pre );
		default:
		    /* Need a ; as this is the last statement */;
	};
}

// Called before every interesting syscall
static bool pre_syscall_event( void *drcontext, const int sysnum ) {
	syscall_event( drcontext, sysnum, true );
	return true;
}

// Called after every interesting syscall
static void post_syscall_event( void *drcontext, const int sysnum ) {
	syscall_event( drcontext, sysnum, false );
}

/*********************************************************/
/*                                                       */
/*                       From Header                     */
/*                                                       */
/*********************************************************/


// Setup the external stack server for the DynamoRIO client
void ExternalSS::setup( SSHandlers **const handlers, const char *const socket_path ) {
	*handlers = new SSHandlers( on_call, on_ret, on_signal );

	// Setup the socket
	const char *const fd_str = getenv( DR_SS_ENV_FD );
	Utilities::assert( fd_str != nullptr, "getenv() failed." );
	if ( fd_str[0] != (char) 0 ) {
		Utilities::log( "Existing socket connected detected: ", socket_path );
		sock = stoi( std::string( fd_str ) );
		Utilities::log( "\t- Using file descriptor ", sock,
		                " as socket fd, as it is already connected..." );
	}
	else {
		Utilities::log( "Client connecting to ", socket_path );
		sock = QS::create_client( socket_path );
		const auto sock_str = std::to_string( sock );
		Utilities::assert( setenv( DR_SS_ENV_FD, sock_str.c_str(), true ) == 0,
		                   "setenv() failed." );
		Utilities::log( "Set environment variable " DR_SS_ENV_FD " to ", sock_str );
	}

	// Hook syscalls
	Utilities::log( "Hooking syscalls..." );
	dr_register_filter_syscall_event( syscall_filter );
	drmgr_register_pre_syscall_event( pre_syscall_event );
	drmgr_register_post_syscall_event( post_syscall_event );
}
