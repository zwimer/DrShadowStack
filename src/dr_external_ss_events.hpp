/** @file */
#ifndef __DR_EXTERNAL_SS_EVENTS_HPP__
#define __DR_EXTERNAL_SS_EVENTS_HPP__

#include "dr_shadow_stack_client.hpp"


/** Make a distinction between the internal and external SS functions */
namespace ExternalSS {

	/** Setup the external stack server for the DynamoRIO client */
	void setup( SSHandlers **const handlers, const char *const );
}; // namespace ExternalSS


#endif
