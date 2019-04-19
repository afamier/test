#! /bin/bash
# oprofile tools

###### main() ######
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR]Operation not permitted! Please run cmd by root, or sudo cmd ..."
    exit 1
fi

# get oprofile package
wget http://server02.dimiccs.com/provision/profile/oprofile-0.9.9.tar.gz

# install 3rd libs
apt-get install libpopt-dev
apt-get install binutils-dev
apt-get install build-essential
apt-get install g++

# env settings
groupadd oprofile
useradd -g oprofile oprofile

# tar oprofile package
tar -zxf oprofile-0.9.9.tar.gz
cd ./oprofile-0.9.9

# make install
./configure
make install

# show version
opcontrol --version

echo "****** install oprofile success!"
exit 0

