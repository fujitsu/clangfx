#!/bin/bash

GCC=gcc
GXX=g++

BUILD_DIR=build

rm -rf $BUILD_DIR
mkdir $BUILD_DIR

NO_OUTLINE_ATOMICS_C=""
NO_OUTLINE_ATOMICS_CXX=""

GCC_MAJOR_VERSION=$($GCC -dumpversion | grep -oP '^\d+')
GXX_MAJOR_VERSION=$($GXX -dumpversion | grep -oP '^\d+')

if [ $((GCC_MAJOR_VERSION+0)) -ge 10 ]; then
  NO_OUTLINE_ATOMICS_C="-mno-outline-atomics"
fi

if [ $((GXX_MAJOR_VERSION+0)) -ge 10 ]; then
  NO_OUTLINE_ATOMICS_CXX="-mno-outline-atomics"
fi

pushd $BUILD_DIR
  cmake \
      -GNinja                                  \
      -DCMAKE_INSTALL_PREFIX=../install        \
      -DCMAKE_BUILD_TYPE=Release               \
      -DCMAKE_C_COMPILER="$GCC"                \
      -DCMAKE_CXX_COMPILER="$GXX"              \
      -DLLVM_LIBDIR_SUFFIX=64                  \
      -DBUILD_SHARED_LIBS=ON                   \
      -DCMAKE_C_FLAGS="${NO_OUTLINE_ATOMICS_C} -fno-strict-aliasing"   \
      -DCMAKE_CXX_FLAGS="${NO_OUTLINE_ATOMICS_CXX} -fno-strict-aliasing" \
      -DCMAKE_ASM_FLAGS="-fno-strict-aliasing" \
      -DLLVM_TARGETS_TO_BUILD="AArch64"        \
      -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;libcxx;libcxxabi;compiler-rt;lld;openmp;polly" \
      -DLLVM_ENABLE_TERMINFO=OFF               \
      -DLLVM_DEFAULT_TARGET_TRIPLE="aarch64-unknown-linux-gnu"   \
      -DCMAKE_CROSSCOMPILING=True              \
      -DCLANG_VENDOR="Fujitsu"                 \
      ../llvm
  ninja
  ninja install
popd
