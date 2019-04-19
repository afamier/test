#!/bin/sh
# encrypt/decrypt filename
# ----------------------------------------------------------------------------------
# encrypt/decrypt filename
# 1. input password:
#     tar -zcvf - filename |openssl des3 -salt | dd of=filename.des3
#     dd if=filename.des3 |openssl des3 -d |tar zxf -
# 2. inline password:
#     tar -zcvf - filename |openssl des3 -salt -k password | dd of=filename.des3
#     dd if=filename.des3 |openssl des3 -d -k password|tar zxf -
# ----------------------------------------------------------------------------------

if [ -z $1 ];then
    printf "usage: $0 <filename.des3> [password] \n"
    exit 1
fi

if [ -z $2 ]; then
    dd if=$1 |openssl des3 -d |tar zxf -
else
    dd if=$1 |openssl des3 -d -k $2|tar zxf -
fi

