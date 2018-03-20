#include "get_tid.hpp"
#include "utilities.hpp"


// If this is an apple
#ifdef __APPLE__

#include <pthread.h>

// Define a mac-equivalent gettid function
pid_t get_tid() {
	uint64_t tid64;
	ss_assert( pthread_threadid_np(NULL, &tid64) == 0, 
		"pthread_threadid_np() failed.");
	return (pid_t) tid64;
}


// Otherwise, if it is not an apple
#else

#include <sys/syscall.h>
#include <unistd.h>

// Define a gettid function
pid_t get_tid() {
	pid_t ret = syscall(SYS_gettid);
	ss_assert(ret != -1, "syscall(SYS_gettid) failed.");
	return ret;
}

#endif
