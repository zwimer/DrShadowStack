// gcc hacked_toy.c -O0 -o hacked_toy.out
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


// Call for a shell
void win() {
	system("/bin/sh");
}

// Overwrite main's return address
int main() {
	char buf[4];
	memcpy(buf+24, "\xf6\x05@\x00\x00\x00\x00\x00", sizeof(void*));
	return 0;
}
