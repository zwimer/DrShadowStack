/** @file */
#ifndef __EXTERNAL_STACK_SERVER_HPP__
#define __EXTERNAL_STACK_SERVER_HPP__


/** The function for running the external shadow stack sever
 *  Sock must be the file descriptor to the unix domain
 *  server that connects the shadow stack program to the
 *  dynamorio client managing the program to be run */
void start_external_shadow_stack(const int sock);


#endif
