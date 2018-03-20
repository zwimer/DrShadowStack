/** @file */
#ifndef __PROC_RC_HPP__
#define __PROC_RC_HPP__

#include "group.hpp"

#include <boost/interprocess/sync/interprocess_mutex.hpp>


// The type the process rc is
typedef long prc_t;


// Note: dynamorio does not play well with pthreads
// thus this file exists to extract out the functionality
// of the mutex from group.cpp, doing so allows group.cpp

/// A class that wraps a process reference counter
/** This class is initalized at startup, so no 
 *  'setup' function has to be called */
class ProgRC {
private:

	// Delete non-default constructors
	ProgRC( ProgRC && ) = delete;
	ProgRC( const ProgRC & ) = delete;
	ProgRC & operator=( const ProgRC & ) = delete;

	// A bool used to determine of a ProgRC exists
	static bool setup;

	// The process rc pointer and the lock that protects it
	boost::interprocess::interprocess_mutex rc_lock;
	prc_t * const proc_rc;
	
public:

	/// A singleton constructor
	/** Initalizes process reference counter to 0 */
	ProgRC();

	/** This function increments the process reference count
	 *  Note: This function merely wraps the decrement
	 * rc function from proc_rc with a tod */
	void inc();

	/// This function decreases the process reference count
	/** If the count hits 0, the group is terminated
	 *  Note: This function merely wraps the decrement
	 * rc function from proc_rc with a tod */
	void dec();
};

// A global process rc
ProgRC prc;


#endif
