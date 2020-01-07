#!/bin/bash

echo "#ifndef __TARGET_PROCS_H"
echo -e "#define __TARGET_PROCS_H\n"

m68k-linux-gnu-nm target.elf|while read addr type name
do
#echo "[$name]"
if [ $type == "t" -a $name != "gcc2_compiled." ]; then
    echo "#define TARGET_$name 0x$addr"
fi
done

echo "#endif"
