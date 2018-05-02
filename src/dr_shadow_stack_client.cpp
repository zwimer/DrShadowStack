#include "dr_shadow_stack_client.hpp"
#include "dr_internal_ss_events.hpp"
#include "dr_external_ss_events.hpp"
#include "constants.hpp"
#include "utilities.hpp"
#include "ss_mode.hpp"
#include "group.hpp"

#include "drmgr.h"


// The mode specific shadow stack events to be used.
SSHandlers *handlers = nullptr;


/*********************************************************/
/*                                                       */
/*                       From header                     */
/*                                                       */
/*********************************************************/


// Constructor
SSHandlers::SSHandlers( SSHandlers::on_call_signature c, SSHandlers::on_ret_signature r,
                        SSHandlers::on_signal_signature s )
    : on_call( c ), on_ret( r ), on_signal( s ) {}

// Returns true if all function pointers are non-null
bool SSHandlers::is_valid() {
	return ( on_call != nullptr ) && ( on_ret != nullptr ) && ( on_signal != nullptr );
}


/*********************************************************/
/*                                                       */
/*                     Not from header                   */
/*                                                       */
/*********************************************************/


// Calls setup functions.
// The order of these functions matters !
static inline void run_before_everything() {

	// Setup utilities
	Utilities::setup( false );
	Utilities::enable_multi_thread_or_process_mode();

	// General setup
	TerminateOnDestruction tod;
	Utilities::log( "DynamoRIO client started" );
	dr_set_client_name( "ShadowStack DynamoRIO Client 'ShadowStack'",
	                    "http://github.com/zwimer/ShadowStack" );
	Utilities::log( "Client 'DrShadowStack' initializing..." );

	// Call module init functions
	Utilities::assert( drmgr_init(), "drmgr_init() failed." );
	tod.disable();
}

// Called whenever a signal is called. Adds a wildcard to the shadow stack
// Note: the reason we use this instead of the event is this ignores ignored signals
static void kernel_xfer_event_handler( void *, const dr_kernel_xfer_info_t *info ) {
	if ( info->type == DR_XFER_SIGNAL_DELIVERY ) {
		Utilities::verbose_log( "Caught sig ", info->sig, " - ", strsignal( info->sig ),
		                        "\t\n- Handler address = ", (void *) info->target_pc );
		handlers->on_signal();
	}
}

// The function that inserts the call and ret handlers
// Whenever a new basic block is seen, this function will be
// called once for each instruction in it. If either a call
// or a ret is seen, the call and ret handlers are inserted
// before said instruction. Note: an app_pc is defined in comments
static dr_emit_flags_t event_app_instruction( void *drcontext, void *tag, instrlist_t *bb,
                                              instr_t *instr, bool for_trace,
                                              bool translating, void *user_data ) {

	// Concerning DynamoRIO's app_pc type. From their source:
	//   include/dr_defines.h:typedef byte * app_pc;
	//   tools/DRcontrol.c:typedef unsigned char byte;
	// Thus app_pc is simply an unsigned char *

	// If the instruction is a call, get the address,
	// add the size of the call instruction (to get the
	// return address), then insert the on_call function
	// with the return address as a parameter
	if ( instr_is_call( instr ) ) {
		const app_pc xip = instr_get_app_pc( instr ) + instr_length( drcontext, instr );
		dr_insert_clean_call( drcontext, bb, instr, (void *) handlers->on_call, false, 1,
		                      OPND_CREATE_INTPTR( xip ) );
	}

	// If the instruction is a ret, insert the ret handler as an
	// mbr_implementation so as to gain access to the info we need
	if ( instr_is_return( instr ) ) {
		dr_insert_mbr_instrumentation( drcontext, bb, instr, (void *) handlers->on_ret,
		                               SPILL_SLOT_1 );
	}

	// All went well
	return DR_EMIT_DEFAULT;
}

// Called on exit of client program
// Checks how the client returned then exits
static void exit_event() {
	Utilities::assert( drmgr_unregister_bb_insertion_event( event_app_instruction ),
	                   "client process returned improperly." );
	drmgr_exit();
}


// The main client function
// This function dynamically 'injects' the shadow stack
DR_EXPORT void dr_client_main( client_id_t id, int argc, const char *argv[] ) {

	// Setup the client
	run_before_everything();
Utilities::message("START");
	TerminateOnDestruction tod;
	Utilities::assert( argc == 3, "Incorrect usage of dr_client_main\n"
	                              "Expected args: <Mode> <Socket path or empty string>" );

	// Extract the mode
	const SSMode mode( argv[1] );
	Utilities::assert( mode.is_valid_mode, "Invalid mode given to the client" );

	// Call the proper setup function
	if ( mode.is_internal ) {
		InternalSS::setup( &handlers, argv[2] );
	}
	else if ( mode.is_external ) {
		ExternalSS::setup( &handlers, argv[2] );
	}
	else {
		Group::terminate( "Unimplemented mode passed to the client" );
	}

	// Error checking
	Utilities::assert( handlers->is_valid(), "SSHandlers setup incomplete" );

	// Register events
	dr_register_exit_event( exit_event );
	drmgr_register_kernel_xfer_event( kernel_xfer_event_handler );

	// The event used to re-route call and ret's
	drmgr_register_bb_instrumentation_event( NULL, event_app_instruction, NULL );

	// Nothing went wrong, proceed
	tod.disable();
}
