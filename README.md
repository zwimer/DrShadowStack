# DrShadowStack

DrShadowStack is a software defined dynamic shadow stack implemented via DynamoRIO. DrShadowStack implements a shadow stack for most any binary given to it. If the program attempts to return to a corrupted return address, DrShadowStack will terminate the entire process group (which it sets up).

## Requirements

This project utilizes [DynamoRIO](https://github.com/DynamoRIO/dynamorio). Installation instructions may be found [here](https://github.com/DynamoRIO/dynamorio/wiki/How-To-Build).

The project is built on Ubuntu 16.04 via [cmake](https://cmake.org/). It requires compiler that supports `c++11`.

### Developers

The documentation is build via [Doxygen](http://www.stack.nl/~dimitri/doxygen/).

The changelog is generated via [github\_changelog\_generator](https://github.com/skywinder/github-changelog-generator), which is installed via the command 
```bash
gem install github_changelog_generator
```

## Installation Instructions

1. Install dependencies
2. Clone the repository
```bash
git clone https://github.com/zwimer/DrShadowStack
```
3. Configure the CMakeLists.txt file (optional)
```bash
vim DrShadowStack/src/CMakeLists.txt
```
4. Create a build directory
```bash
cd DrShadowStack/src
mkdir build && cd build
```
5. Build with CMake and make
```bash
cmake .. && make -j 4
```

## Usage

The usage of this program is: `./DrShadowStack.out <Mode> <drrun> <a.out> <arguments for a.out>`

There are two different modes, `-int` (internal) and `-ext` (external). The internal mode keeps the shadow stack internally in the DynamoRIO client. The external mode stores the stack in a separate process.

## Example

From the build directory, an example could be:
```bash
vagrant@ubuntu-xenial ~/S/s/build> ./DrShadowStack.out -int ~/dynamorio/build/bin64/drrun ls -la
TID 25134: DynamoRIO client started
total 468
drwxrwxr-x 3 vagrant vagrant   4096 Mar 31 00:25 ./
drwxrwxr-x 3 vagrant vagrant   4096 Mar 30 23:34 ../
-rw-rw-r-- 1 vagrant vagrant  11755 Mar 31 00:25 CMakeCache.txt
drwxrwxr-x 7 vagrant vagrant   4096 Mar 31 00:25 CMakeFiles/
-rw-rw-r-- 1 vagrant vagrant   1380 Mar 31 00:25 cmake_install.cmake
-rwxrwxr-x 1 vagrant vagrant 100360 Mar 31 00:25 DrShadowStack.out*
-rwxrwxr-x 1 vagrant vagrant 215888 Mar 31 00:25 libss_dr_client.so*
-rwxrwxr-x 1 vagrant vagrant 102536 Mar 31 00:25 libss_support.so*
-rw-rw-r-- 1 vagrant vagrant  15205 Mar 31 00:25 Makefile
-rw-rw-r-- 1 vagrant vagrant   8157 Mar 31 00:25 ss_dr_client.ldscript
```

## Documentation

Additional documentation to each component of DrShadowStack is build via [Doxygen](http://www.stack.nl/~dimitri/doxygen/), and hosted [here](https://zwimer.com/DrShadowStack).
