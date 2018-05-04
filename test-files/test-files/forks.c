#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define N 1000


// Can be called by pthread, slowly prints a few numbers
void print_numbers(char * id) {
	for ( int i = 0; i < N; ++i ) {
		printf("%s putting: %i\n", (char *) id, i);
		fflush( 0 );
	}
}

// Main function
int main() {

	// Fork
	const int rv = fork();
	if ( rv == -1 ) {
		perror("fork failed().");
		exit(EXIT_FAILURE);
	}

	// Print stuff while it runs
	print_numbers( (rv == 0) ? "Child" : "Parent" );
	return EXIT_SUCCESS;
}
