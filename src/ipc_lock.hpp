#ifndef __IPC_LOCK_HPP__
#define __IPC_LOCK_HPP__

#include <cstdio>


/** An interprocess lock 
 *  This lock is implemented via flockfile
 *  This mutex is re-entrent, however an equal number
 *  of lock and unlock calls must me made to unlock the mutex */
class IPCLock {
public:

	/** Constructor */
	IPCLock();

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
