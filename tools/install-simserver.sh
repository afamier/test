#!/bin/bash
# install simserver package at local
# ----------------------------------------------------------------------------------
# usage:
# $ install-simserver.sh simserver.deb
# $ install-simserver.sh simserver.tar.des3 [password] [auto]
# ----------------------------------------------------------------------------------
# tips:
# how to get file size: " du -sb web.tar.gz | awk '{print $1}' "
# null.tar.gz --- 111 bytes
# ----------------------------------------------------------------------------------

# 12.04, 14.04, 16.04
RELEASE=`lsb_release -r|cut -f 2`
# i386, x86_64
ARCH=`uname -i`


if [ -z $1 ];then
    printf "usage 1: $0 simserver.deb [auto] \n"
    printf "usage 2: $0 simserver.tar.des3 [password] [auto] \n"
    exit 1
fi

###### main() ######
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR]Operation not permitted! Please run cmd by root, or sudo cmd ..."
    exit 1
fi

SYSTEM_CONF="/etc/dinstar/system_conf.xml"

PACKAGE_FILE="$1"
FILE_PWD="$2"
AUTO_MODE="$3"
FILE_TYPE="${PACKAGE_FILE##*.}"

#####################################################################################
# check package at local
#####################################################################################

# check package type
if [ ! "${FILE_TYPE}" = "deb" ]; then
  if [ ! "${FILE_TYPE}" = "des3" ]; then
    echo "[ERROR] unknown simserver package type!"
    echo "usage-1: $0 <simserver.deb> [auto]"
    echo "usage-2: $0 <simserver.tar.des3> [password] [auto]"
    exit 1
  fi
fi

# check package exists?
if [ ! -f ${PACKAGE_FILE} ]; then
  echo "[ERROR] ${PACKAGE_FILE} not exists!"
  exit 1
fi

# decrypt package.des3
if [ "${FILE_TYPE}" = "des3" ]; then
  # delete old package file
  rm -f simserver.deb

  # decrypt package.des3 to package.deb
  echo "decrypt package: ${PACKAGE_FILE} ..."
  if [ "${FILE_PWD}" = "" ]; then
    dd if=${PACKAGE_FILE} |openssl des3 -d |tar zxf -
  else
    dd if=${PACKAGE_FILE} |openssl des3 -d -k ${FILE_PWD} |tar zxf -
  fi

  # check package.deb exists?
  if [ ! -e simserver.deb ];then
    echo "[ERROR] ${PACKAGE_FILE} decrypt fail!"
    exit 1
  fi
else
  # patch
  AUTO_MODE="$FILE_PWD"
fi

#####################################################################################
# install new package at local
#####################################################################################

# stop simserver service
echo "stop simserver service ..."
echo [`date +"%Y-%m-%d %T"`]"[script]stop simserver service..." | logger
service simserver stop

echo "wait 15 seconds ..."
sleep 15

# install new package
echo "dpkg -i simserver.deb"
echo [`date +"%Y-%m-%d %T"`]"[script]install new package ${PACKAGE_FILE}..." | logger
dpkg -i simserver.deb > /dev/null


# fix env issues
if [ ! -e /sbin/insserv ]; then
  ln -s  /usr/lib/insserv/insserv /sbin/insserv
fi

if [ ! -d /etc/dinstar ]; then
  mkdir /etc/dinstar
fi

# force to add service simserver
if  [ "$RELEASE" = "12.04" ]
then
    # ubuntu chkconfig update
    apt-get install chkconfig -qq
    cp -f /dinstar/cfg/simserver /etc/init.d/simserver
    update-rc.d -f simserver remove
    update-rc.d -f simserver start 80 2 3 4 5 . stop 80 2 3 4 5 .
    ### /sbin/chkconfig -d simserver > /dev/null
    ### /sbin/chkconfig -a simserver > /dev/null
elif  [ "$RELEASE" = "14.04" ]
then
    # ubuntu sysv-rc-conf update
    apt-get install sysv-rc-conf -qq
    cp -f /dinstar/cfg/simserver /etc/init.d/simserver
    sysv-rc-conf --level 2345 simserver on
elif  [ "$RELEASE" = "16.04" ]
then
    # ubuntu sysv-rc-conf update
    apt-get install sysv-rc-conf -qq
    cp -f /dinstar/cfg/simserver_16.04 /etc/init.d/simserver
    sysv-rc-conf --level 2345 simserver on
fi


# check and update web libs, would be installed from /dinstar/backup
cd /dinstar/webapps
tmpSize=`du -sb extjs.tar.gz | awk '{print $1}'`
if [ ${tmpSize} -gt 1024 ]; then
  echo "copy web libs into /dinstar/backup ..."

  mkdir -p /dinstar
  mkdir -p /dinstar/backup

  cp -f extjs.tar.gz /dinstar/backup
  cp -f lib_web.tar.gz /dinstar/backup
fi

# check and update web apps, would be installed from /dinstar/webapps
cd /dinstar/webapps
tmpSize=`du -sb web.tar.gz | awk '{print $1}'`
if [ ${tmpSize} -lt 1024 ]; then
  echo "*** SUCCESS! webapps is null, only for debugging."
else
  # install new webapps
  echo "install webapps ..."
  ./install_webapps.sh

  echo "*** SUCCESS! full release, webapps was upgraded."
fi

# import old system conf file
cd /dinstar/bin
./update-system-conf.sh

# load and update system conf file
cd /dinstar/bin
./configure.sh $AUTO_MODE

if [ -e ${SYSTEM_CONF} ]; then
  echo " "
else
  echo " "
  echo "ERROR! No system conf file! please run: sudo /dinstar/bin/configure.sh"
fi

# show chkconfig info
if  [ "$RELEASE" = "12.04" ]
then    
    # show chkconfig info
    echo " "
    echo "chkconfig -l simserver"
    chkconfig -l simserver
elif  [ "$RELEASE" = "14.04" ]
then
    # show sysv-rc-conf info
    echo " "
    echo "sysv-rc-conf --list simserver"
    sysv-rc-conf --list simserver
elif  [ "$RELEASE" = "16.04" ]
then
    # show sysv-rc-conf info
    echo " "
    echo "sysv-rc-conf --list simserver"
    sysv-rc-conf --list simserver
fi


# check database updates
echo " "
echo "Notes: for database updates, please run: sudo /dinstar/bin/update-database.sh"
echo "       for db_split updates, please run: sudo /dinstar/bin/db-split-log-tables.py"
echo "       for db_split updates, please run: sudo /dinstar/bin/db-split-pmd-tables.py"
echo "       for cloudenv updates, please run: sudo /dinstar/cloudenv/bin/configure.sh"

### reboot system in auto mode 
if [ "${AUTO_MODE}" = "auto" ]; then
  sync
  sleep 1
  sync
  sleep 1
  sync
  sleep 1

  reboot
fi

cd ~
echo " "

