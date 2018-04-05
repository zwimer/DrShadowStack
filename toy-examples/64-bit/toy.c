// gcc toy.c -O0 -o toy.out
#include <stdio.h>
#include <unistd.h>

typedef unsigned long long uint_z;

// A macro used to print out a number
// This is a macro to avoid calling and functions
// except for the system call write
#define PNT(n) {													\
		int len = 16;												\
		char arr[len];												\
		int indx = len - 1;											\
		arr[indx--] = '\n';											\
		arr[indx--] = ')';											\
																	\
		int val = n;												\
		while ( val ) {												\
			arr[indx--] = "0123456789"[val % 10];					\
			val /= 10;												\
		}															\
		if (n == 0) arr[indx--] = '0';								\
		arr[indx--] = '(';											\
		arr[indx--] = 'r';											\
		arr[indx--] = 't';											\
		arr[indx--] = 'I';											\
		write(1, arr + indx + 1, len - indx - 1);					\
	}


// Just a functio nthat calls itself -1
// Also prints out n
uint_z itr(uint_z n)  {
	PNT(n)
	if ( n == 0 ) return 0;
	return itr(n - 1);
}

// Just calls itr(99)
int main() {
	itr(99);
}
