/** @file */
#ifndef __DR_INTERNAL_SS_EVENTS_HPP__
#define __DR_INTERNAL_SS_EVENTS_HPP__

#include "dr_shadow_stack_client.hpp"


/** Make a distinction between the internal and external SS functions */
namespace InternalSS {

	/** Setup the internal stack server for the DynamoRIO client */
	void setup(SSHandlers ** const handlers, const char * const);
};

#endif
