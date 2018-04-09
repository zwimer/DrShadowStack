#include "dr_external_ss_events.hpp"
#include "quick_socket.hpp"
#include "utilities.hpp"
#include "message.hpp"

#include "drmgr.h"


// The socket to be used by this client
static int sock = -1;


// The call handler. 
// This function is called whenever a call instruction is about 
// to execute. This function is static for optimization reasons */
static void on_call(const app_pc ret_to_addr) {
	Utilities::verbose_log("(client) Call @ ", (void *) ret_to_addr, " - 0x5");
	send_msg<Message::Call>(sock, (char *) & ret_to_addr );
}

// The ret handler. 
// This function is called whenever a ret instruction is about 
// to execute. This function is static for optimization reasons */
static void on_ret(const app_pc instr_addr, const app_pc target_addr) {
	Utilities::verbose_log("(client) Ret to ", (void *) target_addr);
	send_msg<Message::Ret>(sock, (char *) & target_addr );
	(void) recv_msg<Message::Continue>(sock);
}

// The fork event handler
// This function is called by the child process after a fork
void fork_event( void * ) {
	Utilities::verbose_log("(client) Fork event caught");
	// TODO
}

// The thread event handler
// This function is called by the new thread whenever the process threads
void thread_event( void * ) {
	Utilities::verbose_log("(client) Thread event caught");
	// TODO
}

// Called whenever a signal is called. Adds a wildcard to the shadow stack
// Note: the reason we use this instead of the signal event is this ignores ignored signals
static void kernel_xfer_event_handler(void *, const dr_kernel_xfer_info_t * info) {
	if (info->type == DR_XFER_SIGNAL_DELIVERY) {
		Utilities::verbose_log(	"Caught sig ", info->sig, " - ", strsignal(info->sig),
								"\t\n- Handler address = ", (void *) info->target_pc);
		send_msg<Message::NewSignal>(sock);
	}
}

// The function that inserts the call and ret handlers
// Whenever a new basic block is seen, this function will be
// called once for each instruction in it. If either a call
// or a ret is seen, the call and ret handlers are inserted 
// before said instruction. Note: an app_pc is defined in comments
dr_emit_flags_t external_event_app_instruction(	void *drcontext, void *tag, 
												instrlist_t *bb, instr_t *instr, 
												bool for_trace, bool translating, 
												void *user_data ) {

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
void exit_event() {
	Utilities::assert(	drmgr_unregister_bb_insertion_event(external_event_app_instruction),
						"client process returned improperly." );
	drmgr_exit();
}


/*********************************************************/
/*                                                       */
/*                  	  From Header					 */
/*                                                       */
/*********************************************************/


// Setup the external stack server for the DynamoRIO client
void ExternalSS::setup(const char * const socket_path) {

	// Setup
	Utilities::assert( drmgr_init(), "drmgr_init() failed." );

	// Register events
	dr_register_exit_event(exit_event);
	dr_register_fork_init_event(fork_event);
	drmgr_register_thread_init_event(thread_event);

	// Create the socket to be used
	Utilities::log("Client connecting to ", socket_path);
	sock = QS::create_client(socket_path);

	// The event used to re-route call and ret's
    drmgr_register_bb_instrumentation_event(NULL, external_event_app_instruction, NULL);

	// Whenever a non-ignored singal is caught, we add a wildcard to the stack
	drmgr_register_kernel_xfer_event(kernel_xfer_event_handler);
}
