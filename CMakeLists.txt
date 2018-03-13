cmake_minimum_required(VERSION 3.0)
project(ShadowStack)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(ShadowStack.out
	run_shadow_stack.cpp
	stack.cpp
	group.c
	utilities.c
	quick_socket.c )
