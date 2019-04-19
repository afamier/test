#!/bin/sh
# reserved the generated version.h, only for debugging
# ----------------------------------------------------------------------------------
# usage:
# $ makeapp.sh [simsrv] [version] [debug|release] [local|cloud] [x86|x86_64]
# ----------------------------------------------------------------------------------

makeapp()
{
    local product=$1
    local app=$2
    local ver=$3
    local bdebug=$4
    local blocal=$5
    local cpu=$6
    local os=$7
    local makepath=$8
    local ret
    btime=`date "+%Y-%m-%d %T"`

    echo "update version.h"
    chmod 777 ../source/include/version.h
    cp ../source/include/version.h.in ../source/include/version.h
    sed -i "s/version/$ver/g" ../source/include/version.h
    sed -i "s/buildtime/$btime/g" ../source/include/version.h
    sed -i "s/builddebug/$bdebug/g" ../source/include/version.h
    sed -i "s/buildlocal/$blocal/g" ../source/include/version.h

    # re-build simsafe
    echo "re-build simsafe"
    gcc -o simsafe ./simsafe.c

    # re-build get_server_magic
    echo "re-build get_server_magic"
    gcc -o get_server_magic ./get_server_magic.c

    # re-build udp_echo_server
    echo "re-build udp_echo_server"
    gcc -o udp_echo_server ./udp_echo_server.c

    # re-build udp_relay_server
    echo "re-build udp_relay_server"
    gcc -o udp_relay_server ./udp_relay_server.c

    cd $makepath

    rm -f ssp_version.o
    make clean
    make dep -j2
    make all -j2 LOCAL=$blocal
    ret=$?
    if [ $ret -ne 0 ]; then
        return 1
    fi
    #rm -f ../../tools/bin/$app
    #rm -f ../../tools/bin/$app.org
    #cp $product ../../tools/bin/$app
    #cp $product.org ../../tools/bin/$app.org
    #cd ../../tools/bin
    #rm -f $app.gz
    #rm -f $app.ldf
    #gzip $app
    #../addhead $app.gz $ver pr $cpu $product $os
    #ret=$?
    #rm -f $app.gz
    #mv $app.org $app
    #cd ..
    # rm -f ../source/include/version.h
    return $ret
}

######################################################################

if [ $# -lt 5 ];then
    echo "please input [simsrv] [version] [debug|release] [local|cloud] [x86|x86_64]"
    exit 1
fi

###### main() ######
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR]Operation not permitted! Please run cmd by root, or sudo cmd ..."
    exit 1
fi

my_product=$1
my_ver=$2
my_debug=$3
my_local=$4
my_env=$5

# check make debug
case $my_debug in
    debug)
        DEBUG=1
        ;;
    release)
        DEBUG=0
        ;;
    *)
        echo "please input [simsrv] [version] [debug|release] [local|cloud] [x86|x86_64]"
        exit 1
esac

# check make local
case $my_local in
    local)
        LOCAL=1
        ;;
    cloud)
        LOCAL=0
        ;;
    *)
        echo "please input [simsrv] [version] [debug|release] [local|cloud] [x86|x86_64]"
        exit 1
esac

# check make env
case $my_env in
    x86)
        STX=x86
        CPU=x86
        OS=linux
        MAKEPATH=../project/linux-x86
        echo "cp -f $MAKEPATH/makefile.x86 $MAKEPATH/makefile"
        cp -f $MAKEPATH/makefile.x86 $MAKEPATH/makefile
        ;;
    x86_64)
        STX=x86
        CPU=x86
        OS=linux
        MAKEPATH=../project/linux-x86
        echo "cp -f $MAKEPATH/makefile.x86_64 $MAKEPATH/makefile"
        cp -f $MAKEPATH/makefile.x86_64 $MAKEPATH/makefile
        ;;
    *)
        echo "please input [simsrv] [version] [debug|release] [local|cloud] [x86|x86_64]"
        exit 1
esac

# clean make bin
if [ -d bin ]; then
    rm -rf bin/app$STX.ldf
else
    mkdir bin
fi

# make start
# 	local product=$1
# 	local app=$2
# 	local ver=$3
# 	local bdebug=$4
# 	local blocal=$5
# 	local cpu=$6
# 	local os=$7
# 	local makepath=$8
# e.g. ./makeapp.sh simsrv 02320203 debug cloud x86
makeapp $my_product app$STX $my_ver $DEBUG $LOCAL $CPU $OS $MAKEPATH
echo "KEYS: $my_ver $my_debug $my_local"



