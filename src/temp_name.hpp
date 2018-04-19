/** @file */
#ifndef __TEMP_NAME_HPP__
#define __TEMP_NAME_HPP__

#include <string>


// This function is in it's own file so that it may be
// used by the external server as well as the main program
// This function utilizes boost filesystem, which violates
// the TLS assumptions of DynamoRIO, thus it cannot be linked
// into the quick_socket file. Hence, it has it's own file


/** Return a non-existent temp-file filename */
std::string temp_name();

#endif
