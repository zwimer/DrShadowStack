#include "dr_print_sym.hpp"
#include "utilities.hpp"

#include "dr_api.h"
#include "drsyms.h"
#include "drmgr.h"

#include <sstream>
#include <memory>

// The maximum length a symbol may be
#define MAX_SYM_RESULT 4096


// Initalize statics
bool Sym::setup = false;


// Setup drsym
void Sym::init() {
	Utilities::assert( ! setup, "Sym::setup() called twice!" );
    Utilities::assert( drsym_init(0) == DRSYM_SUCCESS, "drsym_init() failed." );
	setup = true;
}

// Cleanup drsym
void Sym::finish() {
	Utilities::assert( setup, "Sym::cleanup() called before setup!" );
    Utilities::assert( drsym_exit() == DRSYM_SUCCESS, "drsym_exit() failed." );
}

// Print symbol information for the what is located at addr
//  description is a description of what the address addr points to
void Sym::print( const char * const description, const app_pc addr ) {
 
	// Print out the description
	Utilities::message("Printing symbol information for ", description, "...");

	// Temporaries used for struct internals
    char name[MAX_SYM_RESULT + 1];
    char file[MAXIMUM_PATH + 1];

	// Check to see what module the symbol is in
	// If it is not in any, note so then return
    module_data_t * data = dr_lookup_module(addr);
    if (data == nullptr) {
        Utilities::log_error("Unknown symbol for ", (void *) addr);
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

	// Switch on symres
	switch(symres) {

		// If an unknown error happened, note so
		default :
			Utilities::log_error(	"drsym_lookup_address() failed for ", 
									(void *) addr, "with error number: ", symres );
			return;

		// If the symbol was not found, note so
		case DRSYM_ERROR_SYMBOL_NOT_FOUND : 
			Utilities::log_error(	"drsym_lookup_address() failed for ", 
									(void *) addr," - symbol not found" );
			return;

		// If the sym is known, continue on
		case DRSYM_SUCCESS :
		case DRSYM_ERROR_LINE_NOT_AVAILABLE : 
		/* No fall through, so we need a semioclon */ ;
	}

	// Determine the name of the module
	const char * modname = dr_module_preferred_name(data);
	if (modname == nullptr) {
		modname = "<noname>";
	}

	// Get the basic symbol info	
	std::stringstream symbol_info;	
	symbol_info	<< "Address: " << (void *) addr << "\n\t- Module: "
				<< modname << "\n\t- Symbol: " << sym.name << " + "
				<< addr - data->start - sym.start_offs;

	// If possible, print line specific information
	if (symres == DRSYM_ERROR_LINE_NOT_AVAILABLE) {
		symbol_info	<< "\n\t- File: " << sym.file << "\n\t- Line: "
					<< sym.line << " + " << sym.line_offs;
	}

	// Otherwise, note that no info is available
	else {
		symbol_info << "\n\t- No line specific information available.\n";
	}

	// Print the info
	Utilities::message( symbol_info.str() );
}
