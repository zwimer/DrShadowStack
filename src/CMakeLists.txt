cmake_minimum_required(VERSION 3.5)

#################################################
#												#
#					Set by user 				#
#												#
#################################################


# Location DynamoRIO
set(DynamoRIO_DIR /home/vagrant/dynamorio/build/cmake)


### Developer options below ###

# Debug mode option, comment out to disable
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

# The name of the internal internal client .so to be created
set(SS_DR_CLIENT_SO ss_dr_client)

# The names of the internal and external
# client shared libraries to be created
set(SS_DR_CLIENT_INTERNAL_SO ss_dr_client_int)
set(SS_DR_CLIENT_EXTERNAL_SO ss_dr_client_ext)

# The name of the library to be shared by all created targets
set(SS_SHARED_LIB ss_general_shared_lib)

# Find packages
find_package (Threads REQUIRED)
find_package(DynamoRIO REQUIRED)

# Require C++ 11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Required complication flags
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wno-unused-parameter -Werror" )

# Add macro definitions
add_definitions(-DPROGRAM_NAME="${PROGRAM_NAME}")
add_definitions(-DDYNAMORIO_CLIENT_SO="${CMAKE_BINARY_DIR}/lib${SS_DR_CLIENT_EXTERNAL_SO}.so")


# If debug mode
if(DEFINED DEBUG_MODE)
	message("Debug Mode ON!")
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


# A common library shared by both everything
add_library(${SS_SHARED_LIB} SHARED 
	quick_socket.cpp
	get_tid.cpp
	group.cpp
	)

# Create the internal .so
add_library(${SS_DR_CLIENT_INTERNAL_SO} SHARED 
	dr_shadow_stack_internal_client.cpp
	)

# Create the external .so
add_library(${SS_DR_CLIENT_EXTERNAL_SO} SHARED 
	dr_shadow_stack_external_client.cpp
	utilities.cpp # TODO : make not needed to compile twice
	)

# Link the libraries to the common library
target_link_libraries(${SS_DR_CLIENT_EXTERNAL_SO} ${SS_SHARED_LIB})
target_link_libraries(${SS_DR_CLIENT_INTERNAL_SO} ${SS_SHARED_LIB})

# Configure DynamoRIO for both
configure_DynamoRIO_client(${SS_DR_CLIENT_INTERNAL_SO})
configure_DynamoRIO_client(${SS_DR_CLIENT_EXTERNAL_SO})
use_DynamoRIO_extension(${SS_DR_CLIENT_INTERNAL_SO} "drmgr")
use_DynamoRIO_extension(${SS_DR_CLIENT_EXTERNAL_SO} "drmgr")


#################################################
#												#
#			Creating the project .out			#
#												#
#################################################


# Build the shadow stack executable
add_executable(${PROGRAM_NAME}.out
	shadow_stack.cpp
	stack_server.cpp
	utilities.cpp # TODO : make not needed to compile twice
	proc_rc.cpp
	)

# Link the executable to the thread package
target_link_libraries (${PROGRAM_NAME}.out ${CMAKE_THREAD_LIBS_INIT} ${SS_SHARED_LIB})
