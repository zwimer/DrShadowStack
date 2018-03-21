/** @file */
#include "constants.hpp"
#include "utilities.hpp"
#include "quick_socket.hpp"

#include "dr_api.h"
#include "drmgr.h"

#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <string>


// The socket to be used by this client
int sock = -1;


/// The call handler. 
/** This function is called whenever a call instruction is about 
 *  to execute. This function is static for optimization reasons */
static void on_call(const void * const ret_to_addr) {

	// Log the call
	ss_log("Call(%.*s)", POINTER_SIZE, ret_to_addr);

	// Create the message to send
	const int num_bytes = MESSAGE_HEADER_LENGTH + POINTER_SIZE;
	char buffer[num_bytes];
	memcpy(buffer, CALL, MESSAGE_HEADER_LENGTH);
	memcpy( & buffer[MESSAGE_HEADER_LENGTH], ret_to_addr, POINTER_SIZE);

	// Send the message
	ss_assert( write(sock, buffer, num_bytes) == num_bytes, "write() failed!");
}

/// The ret handler. 
/** This function is called whenever a ret instruction is about 
 *  to execute. This function is static for optimization reasons */
static void on_ret(app_pc instr_addr, app_pc target_addr) {

	// Log the ret
	ss_log("Ret(%.*s)", POINTER_SIZE, target_addr);

	// Create the message to send
	const int num_bytes = MESSAGE_HEADER_LENGTH + POINTER_SIZE;
	char buffer[num_bytes];
	memcpy(buffer, RET, MESSAGE_HEADER_LENGTH);
	memcpy( & buffer[MESSAGE_HEADER_LENGTH], (void *) target_addr, POINTER_SIZE);

	// Send the ret message
	ss_assert( write(sock, buffer, num_bytes) == num_bytes, "write() failed!");

	// Wait until a we get a 'continue'
	const int bytes_recv = recv( sock, buffer, MESSAGE_HEADER_LENGTH, MSG_WAITALL );
	ss_assert( bytes_recv == MESSAGE_HEADER_LENGTH, "Did not get full size message!");
	ss_assert( memcmp(buffer, CONTINUE, MESSAGE_HEADER_LENGTH) == 0, 
		"Received incorrect message!");
}

/// The function that inserts the call and ret handlers
/** Whenever a new basic block is seen, this function will be
 *  called once for each instruction in it. If either a call
 *  or a ret is seen, the call and ret handlers are inserted 
 * before said instruction. Note: an app_pc is defined in comments */
static dr_emit_flags_t event_app_instruction(void *drcontext, void *tag, 
			instrlist_t *bb, instr_t *instr, bool for_trace, 
			bool translating, void *user_data) {

	// Concerning DynamoRIO's app_pc type. From their source"
	// include/dr_defines.h:typedef byte * app_pc;
	// tools/DRcontrol.c:typedef unsigned char byte;
	// Thus app_pc is simply an unsigned char *

	// If the instruction is a call, get the address, 
	// add the size of the call instruction (to get the 
	//return address), then insert the on_call function 
	// with the return address as a parameter
	if ( instr_is_call(instr) ) {
		const void * const xip = (char *) instr_get_app_pc(instr) 
								 + instr_length(drcontext, instr);
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


/// The main client function
/** This function dynamically 'injects' the shadow stack */
DR_EXPORT void dr_client_main(client_id_t id, int argc, const char *argv[]) {	

	// TODO: use arg parser
	ss_log("DynamoRIO client started");
	ss_assert(argc == 2, "Incorrect usage. Dr_client_main expects 1 argument"
							" which gives the unix domain socket to connect to.");

	// Setup the client
    dr_set_client_name("ShadowStack DynamoRIO Client 'ShadowStack'", "http://github.com/zwimer/ShadowStack");
    drmgr_init();

    // Register events
    dr_register_exit_event(drmgr_exit);
    drmgr_register_bb_instrumentation_event(NULL, event_app_instruction, NULL);

    // Make it easy to tell, by looking at log file, which client executed
    dr_log(NULL, DR_LOG_ALL, 1, "Client 'ShadowStack' initializing\n");

	// Create the socket to be used
	ss_log("Client connecting to %s", argv[1]);
	sock = create_client(argv[1]);
}
