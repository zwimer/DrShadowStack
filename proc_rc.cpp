#include "proc_rc.hpp"
#include "utilities.hpp"

#include <sys/mman.h>


// A global process rc
ProgRC prc;

// Initalize the setup variable of ProgRC
bool ProgRC::setup = false;


/*********************************************************/
/*                                                       */
/*                     Helper functions				 	 */
/*                                                       */
/*********************************************************/


// Create a chunk of chared memory of size size
// This memory will be readable, writeable, and anonymous
// On failure, this function terminates the group
void * create_shared_memory(const size_t size) {

	// Our memory buffer will be readable and writable:
	const int protection = PROT_READ | PROT_WRITE;

	// The buffer will be shared (meaning other processes can access it), but
	// anonymous (meaning third-party processes cannot obtain an address for it),
	// so only this process and its children will be able to use it:
	const int visibility = MAP_ANONYMOUS | MAP_SHARED;

	// Allocate the memory and return it
	void * const ret = mmap(nullptr, size, protection, visibility, 0, 0);
	Utilities::assert( ret != MAP_FAILED, "mmap() failed." );
	return ret;
}


// Constructor
ProgRC::ProgRC() : proc_rc((prc_t*) create_shared_memory(sizeof(prc_t))) {
	Utilities::assert( ! setup, "ProgRC constructor called twice." );
	setup = true;
	*proc_rc = 0;
}

// TODO: destroy shmem if it is a thing, on termination

// This function increases the reference count
void ProgRC::inc() {
	TerminateOnDestruction tod;
	rc_lock.lock();
	*proc_rc += 1;
	rc_lock.unlock();
	tod.disable();
}

// This function decreases the reference count
// If the count hits 0, the group is terminated
void ProgRC::dec() {
	TerminateOnDestruction tod;

	// Decrement the rc
	rc_lock.lock();
	*proc_rc -= 1;

	// If the rc is 0, kill everything
	if ( *proc_rc <= 0 ) {
		Group::terminate( "Valid process reference counter"
							" hit 0\nProgram has ended.");
	}

	// Otherwise, release the lock
	rc_lock.unlock();
	tod.disable();
}
