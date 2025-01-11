#!/usr/bin/env bash
set -euxo pipefail

CC="gcc"
PROGRAM="test"
SOURCES=$(ls ./*.c)
LINKS=""
OPTIMIZE="-O0"
ERROR="-Werror -Wall -Wextra"
SANITIZE="-fsanitize=undefined,address" # doesn't work on windows lol

$CC $OPTIMIZE $ERROR $SANITIZE $SOURCES -o $PROGRAM $LINKS

exit 0

