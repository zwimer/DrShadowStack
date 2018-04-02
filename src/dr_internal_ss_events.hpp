/** @file */
#ifndef __DR_INTERNAL_SS_EVENTS_HPP__
#define __DR_INTERNAL_SS_EVENTS_HPP__

#include "dr_api.h"
#include "drmgr.h"


/** Make a distinction between the internal and external SS functions */
namespace InternalSS {

	/** Setup the internal stack server for the DynamoRIO client */
	void setup(const char * const);
};

#endif
