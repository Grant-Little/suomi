#!/usr/bin/env bash
set -euo pipefail

CC="gcc"
#ERROR="-Werror -Wall -Wextra"
ERROR=""

declare -a LIBRARY=()
for lib_file in ../*.c; do
    LIBRARY+=("$lib_file")
done

declare -a EXAMPLES_SRC=()
declare -a EXAMPLES_BIN=()
for example_file in ./*.c; do
    EXAMPLES_SRC+=("$example_file")
    EXAMPLES_BIN+=("${example_file::-2}")
done

declare -a TO_BUILD=()
for ((i = 0; i < ${#EXAMPLES_SRC[@]}; i++)); do
    TO_BUILD+="${EXAMPLES_SRC[i]} "
    TO_BUILD+="${LIBRARY[*]}"

    $CC $ERROR ${TO_BUILD[*]} -o ${EXAMPLES_BIN[i]}

    TO_BUILD=()
done

exit 0

