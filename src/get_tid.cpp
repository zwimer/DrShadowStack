#include "get_tid.hpp"
#include "utilities.hpp"

#include <sys/syscall.h>
#include <unistd.h>


// Define a gettid function
pid_t get_tid() {
	pid_t ret = syscall(SYS_gettid);
	Utilities::assert(ret != -1, "syscall(SYS_gettid) failed.");
	return ret;
}
