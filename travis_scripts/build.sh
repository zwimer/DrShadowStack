#!/bin/sh
set -e
cd $TRAVIS_BUILD_DIR

# Make the build directory
mkdir src/build && cd src/build

echo "Getting latest DynamoRIO 7.0.0"
wget 'https://github.com/DynamoRIO/dynamorio/releases/download/cronbuild-7.0.17636/DynamoRIO-x86_64-Linux-7.0.17636-0.tar.gz' -nv

echo "Expanding it..."
tar -zxf DynamoRIO-x86_64-Linux-7.0.17636-0.tar.gz
DR=$(pwd)/DynamoRIO-x86_64-Linux-7.0.17636-0

echo "Building DrShadowStack..."
cmake .. -DDynamoRIO_DIR=$DR/cmake -DDRRUN_PATH=$DR/bin64/drrun
make -j 2
