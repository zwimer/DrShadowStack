#include <unistd.h>

int main(int argc, char * argv[]) {
	char * args[] = {"/usr/bin/stat", argv[0], 0};
	execvp(args[0], args);
}
