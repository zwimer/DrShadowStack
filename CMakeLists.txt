cmake_minimum_required(VERSION 3.0)
project(ShadowStack)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# The gcda's are empty...
# set (CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
# set (CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")

add_executable(ShadowStack.out
	run_shadow_stack.cpp
	stack.cpp
	group.c
	utilities.c
	quick_socket.c )
