#include "get_tid.hpp"
#include "utilities.hpp"

#include <sys/syscall.h>
#include <unistd.h>


// Define a gettid function
pid_t get_tid() {
	return syscall(SYS_gettid);
}
