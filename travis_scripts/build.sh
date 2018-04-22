#!/bin/sh
set -e

cd $TRAVIS_BUILD_DIR

git clone https://github.com/DynamoRIO/dynamorio.git | true
ls -la
cd dynamorio
if [ -d ./build ]; then
	echo 'DynamoRIO cached'
	cd build
	DR=$(pwd)
else
	sudo apt-get install --yes cmake g++ g++-multilib doxygen transfig imagemagick ghostscript git
	mkdir build | true
	cd build
	DR=$(pwd)
	cmake ..
	make -j 2
fi

cd $TRAVIS_BUILD_DIR
mkdir src/build
cd src/build

cmake .. -DDynamoRIO_DIR=$DR/cmake -DDRRUN_PATH=$DR/bin64/drrun
make -j 2
