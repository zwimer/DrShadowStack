#include "dr_external_ss_events.hpp"
#include "quick_socket.hpp"
#include "utilities.hpp"
#include "message.hpp"

#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>

#include "drmgr.h"


// The socket to be used by this client
static int sock = -1;


// The event that occurs before a signal is given to the program
static dr_signal_action_t signal_event(void *drcontext, dr_siginfo_t *info) {

	// Log the signal
	Utilities::verbose_log("(client) Caught signal %d", info->sig);

	// Create and send the message
	char buffer[sizeof(intmax_t) + 1];
	sprintf(buffer, "%d", info->sig);
	Message::NewSignal msg( buffer );
	const int bytes_sent = write(sock, msg.message, msg.size);
	Utilities::assert( bytes_sent == msg.size, "write() failed!");

	// Deliver the signal
    return DR_SIGNAL_DELIVER;
}

// The call handler. 
// This function is called whenever a call instruction is about 
// to execute. This function is static for optimization reasons */
static void on_call(const app_pc ret_to_addr) {

	// Log the call
	Utilities::verbose_log("(client) Call(%p)", ret_to_addr);

	// Create and send the message
	Message::Call to_send( (char *) & ret_to_addr );
	const int bytes_sent = write(sock, to_send.message, to_send.size);
	Utilities::assert( bytes_sent == to_send.size, "write() failed!");
}

// The ret handler. 
// This function is called whenever a ret instruction is about 
// to execute. This function is static for optimization reasons */
static void on_ret(const app_pc instr_addr, const app_pc target_addr) {

	// Log the ret
	Utilities::verbose_log("(client) Ret(%p)", target_addr);

	// Create and send the message
	Message::Ret to_send( (char *) & target_addr );
	const int bytes_sent = write(sock, to_send.message, to_send.size);
	Utilities::assert( bytes_sent == to_send.size, "write() failed!");

	// For clarity
	const constexpr int size = Message::Continue::size;
	const constexpr auto is_continue = Message::is_a_valid<Message::Continue>;

	// Wait until a we get a 'continue'
	char buffer[size];
	const int bytes_recv = recv( sock, buffer, size, MSG_WAITALL );
	Utilities::assert( bytes_recv == size, "Did not get full size message!");
	Utilities::assert( is_continue(buffer), "Received incorrect message!");
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
dr_emit_flags_t ExternalSS::event_app_instruction(	void *drcontext, void *tag, 
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
void ExternalSS::exit_event() {
	Utilities::assert(	drmgr_unregister_bb_insertion_event(event_app_instruction),
						"client process returned improperly." );
	drmgr_exit();
}

// Setup the external stack server for the DynamoRIO client
void ExternalSS::setup(const char * const socket_path) {

	// Create the socket to be used
	Utilities::log("Client connecting to %s", socket_path);
	sock = QS::create_client(socket_path);

	// Whenever a singal is caught, we add a wildcard to the stack
	drmgr_register_signal_event(signal_event);
}
