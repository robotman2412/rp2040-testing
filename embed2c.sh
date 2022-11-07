#!/bin/bash

# Check for help
case "$1" in
    -h|--h|-help|--help)
        echo "Usage: $0 [infile] [id]"
        echo "Produces a C const array with the contents of infile"
        exit 1
        ;;
esac

# Enforce parameters
test "$1" == "" && exit 1
test "$2" == "" && exit 1

# INITIALISER
infile="$1"
id="$2"

echo "// File dump of $infile"
echo "const unsigned char $id[] = {"
cat "$infile" | xxd -i
echo "};"
echo "const unsigned int ${id}_length = sizeof($id) / sizeof(unsigned char);"

# Success?
exit 0
