cmake_minimum_required(VERSION 3.5)

# Location DynamoRIO
set(DynamoRIO_DIR /home/vagrant/dynamorio/build/cmake)
find_package(DynamoRIO REQUIRED)

# Compile flags
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra")

# Compile the client
add_library(shadow_stack_dr_client SHARED 
	shadow_stack_dr_client.cpp
	../quick_socket.cpp
	../utilities.cpp
	../group.cpp
	)

# Configure DynamoRIO
configure_DynamoRIO_client(shadow_stack_dr_client)
use_DynamoRIO_extension(shadow_stack_dr_client "drmgr")


# For YouCompleteMe
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build the toy to test on
add_executable(toy.out toy.c)

# Compile toy2
set (CMAKE_C_FLAGS "-fno-stack-protector -Wall -Wextra")
add_executable(toy2.out toy2.c)
