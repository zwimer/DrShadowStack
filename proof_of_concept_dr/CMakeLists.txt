cmake_minimum_required(VERSION 3.5)
set(DynamoRIO_DIR /home/vagrant/dynamorio/build/cmake)
find_package(DynamoRIO REQUIRED)
add_library(inscount SHARED inscount.cpp)
configure_DynamoRIO_client(inscount)
use_DynamoRIO_extension(inscount "drmgr")
use_DynamoRIO_extension(inscount "droption")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)




add_executable(toy.out toy.c)
