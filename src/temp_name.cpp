#include "temp_name.hpp"
#include "utilities.hpp"

#include <boost/filesystem.hpp>


// Return a non-existent filename
std::string temp_name() {

	// For brevity
	namespace fs = boost::filesystem;

	// Create the model to be used
	static constexpr const int basename_size = 50;
	static const fs::path tmp_dir = fs::temp_directory_path();
	static const fs::path model( tmp_dir.string() + "/" +
	                             std::string( 50, '%' ).c_str() );

	// Generate the unique path
	try {
		const std::string ret = fs::unique_path( model ).string();
		return std::move( ret );
	}

	// If failure, note so
	catch ( ... ) {
		Utilities::err( "boost::filesystem::unique_path() failed." );
	}
}
