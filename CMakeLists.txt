cmake_minimum_required(VERSION 3.0)
project(ShadowStack)

# TODO: turn off
# Debug mode option, comment out to disable
set(DEBUG_MODE "ON")

# Debug mode
if(DEFINED DEBUG_MODE)
	message("Debug Mode ON!")

	# For YouCompleteMe
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

	# For code coverage tools
	set (CMAKE_C_FLAGS "-g -O0")
	set (CMAKE_CXX_FLAGS "-g -O0")

# Not debug mode
else()
	message("Debug Mode OFF!")

	# Optimizations
	# TODO: add more
	set (CMAKE_C_FLAGS "-O2")
	set (CMAKE_CXX_FLAGS "-O2")

endif()

# Build the executable
add_executable(ShadowStack.out
	run_shadow_stack.cpp
	stack.cpp
	group.c
	utilities.c
	quick_socket.c )
