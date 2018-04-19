#include "message.hpp"

#include "string.h"


// Copies n bytes from src into dst during static initilization
// If src is less than n bytes, fills the rest of dst with zeros.
// It will return dst so that dst can be assigned to a static initalization
const char *Message::Msg::set_length( char *const dst, const char *const src, const int n ) {
	return strncpy( (char *) memset( (void *) dst, 0, n ), src, n );
}
