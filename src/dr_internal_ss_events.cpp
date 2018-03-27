#include "dr_internal_ss_events.hpp"
#include "constants.hpp"
#include "utilities.hpp"
#include "message.hpp"
#include "get_tid.hpp"
#include "group.hpp"

#include <stack>

#ifdef DEBUG_MODE
#include "drsyms.h"
#endif


// The stack of shadow stacks that holds the return addresses of the program
// Everytime a signal handler is called, a shadow stack is pushed with a wildcard
// Everytime we return from a signal handler, the stack pops a wildcard
std::stack<app_pc> shadow_stack;

#ifdef DEBUG_MODE

// A function copied from a sample.
// Just prints out the symbols associated with the return addresses
#define MAX_SYM_RESULT 256
static void print_address(app_pc addr, const char *prefix) {
    drsym_error_t symres;
    drsym_info_t sym;
    char name[MAX_SYM_RESULT];
    char file[MAXIMUM_PATH];
    module_data_t *data;
    data = dr_lookup_module(addr);
    if (data == NULL) {
        Utilities::log_error("%s " PFX " ? ??:0\n", prefix, addr);
        return;
    }
    sym.struct_size = sizeof(sym);
    sym.name = name;
    sym.name_size = MAX_SYM_RESULT;
    sym.file = file;
    sym.file_size = MAXIMUM_PATH;
    symres = drsym_lookup_address(data->full_path, addr - data->start, &sym,
                                  DRSYM_DEFAULT_FLAGS);
    if (symres == DRSYM_SUCCESS || symres == DRSYM_ERROR_LINE_NOT_AVAILABLE) {
        const char *modname = dr_module_preferred_name(data);
        if (modname == NULL)
            modname = "<noname>";
        Utilities::log_error("%s " PFX " %s!%s+" PIFX, prefix, addr,
                   modname, sym.name, addr - data->start - sym.start_offs);
        if (symres == DRSYM_ERROR_LINE_NOT_AVAILABLE) {
            Utilities::log_error(" ??:0\n");
        } else {
            Utilities::log_error(" %s:%" UINT64_FORMAT_CODE "+" PIFX "\n",
                       sym.file, sym.line, sym.line_offs);
        }
    } else
        Utilities::log_error("%s " PFX " ? ??:0\n", prefix, addr);
    dr_free_module_data(data);
}
#endif

// The call handler. 
// This function is called whenever a call instruction is about 
// to execute. This function is static for optimization reasons */
static void on_call(const app_pc ret_to_addr) {
	shadow_stack.push( (app_pc) WILDCARD );
}

// The ret handler.
// This function is called whenever a ret instruction is about 
// to execute. This function is static for optimization reasons */
static void on_ret(app_pc, const app_pc target_addr) {

	// Log the address being returned to
	Utilities::log("TID %d: Ret(%p)", get_tid(), target_addr);

	// If the shadow stack is empty, we cannot return
	if ( shadow_stack.empty() ) {
		TerminateOnDestruction tod;
#ifdef DEBUG_MODE
		print_address((app_pc) target_addr, "ret: " );
		Utilities::log_error( 	"*** Shadow stack mistmach detected! ***\n"
								"Attempting to return to %p\n"
								"\tShadow stack is empty!\n", target_addr);
		Group::terminate(nullptr);
#endif
	}

	// If the addresses match, return
	const app_pc top = shadow_stack.top();
	if (top == target_addr) {
		shadow_stack.pop();
		return;
	}

	// Check to see if the top of the stack is a wildcard
	else if ( top == (app_pc) WILDCARD ) {
		Utilities::log("Wildcard detected. Returning from signal handler.");
		shadow_stack.pop();
		return;
	}

	// Otherwise, if the top of the shadow stack 
	// differs from the return address, error
	else {
		TerminateOnDestruction tod;
#ifdef DEBUG_MODE
		print_address((app_pc) stk.top(), "top of shadow stack:" );
		print_address((app_pc) target_addr, "ret:" );
#endif
		Utilities::log_error( 	"*** Shadow stack mistmach detected! ***\n"
								"Attempting to return to %p\n"
								"\tTop of shadow stack is %p\n", target_addr, top);
		Group::terminate(nullptr);
	}
}

// Called whenever a signal is called. Adds a wildcard to the shadow stack
static dr_signal_action_t signal_event(void *drcontext, dr_siginfo_t *info) {
	Utilities::log("Caught sig %d\n", info->sig);
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
dr_emit_flags_t InternalSS::event_app_instruction(	void * drcontext, void * tag, 
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

// Called on exit of client program
// Checks how the client returned then exits
void InternalSS::exit_event() {
	using namespace Utilities;
	assert(	drmgr_unregister_bb_insertion_event(event_app_instruction),
						"client process returned improperly." );
#ifdef DEBUG_MODE
	assert( drreg_exit() == DRREG_SUCCESS, 
		"client process returned improperly." );
#endif
	drmgr_exit();
	log("Program ended without issue");
}

// Setup the internal stack server for the DynamoRIO client
void InternalSS::setup(const char * const socket_path) {

	// Whenever a singal is caught, we add a wildcard to the stack
	drmgr_register_signal_event(signal_event);

	// For debugging
#if DEBUG_MODE
    Utilities::assert( drsym_init(0) == DRSYM_SUCCESS),
		"unable to initialize symbol translation\n" );
	Utilities::assert( drreg_init(&ops) == DRREG_SUCCESS),
		"something went wrong with DynamoRIO in setup." );
#endif

	// Error checking
	Utilities::assert( drmgr_init(),
		"something went wrong with DynamoRIO in setup." );
}
