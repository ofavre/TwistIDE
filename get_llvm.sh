#!/bin/sh

BASE=http://llvm.org/git/

echo "Downloading TwistIDE dependencies from the LLVM SVN repository to ./llvm"
echo ""
echo "Checking out LLVM (trunk) ..."
time -f "      Done in %E" git clone -q $BASE/llvm.git llvm

echo "Checking out LLDB (trunk) ..."
time -f "      Done in %E" git clone -q $BASE/lldb.git llvm/tools/lldb

echo "Checking out Clang (trunk) ..."
time -f "      Done in %E" git clone -q $BASE/clang.git llvm/tools/clang

echo "Checking out Compiler-RT (trunk) ..."
time -f "      Done in %E" git clone -q $BASE/compiler-rt.git llvm/projects/compiler-rt


getSha1FromRev() {
    # Somehow, `git svn find-rev r$1` does not work with the official git mirror of LLVM...
    git rev-list master --pretty | awk -v REV="$1" '
        /^commit / { commit = $2 };                       # remember the last commit sha1
        /^[ \t]*git-svn-id: [^@]+@[0-9]+/ {               # watch for svn revisions
            rev = gensub(/^.*@([0-9]+)$/, "\\1", "", $2); # extract the svn revision
            if (rev <= REV) {                             # the given revision can belong to another project
                                                          # of the whole svn tree, take the first ancestor
                print commit;                             # print the sha1
                exit;
            }
        }'
}

echo "Extracting revision numbers from LLDB scripts ..."
LLVM_REV=`grep -m 1 llvm_revision llvm/tools/lldb/scripts/build-llvm.pl | sed "s/[^0-9]*\([0-9]*\)[^0-9]*/\1/"`
CLANG_REV=`grep -m 1 clang_revision llvm/tools/lldb/scripts/build-llvm.pl | sed "s/[^0-9]*\([0-9]*\)[^0-9]*/\1/"`
cd llvm
LLVM_SHA1=$(getSha1FromRev $LLVM_REV)
cd - > /dev/null
cd llvm/tools/clang
CLANG_SHA1=$(getSha1FromRev $CLANG_REV)
cd - > /dev/null

cd llvm
echo "Updating LLVM to r$LLVM_REV ($LLVM_SHA1)..."
time -f "      Done in %E" git checkout -q $LLVM_SHA1
cd - > /dev/null

cd llvm/tools/clang
echo "Updating Clang to r$CLANG_REV ($CLANG_SHA1)..."
time -f "      Done in %E" git checkout -q $CLANG_SHA1
cd - > /dev/null

echo "Creating 'llvm-build' folder ..."
mkdir -p llvm-build

echo "Generating build environment (configure) ..."
cd llvm-build
../llvm/configure -q --enable-doxygen --enable-shared

# Count the number of logical CPUs (since compilation can benefit from hyperthreading anyway)
CPU=`grep processor /proc/cpuinfo | wc -l`
echo "Building LLVM & friends [Debug+Assert] (using $CPU jobs) ..."
time -f "      Done in %E" make -j$CPU

echo "Building LLVM & friends [Release] (using $CPU jobs) ..."
time -f "      Done in %E" make -j$CPU ENABLE_OPTIMIZED=1

