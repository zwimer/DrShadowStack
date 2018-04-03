cmake_minimum_required(VERSION 3.5)

#################################################
#												#
#					Set by user 				#
#												#
#################################################


# Location of DynamoRIO and drrun
set(DynamoRIO_DIR /home/vagrant/dynamorio/build/cmake)
set(DRRUN_PATH /home/vagrant/dynamorio/build/bin64/drrun)

# Default mode: int for internal, ext for external
set(DEFAULT_MODE int)


### Developer options below ###


# Debug mode options. Comment out to disable
#set(DEBUG_MODE "VERBOSE")
#set(DEBUG_MODE "ON")

# For YouCompleteMe vim plugin
# Disable for non-developers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


#################################################
#												#
#				Automated - General				#
#												#
#################################################


# Set the program name
set(PROGRAM_NAME DrShadowStack)
project(${PROGRAM_NAME})

# Set the program version
set(PROGRAM_VERSION 0.1)


# The name of the client shared library for DynamoRio
set(SS_DR_CLIENT_SO ss_dr_client)

# The name of the support library
set(SS_SUPPORT_LIB ss_support)


# Find packages
find_package(DynamoRIO REQUIRED)
find_package(Boost 1.47.0 COMPONENTS program_options filesystem REQUIRED)

# Require C++ 11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Required complication flags
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wno-unused-parameter -Werror" )

# Add macro definitions
add_definitions(-DDRRUN_PATH="${DRRUN_PATH}")
add_definitions(-DVERSION="${PROGRAM_VERSION}")
add_definitions(-DDEFAULT_MODE="${DEFAULT_MODE}")
add_definitions(-DPROGRAM_NAME="${PROGRAM_NAME}")
add_definitions(-DDYNAMORIO_CLIENT_SO="${CMAKE_BINARY_DIR}/lib${SS_DR_CLIENT_SO}.so")


# If debug mode
if(DEFINED DEBUG_MODE)
	message("Debug Mode ON!")

	# If verbose
	if(DEBUG_MODE STREQUAL "VERBOSE")
		message("\t Verbose Debug Mode ON!")
		add_definitions(-DVERBOSE)
	endif()

	set(CMAKE_BUILD_TYPE debug)
	add_definitions(-DDEBUG_MODE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb")

# Not debug mode
else()
	message("Debug Mode OFF!")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()
message(${CMAKE_CXX_FLAGS})


#################################################
#												#
#			Creating the support lib			#
#												#
#################################################


# Create the .so
add_library(${SS_SUPPORT_LIB} SHARED 
	quick_socket.cpp
	utilities.cpp
	get_tid.cpp
	group.cpp
	)


#################################################
#												#
#			Creating the client .so's			#
#												#
#################################################


# Create the .so
add_library(${SS_DR_CLIENT_SO} SHARED 
	dr_shadow_stack_client.cpp
	dr_internal_ss_events.cpp
	dr_external_ss_events.cpp
	dr_print_sym.cpp
	)

# Configure DynamoRIO
configure_DynamoRIO_client(${SS_DR_CLIENT_SO})
use_DynamoRIO_extension(${SS_DR_CLIENT_SO} "drmgr")
use_DynamoRIO_extension(${SS_DR_CLIENT_SO} "drsyms")

# Link to the support library
target_link_libraries(${SS_DR_CLIENT_SO} ${SS_SUPPORT_LIB})


#################################################
#												#
#			Creating the project .out			#
#												#
#################################################


# Build the shadow stack executable
add_executable(${PROGRAM_NAME}
	external_stack_server.cpp
	shadow_stack.cpp
	parse_args.cpp
	temp_name.cpp
	)

# Link to the support library
target_link_libraries(${PROGRAM_NAME} ${SS_SUPPORT_LIB} Boost::program_options Boost::filesystem)
