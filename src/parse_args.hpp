/** @file */
#ifndef __PARSE_ARGS_HPP__
#define __PARSE_ARGS_HPP__

#include "ss_mode.hpp"

#include <boost/program_options.hpp>


/*********************************************************/
/*                                                       */
/*	  				   Argument keys					 */
/*                                                       */
/*********************************************************/


/** The key to the variables map that stores the target arguments */
#define TARGET_ARGS "target-args"

/** The key to the variables map that stores the target */
#define TARGET "target"

/** The key to the variables map that stores the mode */
#define MODE "ss_mode"


/*********************************************************/
/*                                                       */
/*	  				Expected arguments					 */
/*                                                       */
/*********************************************************/


/** A struct returned by the argument parser */
struct Args {

	/** Constructor */
	Args(	SSMode && mode_, const std::string & targ, 
			std::vector<std::string> & targ_args );

	/** The shadow stack mode */
	const SSMode mode;

	/** Path to target executable */
	const std::string target;

	/** Targe executable arguments */
	const std::vector<std::string> target_args;
};


/*********************************************************/
/*                                                       */
/*	  				Function declarations				 */
/*                                                       */
/*********************************************************/


/** Returns an args_t containing the parsed arguments */
Args parse_args(const int argc, const char * const argv[]);


#endif
