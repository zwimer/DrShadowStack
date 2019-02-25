FROM ubuntu:16.04

# Dependencies
RUN apt-get update
RUN apt-get install -yq wget git g++ make cmake libboost-program-options1.58-dev libboost-filesystem1.58-dev

# DynamoRIO
RUN    wget --progress=bar:force https://github.com/DynamoRIO/dynamorio/releases/download/cronbuild-7.0.17636/DynamoRIO-x86_64-Linux-7.0.17636-0.tar.gz \
    && tar -xzf DynamoRIO-x86_64-Linux-7.0.17636-0.tar.gz

# Clone and build
RUN    git clone https://github.com/zwimer/DrShadowStack \
    && mkdir DrShadowStack/src/build/ \
    && cd DrShadowStack/src/build/ \
    && cmake \
        "-DDynamoRIO_DIR:STRING=/DynamoRIO-x86_64-Linux-7.0.17636-0/" \
        "-DDRRUN_PATH:STRING=/DynamoRIO-x86_64-Linux-7.0.17636-0/bin64/drrun" \
		"-DCMAKE_PREFIX_PATH:STRING=/DynamoRIO-x86_64-Linux-7.0.17636-0/cmake" \
        .. \
    && make -j `nproc`

# Add it to the path
RUN ln -s /DrShadowStack/src/build/DrShadowStack /usr/bin/DrShadowStack
