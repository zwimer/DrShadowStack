#include "parse_args.hpp"
#include "utilities.hpp"
#include "constants.hpp"

#include <string.h>
#include <utility>


// For brevity
using namespace boost::program_options;


/*********************************************************/
/*                                                       */
/*	  				Helper functions					 */
/*                                                       */
/*********************************************************/


// Returns a variables map containing the parsed arguments
variables_map parse_args_helper(const int argc, const char * const argv[]) {

	// Usage options (i.e. how the user wants to use the program)
	// For example, do they want help, version info, to load a config file, or normal use?
	options_description usage_options("Generic options");
	usage_options.add_options() 
		("help", "Produce help message")
		/* ("version,v", "produce version information") */ // TODO
		("config,c", value<std::string>(), "Config file specifying command line options"
										   "\nIf this flag is used, configuration will"
										   "\noccur using options in the config file only.");

	// Configuration options
	options_description config_options("Configuration");
	config_options.add_options()
		(DRRUN",d", value<std::string>()->required(), "The drrun executable")
		(MODE",m", value<std::string>()->required(),
					"The mode in which the shadow stack is used"
					"\n\t" INTERNAL_MODE_FLAG " -- internal shadow stack mode"
					"\n\t" EXTERNAL_MODE_FLAG " -- external shadow stack mode" )
		(TARGET, value<std::string>()->required(), "The target executable")
		(TARGET_ARGS, value<std::vector<std::string> >(),
					"The target executable's arguments" );

	// Declare the target and its arguments as positional
	positional_options_description target_options;
	target_options.add("target", 1).add("target-args", -1);

	// Encapsulate arg parsing
	try {

		// Parse usage options
		variables_map args;
		auto unorganized_parsed_args = command_line_parser(argc, argv)
			.options(usage_options)
			.allow_unregistered()
			.run();
		store(unorganized_parsed_args, args);

		// If help was asked for
		if ( args.count("help") ) {
			std::stringstream help;
			help << usage_options << config_options;
			Utilities::message("\n%s", help.str().c_str());
			exit(EXIT_SUCCESS);
		}

		// If version was asked for
		/* else if ( args.count("version") ) { */
		/* 	std::cout << desc << std::endl; */
		/* 	exit(EXIT_SUCCESS); */
		/* } */

		// If a config file is being loaded, 
		/* else if ( args.count("config") ) { */
			
		/* 	// Load the config file's options */
		/* 	const std::string config_file = vm["config"].as<std::string>(); */
		/* 	std::ifstream ifs( config_file.c_str() ); */
		/* 	Utilities::assert( ifs, (config_file + " could not be opened.").c_str() ); */
		/* 	parse_args = std::bind(parse_config_file, std::move(ifs), _1); */
		/* } */

		// Parse configuration options
		unorganized_parsed_args = command_line_parser(argc, argv)
			.options(config_options)
			.positional(target_options)
			.allow_unregistered()
			.run();
		store( unorganized_parsed_args, args );

		// Done parsing, check for errors
		notify(args);

		// If no target options were given, note so
		if ( args.count(TARGET_ARGS) == 0 ) {
			const auto empty = variable_value(std::vector<std::string>(), false);
			args.insert(std::make_pair(TARGET_ARGS, empty));
		}

		// Return the variable map		
		return std::move(args);
	}

	// If an error occured, note so
	catch (...) {
		Utilities::log_error("Incorrect usage");
		Utilities::message("For usage information, use the --help flag");
		exit(EXIT_FAILURE);
	}
}


/*********************************************************/
/*                                                       */
/*	  					 From header					 */
/*                                                       */
/*********************************************************/


// Args constructor
Args::Args(	const std::string & dr, const bool is_int, const std::string & targ, 
			const std::vector<std::string> & targ_args ) : drrun(dr), 
			is_internal(is_int), target(targ), target_args(targ_args) {}


// Returns an args_t containing the parsed arguments
Args parse_args(const int argc, const char * const argv[]) {

	// Parse the arguments
	const auto vm = parse_args_helper(argc, argv);

	// Extract the arguments and return the result
	return std::move( Args( 
		vm[DRRUN].as<std::string>(),
		(strcmp(vm[MODE].as<std::string>().c_str(), INTERNAL_MODE_FLAG) == 0),
		vm[TARGET].as<std::string>(),
		vm[TARGET_ARGS].as<std::vector<std::string> >()
	));
}