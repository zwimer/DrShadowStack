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
static void on_call(const app_pc ret_to_addr) {
	Utilities::verbose_log("Call @ ", (void *) ret_to_addr );
	shadow_stack.push( ret_to_addr );
}

// The ret handler.
// This function is called whenever a ret instruction is about 
// to execute. This function is static for optimization reasons */
static void on_ret(app_pc, const app_pc target_addr) {

	// Log the address being returned to
	Utilities::verbose_log("Ret to ", (void *) target_addr);

	// If the shadow stack is empty, we cannot return
	if ( shadow_stack.empty() ) {
		TerminateOnDestruction tod;
		Sym::print("return address", target_addr);
		Utilities::log_error(	"*** Shadow stack mistmach detected! ***\n"
								"Attempting to return to ", (void *) target_addr,
								"\n\tShadow stack is empty!\n" );
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
		Utilities::log_error(	"*** Shadow stack mistmach detected! ***\n"
								"Attempting to return to ", (void *) target_addr,
								"\n\tTop of shadow stack is ", (void *) top, '\n' );

		// Print out symbol information, then terminate the group
		Sym::print("top of shadow stack", (app_pc) shadow_stack.top());
		Sym::print("return address", target_addr);
		Group::terminate(nullptr);
	}
}

// Called whenever a signal is called. Adds a wildcard to the shadow stack
// Note: the reason we use this instead of the signal event is this ignores ignored signals
static void kernel_xfer_event_handler(void *, const dr_kernel_xfer_info_t * info) {
	if (info->type == DR_XFER_SIGNAL_DELIVERY) {
		Utilities::verbose_log(	"Caught sig ", info->sig, " - ", strsignal(info->sig),
								"\t\n- Handler address = ", (void *) info->target_pc);
		shadow_stack.push( (app_pc) WILDCARD );
	}
}

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


/*********************************************************/
/*                                                       */
/*                  	  From Header					 */
/*                                                       */
/*********************************************************/


// Setup the internal stack server for the DynamoRIO client
void InternalSS::setup(const char * const socket_path) {

	// Setup
	Utilities::assert( drmgr_init(), "drmgr_init() failed." );
	Sym::init();

	// The event used to re-route call and ret's
    drmgr_register_bb_instrumentation_event(NULL, internal_event_app_instruction, NULL);

	// Whenever a non-ignored singal is caught, we add a wildcard to the stack
	drmgr_register_kernel_xfer_event(kernel_xfer_event_handler);
}
