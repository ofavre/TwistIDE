#!/bin/bash

progdir="$(dirname "$0")"
if [ "$#" -eq 0 -o "$1" == "-h" -o "$1" == "--help" ]; then
    cat <<EOS
Usage: $0 -h, --help
       ${0//?/ } symbol [options]
Searches "symbol" inside the libraries.
Outputs the symbol properties, the full symbol demangled name, and the definition location in the source file.
\`options' is passed as is to grep.
EOS
    [ "$#" -eq 0 ] && exit 1 || exit 0
fi

# Open the subsidiary files that must not take part in grep results
exec 3<"$progdir"/data.prefix
exec 5<"$progdir"/data.source

# Simple grep output, with no line number coloring and no file name,
# but with match coloring.
symbol="$1"
shift
GREP_COLORS="ln=:se=" grep --no-filename -n -F -e "$symbol" --color=always "$@" -- "$progdir"/data.content | (
    # Read grep line numbers, and catch up with the subsidiary files,
    # then output the content of the Nth line for each files.
    currentLine=0
    while read result; do
        # Read the target line number
        lineNumber="${result%%:*}"
        # How many line should we read to reach that line
        (( diff = lineNumber - currentLine ))
        # Make sed advance $diff lines into data.prefix, and print the target line
        sed -nre "${diff} {p;q}" <&3
        # Extract grep's highlighted output
        echo "${result#*:}"
        # Use sed too against data.source
        sed -nre "${diff} {p;q}" <&5
        # Small visual separation
        echo
        # We're now further into the files
        currentLine="$lineNumber"
    done
)

# Close subsitiary files
exec 3>&-
exec 5>&-
