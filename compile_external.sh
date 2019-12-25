#!/bin/bash
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CPUS="$( grep -c ^processor /proc/cpuinfo )"

# Use these when you need to debug the libraries themselves
#export CFLAGS="-mavx -g3 -ggdb"
#export CXXFLAGS="-mavx -g3 -ggdb"

# Otherwise, use these.
export CFLAGS="-DNDEBUG -O2 -mavx -maes -mpclmul -mpopcnt -msse4.1 -msse4.2 -mrdrnd -mf16c -mfsgsbase -mfxsr -mxsave -mxsaveopt -fstack-protector-strong -fstack-clash-protection -fcf-protection -Wl,-O1 -Wl,-z,defs -Wl,-z,now -Wl,-z,relro -Wl,-z,noexecstack "
export CXXFLAGS="-DNDEBUG -O2 -mavx -maes -mpclmul -mpopcnt -msse4.1 -msse4.2 -mrdrnd -mf16c -mfsgsbase -mfxsr -mxsave -mxsaveopt -fstack-protector-strong -fstack-clash-protection -fcf-protection -Wl,-O1 -Wl,-z,defs -Wl,-z,now -Wl,-z,relro -Wl,-z,noexecstack "

function fail {
  echo "missing dir"
  exit 1
}

cd $DIR/external/libsodium || fail
./autogen.sh
./configure --enable-retpoline --enable-shared=no
make clean
make -j$CPUS

cd $DIR/external/libpqxx || fail
./configure --disable-documentation
make clean
make -j$CPUS

cd $DIR || fail
