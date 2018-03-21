/** @file */
#ifndef __STACK_SERVER_HPP__
#define __STACK_SERVER_HPP__


/// The function for running the shadow stack
/** Sock must be the file descriptor to the unix domain
 *  server that connects the shadow stack program to the
 *  dynamorio client managing the program to be run */
void start_shadow_stack(const int sock);


#endif
