#include "delete_me.hpp"
#include "utilities.hpp"
#include "constants.hpp"

#include <sys/socket.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

void snd(std::string s, int set_s = -1) {
	static int sock = -1; if ( set_s != -1 ) { sock = set_s; return; }
	if ( s.size() != 12 ) {
		std::cout << "Got s = " << s.size() << " bytes" << std::endl;
		std::cout << "s = " << s << std::endl;
		ss_assert(false, "wrong size");
	}
	ss_assert( write(sock, s.c_str(), s.size()), "write() failed!");
}

void snd2(std::string pre, std::string s) {
	while (s.size() != 8) {
		s = std::string("0", 1) + s;
	}
	snd(pre + s);
}

void call(std::string s) {
	snd2("CALL", s);
}

void ret(std::string s) {
	snd2("RET-", s);
}

void delete_me(const int sock) {
	snd("", sock);

	for(int i = 0; i < 5; ++i) {
		call( std::string(1, std::string("123456789")[i]) );
	}

	const int num_bytes = sizeof(CONTINUE);
	char buffer[num_bytes];
	for(int i = 4; i >= 0; --i) {
		ret( std::string(1, std::string("123456789")[i]) );
		const int bytes_recv = recv( sock, buffer, num_bytes, MSG_WAITALL );
		ss_assert( bytes_recv == num_bytes, "recv() failed" );
		ss_assert( memcmp(buffer, CONTINUE, num_bytes) == 0, "CONT is wrong!");
		std::cout << "CONT" << std::endl;
	}

	// ret("Fail");

	/* while(1) { */
	/* 	std::cout << "Waiting..." << std::endl; */
	/* 	std::string line; */
	/* 	std::getline(std::cin, line); */
	/* 	std::cout << "SENDING: " << line << "\n\tBytes = " << line.size() << std::endl; */
	/* } */
	fprintf(stderr, "Process Send death\n");
}
