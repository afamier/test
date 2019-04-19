#!/bin/sh
# encrypt/decrypt filename
# ----------------------------------------------------------------------------------
# encrypt/decrypt filename
# 1. inline password:
#     tar -zcvf - filename |openssl des3 -salt -k password | dd of=filename.des3
#     dd if=filename.des3 |openssl des3 -d -k password|tar zxf -
# 2. input password:
#     tar -zcvf - filename |openssl des3 -salt | dd of=filename.des3
#     dd if=filename.des3 |openssl des3 -d |tar zxf -
# ----------------------------------------------------------------------------------

if [ -z $1 ];then
    printf "usage: $0 <filename> [password] \n"
    exit 1
fi

if [ -z $2 ]; then
    tar -zcvf - $1 |openssl des3 -salt | dd of=$1.des3
else
    tar -zcvf - $1 |openssl des3 -salt -k $2 | dd of=$1.des3
fi

