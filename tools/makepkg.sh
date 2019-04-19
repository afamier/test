#!/bin/sh
# generate simserver install package
# ----------------------------------------------------------------------------------
# running path: ./tools
# usage: makepkg.sh
# ----------------------------------------------------------------------------------
# version info: ./source/include/version.h
#     cat ./source/include/version.h | grep SOFTWARE_VERSION | awk '{print $3}' | cut -f 2 -d '"'
#     cat ./source/include/version.h | grep SOFTWARE_BUILDTIME | awk '{print $3}' | cut -f 2 -d '"'
# ----------------------------------------------------------------------------------
# encrypt/decrypt simserver.deb
#     tar -zcvf - pma|openssl des3 -salt -k password | dd of=pma.des3
#     dd if=pma.des3 |openssl des3 -d -k password|tar zxf -
# ----------------------------------------------------------------------------------

# 12.04, 14.04, 16.04
RELEASE=`lsb_release -r|cut -f 2`
# i386, x86_64
ARCH=`uname -i`

# release type: i386, amd64
PKG_FILE=""
SQL_FILE=""
if [ "$ARCH" = "x86_64" ]; then
    PKG_FILE="simserver_1.0.1-1_amd64.deb"
    SQL_FILE="simserver_local.sql.x86_64"
else
    PKG_FILE="simserver_1.0.1-1_i386.deb"
    SQL_FILE="simserver_local.sql.x86"
fi

if [ -z $2 ];then
    printf "usage: $0 <pkg_password> <simsrv.org_password> \n"
    exit 1
fi

###### main() ######
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR]Operation not permitted! Please run cmd by root, or sudo cmd ..."
    exit 1
fi

PACKAGE_PWD=$1
SIMSRV_PWD=$2

# get project dir
PROJ_HOME="sim-server"
CUR_DIR=`pwd`
BASE_DIR=${CUR_DIR%%${PROJ_HOME}*}${PROJ_HOME}
SQL_DIR=${BASE_DIR}/externals/mysql
MAKE_DIR=${BASE_DIR}/project/linux-x86

# clear old package
cd $BASE_DIR
rm -f ./${PKG_FILE}
rm -f ./simserver.deb
rm -f ./package
rm -f $SQL_DIR/simserver_local.sql

# clear old work dir
rm -rf /dinstar/bin
rm -rf /dinstar/cfg
rm -rf /dinstar/log
rm -rf /dinstar/webapps
rm -rf /dinstar/test_api.cloud
rm -rf /dinstar/test_api.mibox

# dos2unix
dos2unix ./tools/start_app.sh
dos2unix ./tools/stop_app.sh
dos2unix ./tools/install-full-package.sh
dos2unix ./tools/install-simserver.sh
dos2unix ./tools/install-from-prov.sh
dos2unix ./tools/configure.sh
dos2unix ./tools/update-system-conf.sh
dos2unix ./tools/load-system-conf.sh
dos2unix ./tools/load-restund-conf.sh
dos2unix ./tools/delete-server-license.sh
dos2unix ./tools/delete-domain-license.sh

dos2unix ./tools/install-oprofile.sh
dos2unix ./tools/start_oprofile.sh
dos2unix ./tools/stop_oprofile.sh
dos2unix ./tools/report_oprofile.sh

dos2unix ./tools/load-simserver-sql.sh
dos2unix ./tools/dump-simserver-sql.sh
dos2unix ./tools/drop-simserver-sql.sh
dos2unix ./tools/update-database.sh
dos2unix ./tools/backup-database.sh
dos2unix ./tools/install-schemasync-from-prov.sh
dos2unix ./tools/strip_index_0212.sh
dos2unix ./tools/encrypt-file.sh
dos2unix ./tools/decrypt-file.sh
dos2unix ./tools/udp_echo_server.py
dos2unix ./tools/udp_echo_client.py
dos2unix ./tools/log_split.sh
dos2unix ./tools/system_conf.xml_default
dos2unix ./tools/license_v1.xml
dos2unix ./tools/license_v2.xml
dos2unix ./tools/config_default
dos2unix ./tools/simserver
dos2unix ./tools/logrotate-simsrv

