#ifndef __IPC_LOCK_HPP__
#define __IPC_LOCK_HPP__

#include <cstdio>

/** A useful typedef for dependency injection */
using AssertFn = void (* const) (bool b, const char * const msg);

/** An interprocess lock 
 *  This lock is implemented via flockfile
 *  This mutex is re-entrent, however an equal number
 *  of lock and unlock calls must me made to unlock the mutex */
class IPCLock {
public:

	/** Constructor 
	 *  Dependency inject the assert function */
	IPCLock( AssertFn assert );

	/** Destructor */
	~IPCLock();

	/** Lock the mutex */
	void lock();

	/** Unlock the mutex */
	void unlock();

private:

	/** The file the lock will act on */
	FILE * const lock_file;
};

#endif
