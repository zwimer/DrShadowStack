# DrShadowStack

DrShadowStack is a software defined dynamic shadow stack implemented via DynamoRIO. DrShadowStack implements a shadow stack for most any binary given to it. If the program attempts to return to a corrupted return address, DrShadowStack will terminate the entire process group (which it sets up).

## Requirements

1. This project utilizes [DynamoRIO](https://github.com/DynamoRIO/dynamorio). Installation instructions may be found [here](https://github.com/DynamoRIO/dynamorio/wiki/How-To-Build).
2. This project utilizes the C++ library [Boost](https://boost.org). This library can be built from source, as explained [here](https://www.boost.org/doc/libs/1_66_0/more/getting_started/unix-variants.html). On [Ubuntu 16.04](http://releases.ubuntu.com/16.04.4/), this library can be installed as follows:
```bash
sudo apt-get update && sudo apt-get install libboost-all-dev
```
3. The project is built on [Ubuntu 16.04](http://releases.ubuntu.com/16.04.4/) via [cmake](https://cmake.org/). It requires compiler that supports `C++11`.

### Developers

1. The documentation is build via [Doxygen](http://www.stack.nl/~dimitri/doxygen/).
2. The changelog is generated via [github\_changelog\_generator](https://github.com/skywinder/github-changelog-generator), which is installed via the command 
```bash
gem install github_changelog_generator
```

## Installation Instructions

1. Install dependencies
2. Clone the repository
```bash
git clone https://github.com/zwimer/DrShadowStack
```
3. Configure the `DrShadowStack/src/CMakeLists.txt` file. Specifically set the path to where DynamoRIO is built.
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

The full usage of this program can be found via: `./DrShadowStack.out --help`

In general, the usage is of this format: `./DrShadowStack.out [--ss_mode <Mode>] <executable target> <target arguments>`

There are two different modes, `int` (internal) and `ext` (external). The internal mode keeps the shadow stack internally in the DynamoRIO client. The external mode stores the stack in a separate process.

## Example

From the build directory, an example could be:
```bash
vagrant@ubuntu-xenial ~/S/s/build> ./DrShadowStack.out --ss_mode int ls -la ./
TID 28866: DynamoRIO client started
total 748
drwxrwxr-x 3 vagrant vagrant   4096 Apr  1 06:43 .
drwxrwxr-x 3 vagrant vagrant   4096 Apr  1 06:51 ..
-rw-rw-r-- 1 vagrant vagrant  14518 Apr  1 03:15 CMakeCache.txt
drwxrwxr-x 7 vagrant vagrant   4096 Apr  1 06:45 CMakeFiles
-rw-rw-r-- 1 vagrant vagrant   1380 Mar 31 00:25 cmake_install.cmake
-rw-rw-r-- 1 vagrant vagrant   8441 Apr  1 06:42 compile_commands.json
-rwxrwxr-x 1 vagrant vagrant 365520 Apr  1 06:43 DrShadowStack.out
-rwxrwxr-x 1 vagrant vagrant 216008 Apr  1 06:42 libss_dr_client.so
-rwxrwxr-x 1 vagrant vagrant 102744 Apr  1 06:42 libss_support.so
-rw-rw-r-- 1 vagrant vagrant    441 Apr  1 06:52 log
-rw-rw-r-- 1 vagrant vagrant  16012 Apr  1 06:42 Makefile
-rw-rw-r-- 1 vagrant vagrant   8157 Apr  1 06:42 ss_dr_client.ldscript
```

## Documentation

Additional documentation to each component of DrShadowStack is build via [Doxygen](http://www.stack.nl/~dimitri/doxygen/), and hosted [here](https://zwimer.com/DrShadowStack).
