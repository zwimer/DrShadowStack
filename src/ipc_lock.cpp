#include "ipc_lock.hpp"


// Constructor
IPCLock::IPCLock( AssertFn assert ) : lock_file(std::tmpfile()) {
	assert( lock_file != nullptr, "std::tmpfile() failed" );
}

// Destructor
IPCLock::~IPCLock() {
	fclose(lock_file);
}

// The lock function
void IPCLock::lock() {
	flockfile(lock_file);
}

// The unlock function
void IPCLock::unlock() {
	funlockfile(lock_file);
}
