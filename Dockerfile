FROM ubuntu:16.04

# Dependencies
RUN    apt-get update \
    && apt-get install -yq cmake make libboost-all-dev
# TOOD
# apt-get install libboost-program-options1.62.0
# TOOD
RUN apt-get install -yq wget

# DynamoRIO
RUN    wget https://github.com/DynamoRIO/dynamorio/releases/download/cronbuild-7.0.17636/DynamoRIO-x86_64-Linux-7.0.17636-0.tar.gz \
    && tar -xzf DynamoRIO-x86_64-Linux-7.0.17636-0.tar.gz

# TOOD
RUN apt-get install -yq git
RUN apt-get install -yq g++

# Clone abd build
RUN    git clone https://github.com/zwimer/DrShadowStack \
    && mkdir DrShadowStack/src/build/ \
    && cd DrShadowStack/src/build/ \
    && cmake \
        "-DDynamoRIO_DIR:STRING=/DynamoRIO-x86_64-Linux-7.0.17636-0/" \
        "-DDRRUN_PATH:STRING=/DynamoRIO-x86_64-Linux-7.0.17636-0/bin64/drrun" \
		"-DCMAKE_PREFIX_PATH:STRING=/DynamoRIO-x86_64-Linux-7.0.17636-0/cmake" \
        .. \
    && make -j `nproc`

# Drop in
ENTRYPOINT cd ./DrShadowStack/src/build/ && /bin/bash
