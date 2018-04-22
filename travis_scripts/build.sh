#!/bin/sh
set -e
cd $TRAVIS_BUILD_DIR

# Get the latest release
echo 'Locating latest DynamoRIO release...'
mkdir dr && cd dr
wget https://github.com/DynamoRIO/dynamorio/releases/latest -O - -nv > tmp
LATEST=$(cat tmp | egrep '/.*/.*/.*tar\.gz' -o | grep x86)

echo "Getting latest DynamoRIO release: $LATEST"
wget https://github.com/$LATEST -nv

echo "Expanding it..."
tar -zxf *.gz
cd $(ls | grep -v 'tar')
DR=$(pwd)

echo "Building DrShadowStack..."
cd $TRAVIS_BUILD_DIR
mkdir src/build && cd src/build
cmake .. -DDynamoRIO_DIR=$DR/cmake -DDRRUN_PATH=$DR/bin64/drrun
make -j 2
