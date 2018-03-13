#ifndef __STACK_HPP__
#define __STACK_HPP__


// Define IS_CPP for utilities.h
#ifndef IS_CPP
#define IS_CPP
#endif


// The function for running the shadow stack
// Sock must be the file descriptor to a sever connected to the client
void start_shadow_stack(const int sock);


#endif
