#!/bin/bash

# Build files list
if ! [ -f files.lst ]; then
    echo "Use the following command to bootstrap your files.lst:"
    echo "    # Append libraries known to llvm-config"
    echo "    llvm-config --libfiles all | tr ' ' '\n' >> files.lst"
    echo "If you used create_shared_libs.sh, them run the following:"
    echo "    # Convert all .a to .so"
    echo "    sed -i -e 's/\.a$/.so/' files.lst"
    echo "You may then re-run this script."
    exit 1
fi
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

# Remove the no longer useful source
rm data



# Uses lorder to generate dependency relations,
# then uses tsort to generate a (possibly non unique)
# topological ordering on the libraries.
order() {
    # Generate the topological sort in files.ordered.lst (standard output)
    # List detected cycles in files.cycles.lst.tmp (error output)
    cat files.lst | xargs -d '\n' lorder | tsort > files.ordered.lst 2> files.cycles.lst.tmp
    # Deduplicate the cycles
    # The following program will read input like:
    #   tsort: -: input contains a loop:
    #   tsort: foo
    #   tsort: bar
    #   tsort: -: input contains a loop:
    #   tsort: bar
    #   tsort: foo
    #   tsort: -: input contains a loop:
    #   tsort: foo
    #   tsort: baz
    # and transforms it to: (sorting each member and reducing a loop to a single line)
    #   bar$foo$
    #   bar$foo$
    #   baz$foo$
prog='
# Take the input pipe it through `sort`
# Also replace newlines by "$", with a tailing "$"
# (this will serve reformatting by `tr`)
func sortThis(content) {
    sorter = "sort"
    rtn = ""
    print content |& sorter
    close(sorter, "to")
    first = "1"
    while((sorter |& getline) > 0) {
        rtn = rtn $0 "$"
    }
    close(sorter)
    return rtn
}
# Read each line of the input
{
    # Remove the "tsort: " prefix
    gsub(/^tsort: /, "")
    # On loop delimiter ("tsort: -: input contains a loop:", with spaces and text varying with the locale)
    if (match($0, /^[^:]*: [^:]*:$/)) {
        if (content != "") {
            # Sort the noted loop members
            print sortThis(content)
            # Start a new loop
            content = ""
        }
    } else {
        # Note a new loop member
        if (content == "")
            content = $0
        else
            content = content "\n" $0
    }
}
# Do not forget to process the last part too
END {
    if (content != "") {
        print sortThis(content)
    }
}
'
    # We then deduplicate loops using `sort -u` and expand "$" to new lines for readability.
    awk -- "$prog" files.cycles.lst.tmp | sort -u | tr '$' '\n' > files.cycles.lst
    # Remove old input
    rm files.cycles.lst.tmp
}

# Generate the inclusion ordering of the libraries
order
