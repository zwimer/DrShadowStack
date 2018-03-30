#include "dr_print_sym.hpp"

#include "drsyms.h" // TODO call drsyms init
#include "drmgr.h"

#include <memory>

// The maximum length a symbol may be
#define MAX_SYM_RESULT 4096


// Print symbol information for the what is located at addr
// Prints out information via the pnt function which works like printf
// Prints out errors via the perr function which works like printf
// description is a description of what the address addr points to
void print_sym( PrintFn pnt, PrintFn perr, const char * const description, 
				const app_pc addr ) {
 
	// Print out the description
	pnt("Printing symbol information for %s...", description);

	// Temporaries used for struct internals
    char name[MAX_SYM_RESULT];
    char file[MAXIMUM_PATH];

	// Check to see what module the symbol is in
	// If it is not in any, note so then return
    module_data_t *data = dr_lookup_module(addr);
    if (data == nullptr) {
        perr("Unknown symbol for %p", addr);
        return;
    }

	// Create a smart pointer to store a module_data_t
	std::unique_ptr<module_data_t, void (*) (module_data_t *)>
		data_encapsulated( data, dr_free_module_data );

	// Create a drsym_info_t
    drsym_info_t sym;
	sym.struct_size = sizeof(sym);
	sym.name_size = MAX_SYM_RESULT;
	sym.file_size = MAXIMUM_PATH;
	sym.file = file;
	sym.name = name;

	// Look up the symbol
	drsym_error_t symres = drsym_lookup_address( data->full_path, addr - data->start, 
												 &sym, DRSYM_DEFAULT_FLAGS );

	// If somethign went wrong other than line not available
    if ((symres != DRSYM_SUCCESS) && (symres != DRSYM_ERROR_LINE_NOT_AVAILABLE)) {
        perr("drsym_lookup_address() failed for %p", addr);
		return;
	}

	// Determine the name of the module
	const char * modname = dr_module_preferred_name(data);
	if (modname == nullptr) {
		modname = "<noname>";
	}

	// Print the basic symbol info	
	pnt( "Address: %p\n\t- Module: %s\n\t- Symbol: %s + %p",
		 addr, modname, sym.name, addr - data->start - sym.start_offs);

	// If possible, print line specific information
	if (symres == DRSYM_ERROR_LINE_NOT_AVAILABLE) {
		pnt( "\t- File: %s\n\t- Line: %llu + %p\n",
			 sym.file, sym.line, sym.line_offs );
	}

	// Otherwise, note that no info is available
	else {
		pnt("\t- No line specific information available.\n");
	}
}
