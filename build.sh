#!/bin/bash

CC="g++"
CFLAGS="-Wall -Wextra -std=c++17 -g3"

PATH_INC="./inc"
PATH_SRC="./src"
PATH_TST="./test"
PATH_BIN="./bin"

function confirm
{
    if [ -e "$1" ]
    then
        read -p "Are you sure you want to overwrite \"$1\": " answer
        if [[ "$answer" != [yY] ]] && [[ "$answer" != [yY][eE][sS] ]]
        then
            exit 1
        fi
    fi
}

function grepinc
{
	local includes=$(grep -Eo '["<].*\.[hi]pp[">]' $1)

	if [ -z "$includes" ]
	then
		return
	fi

	local dependencies=""

	for include in ""$includes""
	do
		include=${include:1:${#include} - 2}

		local entry="${visited[$include]}"

		if [[ -n "$entry" ]]
		then
			continue
		else
			visited["$include"]=true
			grepinc "$PATH_INC/$include"
		fi
	done
}

function generate
{
    echo
    echo "CC = $CC"
    echo "CFLAGS = $CFLAGS"
    echo

    echo "PATH_SRC = $PATH_SRC"
    echo "PATH_INC = $PATH_INC"
    echo "PATH_BIN = $PATH_BIN"
    echo "PATH_TST = $PATH_TST"
    echo

    echo ".PHONY: all"
    echo "all:"
    echo -e "\tmkdir -p \$(PATH_BIN)"
    echo -e "\t@echo"
    echo -e "\t@echo \"*** Compiling object files ***\""
    echo -e "\t@echo \"***\""
    echo -e "\tmake \$(OBJS)"
    echo -e "\t@echo \"***\""

    echo ".PHONY: clean"
    echo "clean:"
    echo -e "\t@echo"
    echo -e "\t@echo \"*** Purging binaries ***\""
    echo -e "\t@echo \"***\""
    echo -e "\trm -rv \$(PATH_BIN)"
    echo -e "\t@echo \"***\""

    echo
    echo "\$(PATH_BIN)/%.exe: \$(PATH_TST)/%.cpp \$(OBJS)"
    echo -e "\t\$(CC) -I \$(PATH_INC) \$(DEFINED) \$(CFLAGS) \$< \$(OBJS) -o \$@"
    echo

    objs="OBJS = \$(addprefix \$(PATH_BIN)/, "

    deps=""
    rules=""

    files=$(ls "$PATH_SRC");
    for file in ""$files""
    do
        declare -A visited

        grepinc "$PATH_SRC/$file"; includes=${!visited[@]}

        unset visited

        file=${file%.cpp};

        rule="\$(PATH_BIN)/$file.o:"

        if [ -n "$includes" ]
        then
            deps_name=$(echo $file | head -c 4 | tr [:lower:] [:upper:])_DEP

            rule="$rule \$($deps_name)"

            deps_list="\$(addprefix \$(PATH_INC)/, $includes) \$(PATH_SRC)/$file.cpp"

            deps="$deps$deps_name = $deps_list\n"
        else
            deps_name=""
            deps_list=""
        fi
        
        rule="$rule\n\t\$(CC) -I \$(PATH_INC) \$(DEFINED) \$(CFLAGS) \$(PATH_SRC)/$file.cpp -c -o \$(PATH_BIN)/$file.o"

        rules="$rules$rule\n\n"

        objs="$objs $file.o"
    done

    objs="$objs)"

    echo -e "$deps\n$rules\n$objs"
}

prog=$(basename "$0")

declare -A classes

# grep every global macro and extract its name
classes[-g]=$(grep -Ev '//' ${PATH_INC}/* ${PATH_SRC}/* | grep -E '__.*__' | cut -d : -f 2 | sed -nE 's/^.*\((__.*__)\).*$/\1/p')

# grep every unit specific macro and extract its name
classes[-u]=$(grep -Ev '//' ${PATH_TST}/* | grep -E '__.*__' | cut -d : -f 2 | sed -nE 's/^.*\((__.*__)\).*$/\1/p')

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
            echo "$prog: macro collision detected \"$macro\" \""$(echo "$entry" | cut -d ' ' -f 2)"\""
            exit 1
        else
            shortcuts["$key"]="$class $macro"
        fi
    done
done

if [ "$1" == "--help" ]
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

if [ "$1" == "--makefile" ]
then
    confirm "Makefile"; generate > Makefile

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
    fexe=$(ls $PATH_TST)
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
