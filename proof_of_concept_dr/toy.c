#include "toy.h"
#include <stdio.h>
#include <unistd.h>


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



uint_z fib(uint_z n) {
	PNT(n)
	if ( n <= 1 ) return n;
	return fib(n-1) + fib(n-2);
}

uint_z itr(uint_z n)  {
	PNT(n)
	if ( n == 0 ) return 0;
	return itr(n - 1);
}


int main() {

	printf("Starting main\n");

	/* const uint_z c = fib(500); */
	const uint_z c = itr(99);

	printf("Result = %llu\n", c);
	printf("Ending main\n");
	

}
