#include"dr_internal_ss_events.hpp"




// The function that inserts the call and ret handlers
// Whenever a new basic block is seen, this function will be
// called once for each instruction in it. If either a call
// or a ret is seen, the call and ret handlers are inserted 
// before said instruction. Note: an app_pc is defined in comments
dr_emit_flags_t InternalSS::event_app_instruction(	void *drcontext, void *tag, 
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
		// TODO
		/* const app_pc xip = instr_get_app_pc(instr) + instr_length(drcontext, instr); */
		/* dr_insert_clean_call(drcontext, bb, instr, (void *) on_call, */
		/* 					false /1* save floating point state *1/, */ 
		/* 					1, OPND_CREATE_INTPTR(xip)); */
	}

	// If the instruction is a ret, insert the ret handler as an
	// mbr_implementation so as to gain access to the info we need
	if ( instr_is_return(instr) ) {
		// TODO
		/* dr_insert_mbr_instrumentation(drcontext, bb, instr, */ 	
		/* 	(void *) on_ret, SPILL_SLOT_1); */
	}

	// All went well
    return DR_EMIT_DEFAULT;
}

// Setup the internal stack server for the DynamoRIO client
void InternalSS::setup(const char * const) {
}
