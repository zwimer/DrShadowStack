#include "dr_internal_ss_events.hpp"
#include "dr_print_sym.hpp"
#include "constants.hpp"
#include "utilities.hpp"
#include "proc_rc.hpp"
#include "group.hpp"

#include <stack>

// The stack of shadow stacks that holds the return addresses of the program
// Everytime a signal handler is called, a shadow stack is pushed with a wildcard
// Everytime we return from a signal handler, the stack pops a wildcard
std::stack<app_pc> shadow_stack;


// The call handler. 
// This function is called whenever a call instruction is about 
// to execute. This function is static for optimization reasons */
static void on_call(const app_pc ret_to_addr) {
	Utilities::verbose_log("Call(%p)", ret_to_addr );
	shadow_stack.push( ret_to_addr );
}

// The ret handler.
// This function is called whenever a ret instruction is about 
// to execute. This function is static for optimization reasons */
static void on_ret(app_pc, const app_pc target_addr) {

	// For clarity
	const constexpr auto log_error = Utilities::log_error;
	const constexpr auto message = Utilities::message;

	// Log the address being returned to
	Utilities::verbose_log("Ret(%p)", target_addr);

	// If the shadow stack is empty, we cannot return
	if ( shadow_stack.empty() ) {
		TerminateOnDestruction tod;
		Sym::print(message, log_error, "return address", target_addr);
		log_error(	"*** Shadow stack mistmach detected! ***\n"
					"Attempting to return to %p\n"
					"\tShadow stack is empty!\n", target_addr );
		Group::terminate(nullptr);
	}

	// If the addresses match, return
	const app_pc top = shadow_stack.top();
	if (top == target_addr) {
		shadow_stack.pop();
		return;
	}

	// Check to see if the top of the stack is a wildcard
	else if ( top == (app_pc) WILDCARD ) {
		Utilities::verbose_log("Wildcard detected. Returning from signal handler.");
		shadow_stack.pop();
		return;
	}

	// Otherwise, if the top of the shadow stack 
	// differs from the return address, error
	else {
		TerminateOnDestruction tod;

		// Print out the mismatch error
		log_error( "*** Shadow stack mistmach detected! ***\n"
			 "Attempting to return to %p\n"
			 "\tTop of shadow stack is %p\n", target_addr, top );

		// Print out symbol information, then terminate the group
		Sym::print(message, log_error, "top of shadow stack", (app_pc) shadow_stack.top());
		Sym::print(message, log_error, "return address", target_addr);
		Group::terminate(nullptr);
	}
}

// Called whenever a signal is called. Adds a wildcard to the shadow stack
static dr_signal_action_t signal_event(void *drcontext, dr_siginfo_t *info) {
	Utilities::verbose_log("Caught sig %d\n", info->sig);
	shadow_stack.push( (app_pc) WILDCARD );
    return DR_SIGNAL_DELIVER;
}


/*********************************************************/
/*                                                       */
/*                  	  From Header					 */
/*                                                       */
/*********************************************************/


// The function that inserts the call and ret handlers
// Whenever a new basic block is seen, this function will be
// called once for each instruction in it. If either a call
// or a ret is seen, the call and ret handlers are inserted 
// before said instruction. Note: an app_pc is defined in comments
dr_emit_flags_t internal_event_app_instruction(	void * drcontext, void * tag, 
												instrlist_t * bb, instr_t *instr, 
												bool for_trace, bool translating, 
												void * user_data ) {

	// Concerning DynamoRIO's app_pc type. From their source"
	// include/dr_defines.h:typedef byte * app_pc;
	// tools/DRcontrol.c:typedef unsigned char byte;
	// Thus app_pc is simply an unsigned char *

	// If the instruction is a call, get the address, 
	// add the size of the call instruction (to get the 
	//return address), then insert the on_call function 
	// with the return address as a parameter
	if ( instr_is_call(instr) ) {
		const app_pc xip = instr_get_app_pc(instr) + instr_length(drcontext, instr);
		dr_insert_clean_call(drcontext, bb, instr, (void *) on_call,
							false /* save floating point state */, 
							1, OPND_CREATE_INTPTR(xip));
	}

	// If the instruction is a ret, insert the ret handler as an
	// mbr_implementation so as to gain access to the info we need
	if ( instr_is_return(instr) ) {
		dr_insert_mbr_instrumentation(drcontext, bb, instr, 	
			(void *) on_ret, SPILL_SLOT_1);
	}

	// All went well
    return DR_EMIT_DEFAULT;
}

// Increments the program reference counter
// This is a wrapper that can be used as an event
void proc_rc_inc(void *) {
	prc->inc();
}

// Decrements the program reference counter
// This is a wrapper that can be used as an event
void proc_rc_dec(void *) {
	prc->dec();
}

// Setup the internal stack server for the DynamoRIO client
void InternalSS::setup(const char * const socket_path) {

	// Setup
	Utilities::assert( drmgr_init(), "drmgr_init() failed." );
	Sym::init();

	// Handle forking and threading, and register the exit event (via thread exit)
	// Note: On exit, thread_exit will be called, so no general exit event is needed
	drmgr_register_thread_exit_event(proc_rc_dec);
	drmgr_register_thread_init_event(proc_rc_inc);
	dr_register_fork_init_event(proc_rc_inc);

	// The event used to re-route call and ret's
    drmgr_register_bb_instrumentation_event(NULL, internal_event_app_instruction, NULL);

	// Whenever a singal is caught, we add a wildcard to the stack
	drmgr_register_signal_event(signal_event);
}
