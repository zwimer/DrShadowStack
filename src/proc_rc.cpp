#include "proc_rc.hpp"
#include "utilities.hpp"

#include <sys/mman.h>


// A global process rc
ProcRC * prc = new ProcRC();

// Initalize the setup variable of ProcRC
bool ProcRC::setup = false;


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

// Delete's prc
static void del_prc() {
	delete prc;
}

/*********************************************************/
/*                                                       */
/*                     From header file					 */
/*                                                       */
/*********************************************************/


// Constructor
ProcRC::ProcRC() : proc_rc((prc_t*) create_shared_memory(sizeof(prc_t))), 
					rc_lock(Utilities::assert)  {
	Utilities::assert( ! setup, "ProcRC constructor called twice." );
	Group::register_delete_proc_rc( del_prc );
	setup = true;
	*proc_rc = 0;
}

// Destructor
ProcRC::~ProcRC() {	
	Utilities::log("ProcRC destructor called");
	if ( prc->proc_rc != nullptr ) {

		// To prevent an infinite loop in case of failure
		prc_t * const rc = prc->proc_rc;
		prc->proc_rc = nullptr;
		prc = nullptr;

		// Remove the shared memory
		Utilities::assert( munmap( rc, sizeof(prc_t)) == 0, "munmap() failed." );
	}
}

// This function increases the reference count
void ProcRC::inc() {
	TerminateOnDestruction tod;
	rc_lock.lock();
	*proc_rc += 1;
	Utilities::log("Process RC incremented. Now: ", *proc_rc);
	rc_lock.unlock();
	tod.disable();
}

// This function decreases the reference count
// If the count hits 0, the group is terminated
void ProcRC::dec() {
	TerminateOnDestruction tod;

	// Decrement the rc
	rc_lock.lock();
	*proc_rc -= 1;
	Utilities::log("Process RC decremented. Now: ", *proc_rc);

	// If the rc is 0, release the lock then kill everything
	if ( *proc_rc <= 0 ) {
		rc_lock.unlock();
		Group::terminate( "Valid process reference counter"
							" hit 0\nProcram has ended.",
							false );
	}

	// Otherwise, release the lock
	rc_lock.unlock();
	tod.disable();
}
