#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void win() {
	system("/bin/sh");
}

int main() {
	char buf[4];
	memcpy(buf+24, "\x86\x05@\x00\x00\x00\x00\x00", sizeof(void*));
	return 0;
}
