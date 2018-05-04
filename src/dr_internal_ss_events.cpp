#include "dr_internal_ss_events.hpp"
#include "dr_print_sym.hpp"
#include "constants.hpp"
#include "utilities.hpp"
#include "group.hpp"

#include "drmgr.h"

#include <syscall.h>
#include <stack>
#include <map>


// A thread local shadow stack.
// The shadow stack that holds the return addresses of the current thread
// Everytime a signal handler is called, a shadow stack is pushed with a wildcard
// Everytime we return from a signal handler, the stack pops a wildcard
template <typename T> class TLS;
TLS<std::stack<app_pc>> *shadow_stack;


/*********************************************************/
/*                                                       */
/*                         TLS                           */
/*                                                       */
/*********************************************************/


// A class used to wrap DynamoRIO's thread local storage
// Stores assumes the object stored is a T. This is safe
// for any T that has a default constructor
template <typename T> class TLS {
  public:
	/** The constructor */
	TLS() : tls_index( drmgr_register_tls_field() ) {
		Utilities::assert( tls_index != -1, "drmgr_register_tls_field() failed." );
	}

	/** Get a reference to the stored T
	 *  If no drcontext is provided, this all will fetch it */
	T &get( void *drcontext = nullptr ) const {
		drcontext = ( drcontext == nullptr ) ? dr_get_current_drcontext() : drcontext;
		T *const ptr = (T *) drmgr_get_tls_field( drcontext, tls_index );
		if ( ptr != nullptr ) {
			return *ptr;
		}
		T *const new_ptr = new T();
		Utilities::assert( drmgr_set_tls_field( drcontext, tls_index, (void *) new_ptr ),
		                   "drmgr_set_tls_field() failed." );
		return *new_ptr;
	}

  private:
	/** The index of tls used for DynamoRIO's TLS API */
	const int tls_index;
};


/*********************************************************/
/*                                                       */
/*                        Handlers                       */
/*                                                       */
/*********************************************************/


// The call handler.
// This function is called whenever a call instruction is about
// to execute. This function is static for optimization reasons */
void on_call( const app_pc ret_to_addr ) {
	Utilities::verbose_log( "Call @ ", (void *) ret_to_addr );
	shadow_stack->get().push( ret_to_addr );
}

// The ret handler.
// This function is called whenever a ret instruction is about
// to execute. This function is static for optimization reasons */
void on_ret( app_pc, const app_pc target_addr ) {

	// Log the address being returned to
	Utilities::verbose_log( "Ret to ", (void *) target_addr );

	// If the shadow stack is empty, we cannot return
	std::stack<app_pc> &ss = shadow_stack->get();
	if ( ss.empty() ) {
		TerminateOnDestruction tod;
		Sym::print( "return address", target_addr );
		Utilities::log_error( "*** Shadow stack mistmach detected! ***\n"
		                      "Attempting to return to ",
		                      (void *) target_addr, "\n\tShadow stack is empty!\n" );
		Group::terminate( nullptr );
	}

	// If the addresses match, return
	const app_pc top = ss.top();
	if ( top == target_addr ) {
		ss.pop();
		return;
	}

	// Check to see if the top of the stack is a wildcard
	else if ( top == (app_pc) WILDCARD ) {
		Utilities::verbose_log( "Wildcard detected. Returning from signal handler." );
		ss.pop();
		return;
	}

	// Otherwise, if the top of the shadow stack
	// differs from the return address, error
	else {
		TerminateOnDestruction tod;

		// Print out the mismatch error
		Utilities::log_error( "*** Shadow stack mistmach detected! ***\n"
		                      "Attempting to return to ",
		                      (void *) target_addr, "\n\tTop of shadow stack is ",
		                      (void *) top, '\n' );

		// Print out symbol information, then terminate the group
		Sym::print( "top of shadow stack", (app_pc) ss.top() );
		Sym::print( "return address", target_addr );
		Group::terminate( nullptr );
	}
}

// Called whenever a signal is called. Adds a wildcard to the shadow stack
// Note: the reason we use this instead of the signal event is this ignores ignored
// signals
void on_signal() { shadow_stack->get().push( (app_pc) WILDCARD ); }


/*********************************************************/
/*                                                       */
/*                 Hooking syscall functions             */
/*                                                       */
/*********************************************************/


// This function dictates what syscall is interesting
static bool syscall_filter( void *, int sysnum ) {
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
static inline void on_execve( void *, bool ) {
	Utilities::verbose_log( "execve syscall detected, clearing shadow stack!" );
	std::stack<app_pc> &ss = shadow_stack->get();
	while ( ss.size() ) {
		ss.pop();
	}
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


// Setup the internal stack server for the DynamoRIO client
void InternalSS::setup( SSHandlers **const handlers, const char *const ) {

	// Setup handlers
	*handlers = new SSHandlers( on_call, on_ret, on_signal );
	Sym::init();

	// Setup shadow stack
	shadow_stack = new TLS<std::stack<app_pc>>();

	// Hook syscalls
	Utilities::log( "Hooking syscalls..." );
	dr_register_filter_syscall_event( syscall_filter );
	drmgr_register_pre_syscall_event( pre_syscall_event );
	drmgr_register_post_syscall_event( post_syscall_event );
}
