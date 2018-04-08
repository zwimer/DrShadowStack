#include "ss_mode.hpp"
#include "utilities.hpp"

#include "string.h"


// strdup that kills the group on failure
char * safe_strdup(const char * const s) {
	char * const ret = strdup(s);
	Utilities::assert( ret != nullptr, "strdup() failed." );
	return ret;
}

// The constructor
SSMode::SSMode(const char * const m) : str(safe_strdup(m)), 
	is_internal(strcmp(str, INTERNAL_MODE_FLAG) == 0),
	is_protected_internal(strcmp(str, PROT_INTERNAL_MODE_FLAG) == 0),
	is_external(strcmp(str, EXTERNAL_MODE_FLAG) == 0) { }
