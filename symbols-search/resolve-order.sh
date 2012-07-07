#!/bin/bash

if [ "$1" == "-h" -o "$1" == "--help" ]; then
	cat <<EOS
Usage: $0 [lib] [...]
Where lib can be either of the following form:
  /path/to/libA.{a,so}    The path, name and extension must match.
  libA.{a,so}             The name and extension must match.
  [-l]A                   The name must match.
                          Will search for .so libs before .a ones.
EOS
	exit 0
fi

progdir="$(dirname "$0")"

# Use files.ordered.lst to get the order of each libs
# output the input argument prefixed by the line number
while [ "$#" -ge 1 ]; do
	pattern="$1"
	shift
	case "$pattern" in
	*.a|*.so)
		if ! rslt="$(grep -n -F "$pattern" "$progdir"/files.ordered.lst)"; then
			echo "Unable to find a matching library for \"$pattern\"!" >&2
			continue
		fi
		echo "${rslt%%:*}:$pattern"
		;;
	*)
		if ! rslt="$(grep -n -E "/lib${pattern#-l}.so\$" "$progdir"/files.ordered.lst)"; then
			if ! rslt="$(grep -n -E "/lib${pattern#-l}.a\$" "$progdir"/files.ordered.lst)"; then
				echo "Unable to find a matching library for \"${pattern#-l}\"!" >&2
				continue
			fi
		fi
		echo "${rslt%%:*}:$pattern"
		;;
	esac
done |
# Then sort using the line number
sort -t ':' -k 1 |
# Remove the line number, keep only the ordered input arguments
cut -d ':' -f 2- |
# Put them all back onto one line
tr '\n' ' ' |
# Replace the last space back with a newline
sed -e 's/ $/\n/'
