#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define N 1000


// Can be called by pthread, slowly prints a few numbers
void * print_numbers(void * id) {
	for ( int i = 0; i < N; ++i ) {
		printf("%s putting: %i\n", (char *) id, i);
		fflush( 0 );
	}
    return NULL;
}

// Main function
int main() {

	// Start a thread
    pthread_t thread_id;
    pthread_create( & thread_id, NULL, print_numbers, "Thread");

	// Print stuff while it runs
	(void) print_numbers("Main");

	// Have the function join
    pthread_join(thread_id, NULL);
	return EXIT_SUCCESS;
}
