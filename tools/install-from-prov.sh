#!/bin/bash
# install simserver package from provision server
# ----------------------------------------------------------------------------------
# usage:
# $ install-from-prov.sh <server02.dimiccs.com> <01320401> <password>
# ----------------------------------------------------------------------------------
# tips:
# how to get file size: " du -sb web.tar.gz | awk '{print $1}' "
# null.tar.gz --- 111 bytes
# ----------------------------------------------------------------------------------

# 12.04, 14.04, 16.04
RELEASE=`lsb_release -r|cut -f 2`
# i386, x86_64
ARCH=`uname -i`

if [ -z $2 ];then
    printf "usage: $0 <server02.dimiccs.com> <01320401> <password> [auto]\n"
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
PACKAGE_PATH="/provision/version/32/"
LOCALHOST_PATH="/provision/provision/version/32/"

PROV_SERVER=$1
PROV_VERSION=$2
PACKAGE_PWD=$3
AUTO_MODE=$4
PACKAGE_FILE=${PROV_VERSION}_simserver.tar.des3

# jump to home dir
cd ~

#####################################################################################
# download new package from provision server
#####################################################################################

# delete old package files
rm -f $PACKAGE_FILE
rm -f simserver.tar.des3
rm -f simserver.deb

# get package.des3 from provision server
if [ "$PROV_SERVER" = "localhost" ]; then
  cp -f ${LOCALHOST_PATH}${PACKAGE_FILE} .
else
  wget --no-cache ${PROV_SERVER}${PACKAGE_PATH}${PACKAGE_FILE}
fi

# check package.des3 exists?
if [ ! -f ${PACKAGE_FILE} ];then
  echo "FAILURE!!! ${PACKAGE_FILE} download fail!"
  exit 1
fi

# decrypt package.des3 to package.deb
echo "decrypt package: ${PACKAGE_FILE}..."
dd if=${PACKAGE_FILE} |openssl des3 -d -k $PACKAGE_PWD |tar zxf -

# check package.deb exists?
if [ ! -e simserver.deb ];then
  echo "FAILURE!!! ${PACKAGE_FILE} decrypt fail!"
  exit 1
fi


#####################################################################################
# update /provision/version
#####################################################################################
if [ ! -d /provision ]; then
  mkdir -p /provision
  mkdir -p /provision/mail
  mkdir -p /provision/backup
  mkdir -p /provision/provision
  mkdir -p /provision/provision/syslog-ng
  mkdir -p /provision/provision/upgrade
  mkdir -p /provision/provision/version
  mkdir -p /provision/provision/temp

  chmod 777 /provision/mail
  chmod 777 /provision/backup
  chmod 777 /provision/provision
  chmod 777 /provision/provision/syslog-ng
  chmod 777 /provision/provision/upgrade
  chmod 777 /provision/provision/version
  chmod 777 /provision/provision/temp

  ln -s /provision/provision/version /provision/version

elif [ ! -L /provision/version ]; then
  echo "[WARN]move old /provision/version to /provision/provision/version"
  
  mkdir -p /provision/mail
  mkdir -p /provision/backup
  mkdir -p /provision/provision
  mv /provision/syslog-ng /provision/provision
  mv /provision/upgrade /provision/provision
  mv /provision/version /provision/provision
  mv /provision/temp /provision/provision
  mkdir -p /provision/provision/syslog-ng
  mkdir -p /provision/provision/upgrade
  mkdir -p /provision/provision/version
  mkdir -p /provision/provision/temp

  chmod 777 /provision/mail
  chmod 777 /provision/backup
  chmod 777 /provision/provision
  chmod 777 /provision/provision/syslog-ng
  chmod 777 /provision/provision/upgrade
  chmod 777 /provision/provision/version
  chmod 777 /provision/provision/temp

  ln -s /provision/provision/version /provision/version
fi

#try to creat dir and chmod,whether it exist or not.
mkdir -p /provision/provision/log/nat
chmod 777 /provision/provision/log/nat

#####################################################################################
# update system language
#####################################################################################
echo "update system language to en_US.UTF-8 ..."
###/usr/share/locales/install-language-pack en_US
###locale-gen --purge
apt-get install language-pack-en
locale-gen en_US.UTF-8

#####################################################################################
# install new package at local
#####################################################################################

# stop simserver service
echo "stop simserver service ..."
echo [`date +"%Y-%m-%d %T"`]"[script]stop simserver service ..." | logger
service simserver stop

echo "wait 15 seconds ..."
sleep 15

# install new package
echo "dpkg -i simserver.deb"
echo [`date +"%Y-%m-%d %T"`]"[script]install new package ${PACKAGE_FILE} ..." | logger
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
  echo "ERROR! No system conf file! run: sudo /dinstar/bin/configure.sh"
fi

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

echo " "

