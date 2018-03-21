/** @file */
#ifndef __DR_SHADOW_STACK_CLIENT__HPP__
#define __DR_SHADOW_STACK_CLIENT__HPP__

#include "dr_api.h"
#include "drmgr.h"

/// The call handler. 
/** This function is called whenever a call instruction is about 
 *  to execute. This function is static for optimization reasons */
static void on_call(const void * const ret_to_addr);

/// The ret handler. 
/** This function is called whenever a ret instruction is about 
 *  to execute. This function is static for optimization reasons */
static void on_ret(app_pc instr_addr, app_pc target_addr);

/// The function that inserts the call and ret handlers
/** Whenever a new basic block is seen, this function will be
 *  called once for each instruction in it. If either a call
 *  or a ret is seen, the call and ret handlers are inserted 
 * before said instruction. Note: an app_pc is defined in comments */
static dr_emit_flags_t event_app_instruction(void *drcontext, void *tag, 
			instrlist_t *bb, instr_t *instr, bool for_trace, 
			bool translating, void *user_data);

/// The main client function
/** This function dynamically 'injects' the shadow stack */
DR_EXPORT void dr_client_main(client_id_t id, int argc, const char *argv[]);


#endif

