FROM ubuntu:20.04

LABEL maintainer="Willian Alves <wra@cesar.org.br>"

ARG GRPC_VERSION=1.34.0
ARG NUM_JOBS=4

ENV DEBIAN_FRONTEND noninteractive

# Install package dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        software-properties-common \
        autoconf \
        automake \
        libtool \
        pkg-config \
        ca-certificates \
        wget \
        git \
        curl \
        vim \
        gdb \
        valgrind \
        cmake \
        libssl-dev \
        liburiparser1 liburiparser-dev \
        protobuf-compiler \
        libprotobuf-dev \
        check \
        lcov \
        gcovr \
	libjansson-dev \
        libcjose-dev
RUN apt-get clean

# gRPC
# https://github.com/grpc/grpc/tree/master/src/cpp
# https://github.com/grpc/grpc/blob/master/BUILDING.md
RUN cd /tmp && \
    apt-get install -y build-essential autoconf libtool pkg-config && \
    git clone --recurse-submodules -b v${GRPC_VERSION} https://github.com/grpc/grpc && \
    cd grpc && \
    mkdir -p cmake/build && \
    cd cmake/build && \
    cmake -DgRPC_INSTALL=ON \
        -DgRPC_BUILD_TESTS=OFF \
        ../.. && \
    make -j${NUM_JOBS} && \
    make install

RUN apt-get update && apt-get install -y --no-install-recommends \
        libgtest-dev