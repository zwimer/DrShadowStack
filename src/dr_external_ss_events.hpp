/** @file */
#ifndef __DR_EXTERNAL_SS_EVENTS_HPP__
#define __DR_EXTERNAL_SS_EVENTS_HPP__

#include "dr_api.h"


/** Make a distinction between the internal and external SS functions */
namespace ExternalSS {

	/// The function that inserts the call and ret handlers
	/** Whenever a new basic block is seen, this function will be
	 *  called once for each instruction in it. If either a call
	 *  or a ret is seen, the call and ret handlers are inserted 
	 *  before said instruction. Note: an app_pc is defined in comments */
	dr_emit_flags_t event_app_instruction(	void *drcontext, void *tag,
		  									instrlist_t *bb, instr_t *instr, 
											bool for_trace, bool translating, 
											void *user_data );

	/** Setup the external stack server for the DynamoRIO client */
	void setup(const char * const socket_path);
};

#endif
