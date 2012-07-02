#!/bin/bash

# Build files list
[ -f files.lst ] || touch files.lst
# Append libraries known to llvm-config
llvm-config --libfiles all | tr ' ' '\n' >> files.lst
# Deduplicate
sort -u files.lst > files.lst.uniq
mv files.lst.uniq files.lst

# Functions for symbol extraction
do_one() {
    nm --print-file-name --demangle --extern-only --line-numbers --no-sort --defined-only "$1" >> data
}
do_each() {
    cnt="$(wc -l "$1" | cut -d ' ' -f 1)"
    i=0
    while read file; do
        (( i++ ))
        echo -n -e "\r$((( 100*i/cnt ))) %"
        do_one "$file"
    done < "$1"
    echo
}

# Perform symbol extraction into ./data
rm -f data
do_each files.lst

# Function for splitting the data into 3 parts:
# The symbol description: library file name, object file name, address and type
# The symbol name itself, demangled
# The definition source file location
split() {
    sed -re 's/^([^ ]+ .) .*$/\1/' data > data.prefix
    sed -re 's/^[^ ]+ . //' -e 's/\t[^ \t]+$//' data > data.content
    sed -re 's/^[^ ]+ . .*\t([^ \t]+)$/\1/i;t;c\ ' data > data.source
}

# Split the data into 3
# This enables searching solely against the symbol name,
# and prevent false positives from file names.
split
