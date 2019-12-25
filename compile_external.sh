#!/bin/bash
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CPUS="$( grep -c ^processor /proc/cpuinfo )"
export CFLAGS="-DXXH_CPU_LITTLE_ENDIAN=1 -mavx"
export CXXFLAGS="-DXXH_CPU_LITTLE_ENDIAN=1 -mavx"

cd $DIR/external/libsodium
./autogen.sh
./configure --enable-retpoline --enable-shared=no
make clean
make -j$CPUS

cd $DIR/external/libpqxx
./configure --disable-documentation
make clean
make -j$CPUS

cd $DIR/external/xxHash
make clean
make -j$CPUS
rm *.so*

cd $dir/external/capnproto
mkdir build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_TESTING=off ../c++/
make -j$CPUS

cd $DIR
