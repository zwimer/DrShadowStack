cmake_minimum_required(VERSION 3.0)
project(ShadowStack)

# For threads
find_package (Threads)

# TODO: turn off
# Debug mode option, comment out to disable
set(DEBUG_MODE "ON")

# Debug mode
if(DEFINED DEBUG_MODE)
	message("Debug Mode ON!")

	# For YouCompleteMe
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

	# For code coverage tools
	set (CMAKE_CXX_FLAGS "-g -O0 -std=c++11 -Wall -Wextra -Wpedantic")

# Not debug mode
else()
	message("Debug Mode OFF!")

	# Optimizations
	# TODO: add more
	set (CMAKE_CXX_FLAGS "-O2 -std=c++11")

endif()

# Build the shadow stack executable
add_executable(ShadowStack.out
	shadow_stack.cpp
	stack_server.cpp
	quick_socket.cpp
	utilities.cpp
	group.cpp
	delete_me.cpp # TODO
	)

# Link threads
target_link_libraries (ShadowStack.out ${CMAKE_THREAD_LIBS_INIT})
