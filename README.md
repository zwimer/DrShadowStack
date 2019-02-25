# DrShadowStack

[![Build Status](https://travis-ci.org/zwimer/DrShadowStack.svg?branch=master)](https://travis-ci.org/zwimer/DrShadowStack)

DrShadowStack is a software defined dynamic shadow stack implemented via DynamoRIO. DrShadowStack implements a shadow stack any binary given to it, provided the file has an ELF header. If the program attempts to return to a corrupted return address, DrShadowStack will terminate the entire process group (which it sets up). DrShadowStack can handle multi-threaded processes, processes that fork, processes that call any variation of exec. All of these processes will also be protected by DrShadowStack.

# Table of Contents

1. [Docker](#docker)
1. [Requirements](#requirements)
1. [Installation Instructions](#installation-instructions)
1. [Usage](#usage)
1. [Example](#example)
1. [Documentation](#documentation)
1. [Developers](#developers)

## Docker
A `Dockerfile` is provided with a pre-installed `DrShadowStack` binary. A docker image is also provided. It is hosted [here](https://cloud.docker.com/u/zwimer/repository/docker/zwimer/drshadowstack) on [https://hub.docker.com/](https://hub.docker.com/). To pull the docker image simply execute:
```bash
docker pull zwimer/drshadowstack
```
To run the container  simply execute:
```bash
docker run --rm -it zwimer/drshadowstack
```
If you would like to build the container yourself execute:
```bash
git clone https://github.com/zwimer/DrShadowStack && \
cd DrShadowStack && \
docker build -t zwimer/DrShadowStack .
```

## Requirements

1. This project utilizes [DynamoRIO](https://github.com/DynamoRIO/dynamorio) version `7.0.17636`. This release can be found [here](https://github.com/DynamoRIO/dynamorio/releases/download/cronbuild-7.0.17636/DynamoRIO-x86_64-Linux-7.0.17636-0.tar.gz).
2. This project utilizes the C++ library [Boost](https://boost.org). This library can be built from source, as explained [here](https://www.boost.org/doc/libs/1_66_0/more/getting_started/unix-variants.html). On [Ubuntu 16.04](http://releases.ubuntu.com/16.04.4/), this library can be installed as follows:
```bash
sudo apt-get update && sudo apt-get install libboost-all-dev
```
3. The project is built on [Ubuntu 16.04 LTS](http://releases.ubuntu.com/16.04.4/) via [CMake](https://cmake.org/). It requires compiler that supports `C++11`.
4. The system architecture is in the x86 or x86\_64 families.

For more specific information about requirements, visit the requirements wiki page [here](https://github.com/zwimer/DrShadowStack/wiki/Requirements).

## Installation Instructions

1. Install dependencies
2. Clone the repository
```bash
git clone https://github.com/zwimer/DrShadowStack
```
3. Configure the `DrShadowStack/src/CMakeLists.txt` file. Instructions in the file itself.
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

The full usage of this program can be found via: `./DrShadowStack --help`

In general, the usage is of this format: 
```bash
./DrShadowStack [--ss_mode <Mode>] <executable target> <target arguments>
```

There are two different modes, `int` (internal) and `ext` (external). The internal mode keeps the shadow stack internally in the DynamoRIO client. The external mode stores the stack in a separate process.

## Example

From the build directory of a previous version, an example could be:
```bash
vagrant@ubuntu-xenial ~/S/s/build> ./DrShadowStack ls -la ./
total 612
drwxrwxr-x 3 vagrant vagrant   4096 Apr  3 20:01 .
drwxrwxr-x 4 vagrant vagrant   4096 Apr  3 19:59 ..
-rw-rw-r-- 1 vagrant vagrant  14536 Apr  3 19:59 CMakeCache.txt
drwxrwxr-x 7 vagrant vagrant   4096 Apr  3 20:01 CMakeFiles
-rw-rw-r-- 1 vagrant vagrant   1381 Apr  3 19:59 cmake_install.cmake
-rw-rw-r-- 1 vagrant vagrant   8045 Apr  3 20:01 compile_commands.json
-rwxrwxr-x 1 vagrant vagrant 402168 Apr  3 20:01 DrShadowStack
-rwxrwxr-x 1 vagrant vagrant  53264 Apr  3 20:00 libss_dr_client.so
-rwxrwxr-x 1 vagrant vagrant  96304 Apr  3 19:59 libss_support.so
-rw-rw-r-- 1 vagrant vagrant  15265 Apr  3 20:01 Makefile
-rw-rw-r-- 1 vagrant vagrant   8157 Apr  3 20:01 ss_dr_client.ldscript
```

## Documentation

Additional documentation of DrShadowStack can be found in the [wiki](https://github.com/zwimer/DrShadowStack/wiki).

## Developers

Before pushing any code, please run the `run-before-push.sh` script. This will automatically update the changelog and format all `C++` code.

Additional documentation to each component of DrShadowStack is built automatically via [Travis CI](https://travis-ci.org/) utilizing [Doxygen](http://www.stack.nl/~dimitri/doxygen/), and hosted [here](https://zwimer.com/DrShadowStack).