# dns conf files
dos2unix ./etc/bind/named.conf.options
dos2unix ./etc/bind/named.conf.local_default
dos2unix ./etc/bind/db.dns_resv_list
dos2unix ./etc/bind/db.dns_domain01_default
dos2unix ./etc/bind/db.dns_domain01_default

dos2unix ./etc/syslog-ng/syslog-ng.conf

# externals/webapps apps
dos2unix ./externals/webapps/install_webapps.sh

# encrypt simsrv.org for debugging, input password
echo "encrypt simsrv.org for debugging..."
cd $MAKE_DIR
tar -zcvf - simsrv.org |openssl des3 -salt -k $SIMSRV_PWD| dd of=simsrv.org.des3

# dump simserver_local to sql file
echo "dump simserver_04xx_64 to sql file..."
cd $BASE_DIR/tools
./dump-simserver-sql.sh root 123456 simserver_04xx_64 $SQL_DIR/simserver_local.sql
if [ ! -f $SQL_DIR/simserver_local.sql ]; then
  echo "FAILURE!!! simserver_local.sql dump fail!"
  exit 1
else
  tmpSize=`du -sb $SQL_DIR/simserver_local.sql | awk '{print $1}'`
  if [ ${tmpSize} -lt 1024 ]; then
    echo "FAILURE!!! simserver_local.sql dump fail!"
    exit 1
  fi
  
  # remove database name at simserver_local.sql
  #sed -i "s/^CREATE DATABASE.*//" $SQL_DIR/simserver_local.sql
  #sed -i "s/^USE `simserver_04xx_64`.*//" $SQL_DIR/simserver_local.sql
fi

# backup simserver_local to sql file
cp -f $SQL_DIR/simserver_local.sql $SQL_DIR/$SQL_FILE

# make new package
echo "make pkg..."
cd $MAKE_DIR
make pkg

# check new package exists?
cd $BASE_DIR
if [ ! -e ${PKG_FILE} ]; then
  echo "FAILURE!!! ${PKG_FILE} generate fail!"
  exit 1
fi

# rename ${PKG_FILE}
mv ${PKG_FILE} simserver.deb

# encrypt simserver.deb into simserver.tar.des3 with simple password
echo "encrypt simserver.deb into simserver.tar.des3 ..."
tar -czf - simserver.deb|openssl des3 -salt -k $PACKAGE_PWD| dd of=simserver.tar.des3

# check simserver.tar.des3 file
tmpSize=`du -sb simserver.tar.des3 | awk '{print $1}'`
if [ ${tmpSize} -lt 1024 ]; then
  echo "FAILURE!!! simserver.tar.des3 generate fail!"
  exit 1
fi


# get version & buildtime info
VERSION_INFO=`cat source/include/version.h | grep SOFTWARE_VERSION | awk '{print $3}' | cut -f 2 -d '"'`
BUILDTIME_INFO1=`cat source/include/version.h | grep SOFTWARE_BUILDTIME | awk '{print $3}' | cut -f 2 -d '"'`
BUILDTIME_INFO2=`cat source/include/version.h | grep SOFTWARE_BUILDTIME | awk '{print $4}' | cut -f 1 -d '"'`
BUILDTIME_INFO="${BUILDTIME_INFO1} ${BUILDTIME_INFO2}"
MD5_INFO=`md5sum simserver.tar.des3|cut -d ' ' -f1`

# add version info
cp simserver.tar.des3 ${VERSION_INFO}_simserver.tar.des3

# update package info
cp -f tools/package .

sed -e "s/{VERSION}/${VERSION_INFO}/g" package > package.tmp
cp -f package.tmp package
sed -e "s/{BUILDTIME}/${BUILDTIME_INFO}/g" package  > package.tmp
cp -f package.tmp package
sed -e "s/{MD5}/${MD5_INFO}/g" package  > package.tmp
cp -f package.tmp package
rm -f package.tmp

# generate new package.tar.gz
rm -f ${VERSION_INFO}.tar.gz
tar -czvf ${VERSION_INFO}.tar.gz package simserver.tar.des3
ls -la ${VERSION_INFO}.tar.gz

echo "Version: ${VERSION_INFO}, BuildTime: ${BUILDTIME_INFO}, MD5: ${MD5_INFO}"
echo "*** SUCCESS!"

