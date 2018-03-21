/** @file */
#ifndef __GET_TID_HPP__
#define __GET_TID_HPP__

#include <sys/types.h>


/// Define a mac-equivalent gettid function
/** The actual gettid function is not glibc and
 *  may not exist on many machines, so we make one */
pid_t get_tid();


#endif
