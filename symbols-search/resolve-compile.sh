#!/bin/bash

#################
# Configuration #
#################

# Automatically acceptable libraries
WHITELIST=(libLLVM-3.1svn.so libclang.so) # prefer shared objects libraries to static libraries
BLACKLIST=(liblldb.so) # evict lldb (it contains almost all symbols declared in other libraries)

#####################
# Utility functions #
#####################

# Extracts resources (sub-scripts) located at the end of the script
extract_file() {
    awk -- "
BEGIN { out=0 }
out==1 && /^# \]\]>$/ { out=0 }
out==1 { print }
/^# <!\[$1\[$/ { out=1 }
" "$0"
}

# Extract a few sed expressions
sedUndefSymbols="$(extract_file "sed:extract undefined symbols")"
sedGccStdString="$(extract_file "sed:gcc std::string")"

# Checks if the first argument is listed in the following arguments
# In order to check for a value being included in an array, use:
#   included "value" "${array[@]}"
# and check the return value.
included() {
    what="$1"
    while [ "$#" -gt 0 ]; do
        shift
        [ "$what" == "$1" ] && return 0
    done
    return 1
}

# Like included(), but tests if an ending of the first argument match the proposed arguments
ending_included() {
    what="$1"
    while [ "$#" -gt 0 ]; do
        shift
        [ "${what/%$1/}" != "$what" ] && return 0
    done
    return 1
}

###################
# Begin of script #
###################

# Note the old flags
LDFLAGS=($LDFLAGS)
oldFlagsCount="${#LDFLAGS[@]}"

# Get our directory so we can call other sibling scripts
progdir="$(dirname "$0")"

# Create a temporary output file and a fifo
tmp="$(mktemp tmp-resolve-compile-XXXXXX.out)"
tmpfifo="$(mktemp tmp-resolve-compile-XXXXXX.fifo)"
rm "$tmpfifo"
mkfifo "$tmpfifo"

# Execute the provided command, and if it fails,
# analyze its output
if "$@" 2> "$tmp"; then
    newflags="unnecessary"
else
    newflags="none"
    echo "Command failed, finding libs..."
    # Extract undefined symbols
    sed -i -nre "$sedUndefSymbols" -- "$tmp"
    # Simplify g++ output, as it expands some STL template types
    sed -i -re "$sedGccStdString" -- "$tmp"
    lineCount="$(wc -l "$tmp" | sed -re 's/^\s*([0-9]+)\s+.*$/\1/')"
    currLine=0
    # Search the library defining each undefined symbol
    while read symbol; do
        echo -ne "$(( 100 * (++currLine) / lineCount )) %\r"
        skipSymbol="no"
        skipToEndOfInput="no"
        candidates=()
        blacklistedCandidates=()
        # Our companion script performs the search,
        # parse its output
        "$progdir"/search.sh "$symbol" > "$tmpfifo" &
        while read libs; do
            # The script outputs 4 lines per result
            # (or 0 if there is no result, but read would have failed)
            # Leave the last, blank line, so that tail can eat up the output
            # untill the end, without blocking, waiting for first output.
            read match
            read file
            lib="${libs%%:*}" # eg.: /usr/lib/libfoo.a
            whitelistMatch="no"
            blacklistMatch="no"
            # Searching in whitelist...
            if ending_included "$lib" "${WHITELIST[@]}"; then
                whitelistMatch="yes"
            else
                # Searching in blacklist...
                if ending_included "$lib" "${BLACKLIST[@]}"; then
                    blacklistMatch="yes"
                fi 
            fi
            if [ "$blacklistMatch" == "no" ]; then
                candidate="$lib"
                candidate="${candidate%.*}"
                candidate="${candidate##*lib}"
                # Possible candidate verify if already included in LDFLAGS
                if included "-l$candidate" "${LDFLAGS[@]}"; then
                    # Indeed, skipping symbol, already resolved
                    skipSymbol="yes"
                    skipToEndOfInput="yes"
                elif [ "$whitelistMatch" == "yes" ]; then
                    # We have a whitelist match, it replaces any other candidates, and exits
                    candidates=($lib)
                    skipToEndOfInput="yes"
                else
                    # Possible candidate verify if already included in candidates
                    if ! included "$lib" "${candidates[@]}"; then
                        # Nope, adding candidate
                        candidates[${#candidates[@]}]="$lib"
                    fi
                fi
            else
                # Forget this lib
                if ! included "$lib" "${blacklistedCandidates[@]}"; then
                    # Noting the blacklisted candidates
                    blacklistedCandidates[${#blacklistedCandidates[@]}]="$lib"
                fi
            fi
            if [ "$skipToEndOfInput" == "yes" ]; then
                # Eat all output and break
                tail >/dev/null
                break
            else
                # tail was not used, we must read the 4th, blank line to continue
                read blank
            fi
        done < "$tmpfifo"
        # Decition time
        if [ "${#candidates[@]}" == "0" -a "${#blacklistedCandidates[@]}" -gt 0 ]; then
            # Only blacklisted candidates were available
            echo "  Symbol has only blacklisted candidates: $symbol"
            echo "  Blacklisted candidates: ${blacklistedCandidates[@]}"
            # Use them anyway!
            candidates=("${blacklistedCandidates[@]}")
        fi
        if [ "$skipSymbol" == "yes" ]; then
             : # Symbol already resolved, continuing
        elif [ "${#candidates[@]}" -gt 0 ]; then
            # We have candidates
            chosen=""
            if [ "${#candidates[@]}" -gt 1 ]; then
                # Multiple candidates
                echo "  Symbol $symbol"
                # Print them
                echo "  Got multiple candidates for:"
                i=0
                for candidate in "${candidates[@]}"; do
                    echo "  $(((++i)))) $candidate"
                done
                # Let the user choose
                read -p "  Choose one: " choice <&1
                if [ "$choice" -gt 0 -a "$choice" -le "${#candidates[@]}" ]; then
                    # Reckless's choice...
                    chosen="${candidates[((choice-1))]}"
                else
                    # Coward's choice
                    echo "  Skipping!"
                    chosen=""
                fi
            else
                # Got a single candidate (the whitelist may have been involved)
                chosen="${candidates[0]}"
            fi
            if [ "$chosen" != "" ]; then # else the user skipped
                chosen="${chosen%.*}"
                chosen="${chosen##*lib}"
                echo "Adding -l$chosen"
                LDFLAGS[${#LDFLAGS[@]}]="-l$chosen"
                newflags="yes"
            fi
        elif [ "${#candidates[@]}" == "0" ]; then
            # No possibilities
            echo "Symbol $symbol"
            echo "  UNRESOLVABLE LINKAGE ISSUE!"
            echo "  This may be due to g++ being outputting expanded template class from the STL, while the indexed ones are probably not."
        fi
    done < "$tmp"
fi
echo "     " # erase "100 %"

# Remove temporary files
rm "$tmp"
rm "$tmpfifo"

# Conclusion
case "$newflags" in
    unnecessary)
        echo "The command succeeded!"
        ;;
    yes)
        echo "Here are the libraries you were missing:"
        LDFLAGS[0]="" # remove the old flags taken from the environment
        echo "${LDFLAGS[@]}"
        ;;
    none)
        echo "No new library needed."
        echo "The flags you have may not be properly ordered..."
        ;;
esac

#################
# End of script #
#################
exit 0

# <![sed:extract undefined symbols[
s/^.*:[0-9]+: undefined reference to `(.*)'$/\1/p
# ]]>

# <![sed:gcc std::string[
s/std::basic_string<char, std::char_traits<char>, std::allocator<char> >/std::string/
# ]]>
