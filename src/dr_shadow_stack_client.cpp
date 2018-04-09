/** @file */
#include "dr_internal_ss_events.hpp"
#include "dr_external_ss_events.hpp"
#include "constants.hpp"
#include "utilities.hpp"
#include "ss_mode.hpp"
#include "group.hpp"

#include "dr_api.h"
#include "drmgr.h"


/** The main client function
 *  This function dynamically 'injects' the shadow stack */
DR_EXPORT void dr_client_main(client_id_t id, int argc, const char *argv[]) {	

	// Setup utilities
	Utilities::setup(false);
	Utilities::enable_multi_thread_or_process_mode();

	// Alert the user that DynamoRIO has started
	Utilities::log("DynamoRIO client started");
	Utilities::assert(argc == 3, "Incorrect usage of dr_client_main\n"
								 "Expected args: <Mode> <Socket path or empty string>" );

	// Setup the client
    dr_set_client_name( "ShadowStack DynamoRIO Client 'ShadowStack'", 
						"http://github.com/zwimer/ShadowStack");

    // Make it easy to tell, by looking at log file, which client executed
    Utilities::log("Client 'DrShadowStack' initializing\n");

	// Extract the mode
	const SSMode mode(argv[1]);
	Utilities::assert( mode.is_valid_mode, "Invalid mode given to the client" );

	// Call the proper setup function
	if ( mode.is_internal ) {
		InternalSS::setup( argv[2] );
	}
	else if ( mode.is_external ) {
		ExternalSS::setup( argv[2] );
	}
	else { 
		Group::terminate("Unimplemented mode passed to the client");
	}
}
