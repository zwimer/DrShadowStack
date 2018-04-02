/** @file */
#ifndef __DR_EXTERNAL_SS_EVENTS_HPP__
#define __DR_EXTERNAL_SS_EVENTS_HPP__

#include "dr_api.h"


/** Make a distinction between the internal and external SS functions */
namespace ExternalSS {

	/** Setup the external stack server for the DynamoRIO client */
	void setup(const char * const socket_path);
};

#endif
