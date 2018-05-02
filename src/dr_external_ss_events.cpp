#include "dr_shadow_stack_client.hpp"
#include "dr_external_ss_events.hpp"
#include "quick_socket.hpp"
#include "utilities.hpp"
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
static void on_signal() {
	send_msg<Message::NewSignal>( sock );
}


/*********************************************************/
/*                                                       */
/*                 Hooking syscall functions             */
/*                                                       */
/*********************************************************/


// This function dictates what syscall is interesting
static bool syscall_filter(void *drcontext, int sysnum) {
	switch(sysnum) {
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
static inline void on_execve(bool pre) {
	send_msg<Message::Execve>( sock );
}


// Called whenever an interesting syscall is found
// This just delegates to the syscall specific function
static inline void syscall_event( const int sysnum, const bool pre ) {
	switch(sysnum) {
		case SYS_execve:
			on_execve(pre);
		default:
			/* Need a ; as this is the last statement */ ;
	};
}

// Called before every interesting syscall
static bool pre_syscall_event( void *, const int sysnum) {
	syscall_event(sysnum, true);
	return true;
}

// Called after every interesting syscall
static void post_syscall_event( void *, const int sysnum) {
	syscall_event(sysnum, false);
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
	Utilities::log( "Client connecting to ", socket_path );
	sock = QS::create_client( socket_path );
/* write(6, "test\n", 5); */
Utilities::message("SOCK", sock);

	// Hook syscalls
	Utilities::log( "Hooking syscalls..." );
    dr_register_filter_syscall_event(syscall_filter);
    drmgr_register_pre_syscall_event(pre_syscall_event);
    drmgr_register_post_syscall_event(post_syscall_event);
}
