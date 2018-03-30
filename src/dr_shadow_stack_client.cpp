/** @file */
#include"dr_internal_ss_events.hpp"
#include"dr_external_ss_events.hpp"
#include "constants.hpp"
#include "utilities.hpp"

#include "drmgr.h"

/// The main client function
/** This function dynamically 'injects' the shadow stack */
DR_EXPORT void dr_client_main(client_id_t id, int argc, const char *argv[]) {	

	// TODO: use arg parser
	Utilities::message("DynamoRIO client started");
	Utilities::assert(argc == 3, "Incorrect usage of dr_client_main\n"
								 "Expected args: <Mode> <Socket path or empty string>" );

	// Setup the client
    dr_set_client_name( "ShadowStack DynamoRIO Client 'ShadowStack'", 
						"http://github.com/zwimer/ShadowStack");
    drmgr_init();

    // Make it easy to tell, by looking at log file, which client executed
    Utilities::log("Client 'DrShadowStack' initializing\n");

	// If the decide what functions to use based on the mode
	const bool is_internal = ( strcmp(argv[1], INTERNAL_MODE_FLAG) == 0 );
	const auto event_fn = is_internal ? 
		InternalSS::event_app_instruction : ExternalSS::event_app_instruction;
	const auto exit_event = is_internal ? 
		InternalSS::exit_event : ExternalSS::exit_event;
	const auto setup = is_internal ? InternalSS::setup : ExternalSS::setup;

	// Register events
    drmgr_register_bb_instrumentation_event(NULL, event_fn, NULL);
	dr_register_exit_event(exit_event);

	// Setup the SS's client side part
	setup( argv[2] );
}
