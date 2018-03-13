#include "group.h"

#include <unistd.h>


// Start the group
void setup() {
	setsid();
}

// Terminate the group
void terminate() {
	// TODO: write
}
