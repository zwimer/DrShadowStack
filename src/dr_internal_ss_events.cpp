#include "dr_internal_ss_events.hpp"
#include "dr_print_sym.hpp"
#include "constants.hpp"
#include "utilities.hpp"
#include "group.hpp"

#include <stack>


// The stack of shadow stacks that holds the return addresses of the program
// Everytime a signal handler is called, a shadow stack is pushed with a wildcard
// Everytime we return from a signal handler, the stack pops a wildcard
std::stack<app_pc> shadow_stack;


// The call handler.
// This function is called whenever a call instruction is about
// to execute. This function is static for optimization reasons */
void on_call( const app_pc ret_to_addr ) {
	Utilities::verbose_log( "Call @ ", (void *) ret_to_addr );
	shadow_stack.push( ret_to_addr );
}

// The ret handler.
// This function is called whenever a ret instruction is about
// to execute. This function is static for optimization reasons */
void on_ret( app_pc, const app_pc target_addr ) {

	// Log the address being returned to
	Utilities::verbose_log( "Ret to ", (void *) target_addr );

	// If the shadow stack is empty, we cannot return
	if ( shadow_stack.empty() ) {
		TerminateOnDestruction tod;
		Sym::print( "return address", target_addr );
		Utilities::log_error( "*** Shadow stack mistmach detected! ***\n"
		                      "Attempting to return to ",
		                      (void *) target_addr, "\n\tShadow stack is empty!\n" );
		Group::terminate( nullptr );
	}

	// If the addresses match, return
	const app_pc top = shadow_stack.top();
	if ( top == target_addr ) {
		shadow_stack.pop();
		return;
	}

	// Check to see if the top of the stack is a wildcard
	else if ( top == (app_pc) WILDCARD ) {
		Utilities::verbose_log( "Wildcard detected. Returning from signal handler." );
		shadow_stack.pop();
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
		Sym::print( "top of shadow stack", (app_pc) shadow_stack.top() );
		Sym::print( "return address", target_addr );
		Group::terminate( nullptr );
	}
}

// Called whenever a signal is called. Adds a wildcard to the shadow stack
// Note: the reason we use this instead of the signal event is this ignores ignored
// signals
void on_signal() { shadow_stack.push( (app_pc) WILDCARD ); }


/*********************************************************/
/*                                                       */
/*                       From Header                     */
/*                                                       */
/*********************************************************/


// Setup the internal stack server for the DynamoRIO client
void InternalSS::setup( SSHandlers **const handlers, const char *const socket_path ) {
	*handlers = new SSHandlers( on_call, on_ret, on_signal );
	Sym::init();
}
