#!/bin/bash

prog=$(basename "$0")

PATH_INC="./inc"
PATH_SRC="./src"
PATH_TEST="./test"
PATH_BIN="./bin"

declare -A classes

# grep every global macro and extract its name
classes[-g]=$(grep -E '__.*__' ${PATH_INC}/* ${PATH_SRC}/* | cut -d : -f 2 | sed -nE 's/^.*\((__.*__)\).*$/\1/p')

# grep every unit specific macro and extract its name
classes[-u]=$(grep -E '__.*__' ${PATH_TEST}/* | cut -d : -f 2 | sed -nE 's/^.*\((__.*__)\).*$/\1/p')

declare -A shortcuts

# For every class of macros
for class in "${!classes[@]}"
do
    # For each macro in the current class
    for macro in ""${classes[$class]}""
    do
        # Create a key corresponding to the macro at hand
        key="-$(echo ${macro:2:1} | tr [:upper:] [:lower:])"

        if [[ "$key" =~ (-[ugxr]) ]]
        then
            echo "$prog: \"$macro\"'s shortcut shadows \"${BASH_REMATCH[1]}\" flag"
            exit 1
        fi

        entry="${shortcuts[$key]}"

        # If there is no entry matching the current key register it
        # Otherwise
        if [[ -n "$entry" ]]
        then
            # If they don't have different names
            if [[ "$entry" =~ (-?)"$macro" ]]
            then
                # If the macro at hand is global
                if [[ "$class" == -g ]]
                then
                    # It overrides the existing entry
                    shortcuts["$key"]="$class $macro"
                fi

                # Otherwise move on to the next macro
                continue
            fi

            # If they do have different names but same keys
            # then report a macro collision that needs to be
            # taken care of
            echo "$prog: macro collision detected \"$macro\" \""$entry"\""
            exit 1
        else
            shortcuts["$key"]="$class $macro"
        fi
    done
done

if [ "$1" == "-h" ] || [ "$1" == "--help" ]
then
    echo "# Options:"
    echo "# -u, --unit-define      Define a macro in a test unit"
    echo "# -g, --global-define    Define a macro globally"
    echo "# -x, --executable       Compile the specified executable   [default: everything in the test unit directory]"
    echo "# -r, --rebuild          Recompile library / executable     [default: do not recompile up-to-date files]"

    echo -e "\n# Shortcuts:"
    for macro in "${!shortcuts[@]}"
    do
        printf "# %s, %s\n" "$macro" "${shortcuts[$macro]}"
    done

    echo -e "\n# Usage:"
    echo "# $prog -u [MACRO]"
    echo "# $prog -g [MACRO]"
    echo "# $prog -x [name]"
    echo "# $prog -r"

    echo -e "\n# Example: $prog -r -u __BENCHMARK__ -u __QUIET__ -g __CACHE_SIZE__=32768"

    exit 0
fi

cmd="$*"
for key in ${!shortcuts[@]}
do
    full="${shortcuts[$key]}"; cmd=${cmd/$key/$full}
done

set -- ""${cmd[@]}""

while [ ! "$#" -eq 0 ]
do
    case "$1" in
        "-u" | "--unit-define")
        shift
        dexe="$dexe -D$1"
        shift
        ;;
        "-g" | "--global-define")
        shift
        dexe="$dexe -D$1"
        dlib="$dlib -D$1"
        shift
        ;;
        "-x" | "--executable")
        shift
        fexe=$(echo -e "$1\n$fexe")
        shift
        ;;
        "-r" | "--rebuild")
        rebuild=true
        shift
        ;;
        *)
        echo "$prog: invalid syntax! \"$*\""
        echo "                           ^"
        exit 1
        ;;
    esac
done

if ([ "$rebuild" ] && [ -z "$fexe" ]) || [ ! -z "$dlib" ]
then
    make clean
fi

make "DEFINED=$dlib"

if [ -z "$fexe" ]
then
    fexe=$(ls $PATH_TEST | tr '\n' ' ')
fi

echo "-e" "\n*** Compiling exe files ***"
echo "***"

for name in ""$fexe""
do
    if [ -z "$name" ]
    then
        continue
    fi

    if [[ "$name" =~ (\.?/?.+)/(.+) ]]
    then
        dir=${BASH_REMATCH[1]}
        file=${BASH_REMATCH[2]}

        if [ "$dir" == "$PATH_BIN" ]
        then
            name="$file"
        else
            echo "$prog: directory mismatch! \"$dir\""
            continue
        fi
    fi

    name="$PATH_BIN/${name//.*/}.exe"

    if ([ "$rebuild" ] || [ ! -z "$dexe" ]) && [ -x "$name" ]
    then
        rm -f "$name"
    fi

    make "$name" "DEFINED=$dexe"
done

echo "***"
