/** @file */
#ifndef __PROC_RC_HPP__
#define __PROC_RC_HPP__

#include "ipc_lock.hpp"
#include "group.hpp"


/** The type the process rc is */
typedef long prc_t;


// Note: dynamorio does not play well with pthreads
// thus this file exists to extract out the functionality
// of the mutex from group.cpp, doing so allows group.cpp

/// A class that wraps a process reference counter
/** This class has a singleton constructor, is initalized at start up */
class ProcRC {
private:

	/** Delete non-default constructors */
	ProcRC( ProcRC && ) = delete;

	/** Delete non-default constructors */
	ProcRC( const ProcRC & ) = delete;

	/** Delete non-default constructors */
	ProcRC & operator=( const ProcRC & ) = delete;

	/** The process rc pointer */
	prc_t * proc_rc;

	/** The mutex used to protect the reference coutner */
	IPCLock rc_lock;

	/** Set to true by the singleton constructor once created
	 *  Default value: false */
	static bool setup;

public:

	/** Initalizes process reference counter to 0
	 *  Also registers a method to call delete prc with Group */
	ProcRC();

	/// A destructor
	/** Frees memory allocated */
	~ProcRC();

	/** This function increments the process reference count */
	void inc();

	/// This function decreases the process reference count
	/** If the count hits 0, the group is terminated */
	void dec();
};


// A global process rc
extern ProcRC * prc;


#endif
