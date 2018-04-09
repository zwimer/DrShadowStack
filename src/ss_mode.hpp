/** @file */
#ifndef __SS_MODE_HPP__
#define __SS_MODE_HPP__


/** The flag that must be passed to invoke internal mode */
#define INTERNAL_MODE_FLAG "int"

/** The flag that must be passed to invoke protected internal mode */
#define PROT_INTERNAL_MODE_FLAG "prot_int"

/** The flag that must be passed to invoke external mode */
#define EXTERNAL_MODE_FLAG "ext"


/** A tiny struct that represents a shadow stack mode */
struct SSMode final {

	/** The constructor 
	 *  Reads the mode in from m and stores a copy of it */
	SSMode(const char * const m);

	/** Disable the default constructor */
	SSMode() = delete;


	/** The mode */
	const char * const str;

	/** True if mode = internal */
	const bool is_internal;

	/** True if mode = protected internal */
	const bool is_protected_internal;

	/** True if mode = external */
	const bool is_external;

	/** True if any mode is valid */
	const bool is_valid_mode;
};


/*********************************************************/
/*                                                       */
/*					  Static assertions					 */
/*                                                       */
/*********************************************************/


/** A compile time strcmp
 *  Recursively checks if the first characters of the strings match */
static constexpr bool str_equal( const char * const s1, 	
								 const char * const s2 ) {
	return (s1[0] != s2[0]) ? false :
		(s1[0] == 0) ? true : str_equal(s1 + 1, s2 + 1);
}


/** Verify the mode flags are not equal */
static_assert( ! str_equal(INTERNAL_MODE_FLAG, EXTERNAL_MODE_FLAG),
				"internal mode flag cannot equal external mode flag");
static_assert( ! str_equal(INTERNAL_MODE_FLAG, PROT_INTERNAL_MODE_FLAG),
				"internal mode flag cannot equal protected internal mode flag");
static_assert( ! str_equal(PROT_INTERNAL_MODE_FLAG, EXTERNAL_MODE_FLAG),
				"protected iternal mode flag cannot equal external mode flag");


#endif
