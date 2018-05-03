#include <unistd.h>

int main() {
	char * args[] = {"echo", "Hello World", 0};
	execvp(args[0], args);
}
