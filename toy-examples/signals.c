#include <unistd.h>
#include <signal.h>

void s1(int s) {
	write(1, "+1\n", 3);
	kill(getpid(), 3);
	write(1, "-1\n", 3);
}
void s3(int s) {
	write(1, "+3\n", 3);
	write(1, "-3\n", 3);
}

int main() { 

	signal(1, s1);
	signal(3, s3);
	
	kill(getpid(),1);
}
