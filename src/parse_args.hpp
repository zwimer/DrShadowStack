#ifndef __PARSE_ARGS_HPP__
#define __PARSE_ARGS_HPP__

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

/** The key to the variables map that stores the path to drrun */
#define DRRUN "drrun"

/** The key to the variables map that stores the mode */
#define MODE "mode"


/*********************************************************/
/*                                                       */
/*	  				Expected arguments					 */
/*                                                       */
/*********************************************************/


/** A struct returned by the argument parser */
struct Args {

	/** Constructor */
	Args(	const std::string & dr, const bool is_int, const std::string & targ, 
			const std::vector<std::string> & targ_args );

	/** Path to drrun */
	const std::string drrun;

	/** State if mode is internal */
	const bool is_internal;

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
