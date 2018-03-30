cmake_minimum_required(VERSION 3.5)

#################################################
#												#
#					Set by user 				#
#												#
#################################################


# Location of DynamoRIO
set(DynamoRIO_DIR /home/vagrant/dynamorio/build/cmake)


### Developer options below ###

# Debug mode options. Comment out to disable
#set(DEBUG_MODE "VERBOSE")
#set(DEBUG_MODE "ON")

# For YouCompleteMe vim plugin
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


#################################################
#												#
#				Automated - General				#
#												#
#################################################


# Set the program name
set(PROGRAM_NAME DrShadowStack)
project(${PROGRAM_NAME})

# The names of the client shared library for DynamoRio
set(SS_DR_CLIENT_SO ss_dr_client)


# Find packages
find_package(DynamoRIO REQUIRED)

# Require C++ 11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Required complication flags
# TODO:
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wno-unused-parameter") # -Werror" )

# Add macro definitions
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
	add_definitions(-DLOG_FILE=stdout)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb")

# Not debug mode
else()
	message("Debug Mode OFF!")
	add_definitions(-DLOG_FILE=nullptr)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()
message(${CMAKE_CXX_FLAGS})


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
	quick_socket.cpp
	dr_print_sym.cpp
	utilities.cpp
	ipc_lock.cpp
	proc_rc.cpp
	get_tid.cpp
	group.cpp
	)

# Configure DynamoRIO
configure_DynamoRIO_client(${SS_DR_CLIENT_SO})
use_DynamoRIO_extension(${SS_DR_CLIENT_SO} "drmgr")
use_DynamoRIO_extension(${SS_DR_CLIENT_SO} "drsyms")

#################################################
#												#
#			Creating the project .out			#
#												#
#################################################


# Build the shadow stack executable
add_executable(${PROGRAM_NAME}.out
	external_stack_server.cpp
	shadow_stack.cpp
	quick_socket.cpp
	utilities.cpp
	ipc_lock.cpp
	get_tid.cpp
	proc_rc.cpp
	group.cpp
	)
