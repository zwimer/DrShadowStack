#include "message.hpp"


// For clarity
using namespace Message;


// Define the message headers
const char Continue::header[] = "CONT";
const char Call::header[] = 	"CALL";
const char Ret::header[] = 		"RET-";


// Check for errors. Note that an extra character will be allocated
// for the null terminator. This is fine, we just don't send that char
static_assert( sizeof(Continue::header) == MESSAGE_HEADER_LENGTH + 1, 	
				"header is of wrong size" );
static_assert( sizeof(Call::header) == MESSAGE_HEADER_LENGTH + 1, 
				"header is of wrong size" );
static_assert( sizeof(Ret::header) == MESSAGE_HEADER_LENGTH + 1, 
				"header is of wrong size" );


// Continue constructor
Continue::Continue() {
	memcpy(message, header, size);
}

// Call constructor
Call::Call(const char * const ptr) {
	memcpy(message, header, MESSAGE_HEADER_LENGTH);
	memcpy( & message[MESSAGE_HEADER_LENGTH], ptr, POINTER_SIZE);
}

// Ret constructor
Ret::Ret(const char * const ptr) {
	memcpy(message, header, MESSAGE_HEADER_LENGTH);
	memcpy( & message[MESSAGE_HEADER_LENGTH], ptr, POINTER_SIZE);
}

