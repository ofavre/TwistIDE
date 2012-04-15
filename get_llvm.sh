#!/bin/sh

BASE=http://llvm.org/svn/llvm-project

echo "Downloading TwistIDE dependencies from the LLVM SVN repository to ./llvm"
echo ""
echo "Checking out LLVM (trunk) ..."
time -f "      Done in %E" svn co --force -q $BASE/llvm/trunk llvm

echo "Checking out LLDB (trunk) ..."
time -f "      Done in %E" svn co --force -q $BASE/lldb/trunk llvm/tools/lldb

echo "Checking out Clang (trunk) ..."
time -f "      Done in %E" svn co --force -q $BASE/cfe/trunk llvm/tools/clang

echo "Checking out Compiler-RT (trunk) ..."
time -f "      Done in %E" svn co --force -q $BASE/compiler-rt/trunk llvm/projects/compiler-rt

echo "Extracting revision numbers from LLDB scripts ..."
LLVM_REV=`grep -m 1 llvm_revision llvm/tools/lldb/scripts/build-llvm.pl | sed "s/[^0-9]*\([0-9]*\)[^0-9]*/\1/"`
CLANG_REV=`grep -m 1 clang_revision llvm/tools/lldb/scripts/build-llvm.pl | sed "s/[^0-9]*\([0-9]*\)[^0-9]*/\1/"`

echo "Updating LLVM to r$LLVM_REV ..."
time -f "      Done in %E" svn update --force -q -r $LLVM_REV llvm

echo "Updating Clang to r$CLANG_REV ..."
time -f "      Done in %E" svn update --force -q -r $CLANG_REV llvm/tools/clang

echo "Creating 'llvm-build' folder ..."
mkdir -p llvm-build

echo "Generating build environment (configure) ..."
cd llvm-build
../llvm/configure

# Count the number of logical CPUs (since compilation can benefit from hyperthreading anyway)
CPU=`grep processor /proc/cpuinfo | wc -l`
echo "Building LLVM & friends [Debug+Assert] (using $CPU jobs) ..."
make -j$CPU

echo "Building LLVM & friends [Release] (using $CPU jobs) ..."
make -j$CPU ENABLE_OPTIMIZED=1

