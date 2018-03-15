#include "group.h"

#include <signal.h>
// TODO: delete io
#include <stdio.h>
#include <unistd.h>


// Start the group
void setup_group() {
	setsid();
}

// Terminate the group
void terminate_group() {
	killpg(0, 9);
}
