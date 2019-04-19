#!/bin/bash
# install full simserver package from provision server
# ----------------------------------------------------------------------------------
# usage:
# $ install-full-package.sh <server02.dimiccs.com> <01320401> <pkg_pwd> <remote_user_pwd>
# ----------------------------------------------------------------------------------
# notes:
# mysql root@localhost:    123456
# mysql dinstar@localhost: 123456
# mysql dinstar@%:         <remote_user_pwd>
# ----------------------------------------------------------------------------------

FILE_CONF="/etc/dinstar/system_conf.xml"
PKG_DB_SQL="/dinstar/cfg/simserver_local.sql"

PROV_URL=$1
VERSION=$2
PKG_PWD=$3
ROOT_USER=$4
ROOT_PWD=$5
REMOTE_PWD=$6

DB_NAME=""

if [ -z $6 ];then
    printf "usage: $0 <server02.dimiccs.com> <02320450> <pkg_pwd> <root> <password> <remote_user_pwd> \n"
    printf "notes: \n"
    printf "(1) pkg_pwd         - package decryption password. \n"
    printf "(2) root            - mysql root user. \n"
    printf "(3) password        - mysql root password. \n"
    printf "(4) remote_user_pwd - mysql user remote password. \n"
    printf "(5) if remote database used, must set remote_user_pwd as same as mysql user pwd. \n"
    exit 1
fi

# -------------------------------------------------
# get one param value from xml file:
#     <param name="param01" value="1"/>
# param_name MUST be unique one
# return: PARAM_VAL_OLD
# -------------------------------------------------
get_xml_param_val()
{
    local my_file=$1
    local my_param=$2

    PARAM_VAL=`cat $my_file | grep $my_param | awk '{print $3}' | cut -f 2 -d '"'`
}

###### main() ######
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR]Operation not permitted! Please run cmd by root, or sudo cmd ..."
    exit 1
fi

DATE_STR=`date +"%Y%m%d_%H%M%S"`

# home dir
cd ~

#####################################################################################
# update system language
#####################################################################################
echo "update system language to en_US.UTF-8 ..."
###/usr/share/locales/install-language-pack en_US
###locale-gen --purge
#apt-get install language-pack-en
#locale-gen en_US.UTF-8
#this package will be installed in 3rdparty-get files.

echo "download packages and install scripts..."

# delete old files
rm -f ./3rdparty-get.tar.gz
rm -f ./install-from-prov.sh

# get 3rdparty-get.tar.gz
wget --no-cache ${PROV_URL}/provision/version/patch/3rdparty-get.tar.gz
if [ ! -e 3rdparty-get.tar.gz ]; then
  echo " "
  echo "[ERROR] download fail! please check network."
  exit 1
fi

# get install-from-prov.sh
wget --no-cache ${PROV_URL}/provision/version/patch/install-from-prov.sh
if [ ! -e install-from-prov.sh ]; then
  echo " "
  echo "[ERROR] download fail! please check network."
  exit 1
fi
chmod +x ./install-from-prov.sh

# install 3rdparty packages
echo "install 3rdparty packages... please set mysql root as ${ROOT_USER}/${ROOT_PWD}"
apt-get update
cd ~
tar -xzvf ./3rdparty-get.tar.gz
cd ./3rdparty-get
./install-3rdparty-get.sh
if [ $? -ne 0 ]
then
    echo "install 3rdparty packages error!"
    exit 1
fi

# install simserver and set system settings
echo "install simserver... version ${VERSION}"
cd ~
./install-from-prov.sh ${PROV_URL} ${VERSION} ${PKG_PWD}

### get db name from system settings
get_xml_param_val ${FILE_CONF} "mysql_db"
DB_NAME="${PARAM_VAL}"
if [ "${DB_NAME}" = "" ]; then
  echo "[WARN] can't get mysql_db from ${FILE_CONF}"
  # old release
  DB_NAME="simserver"
fi

# backup old database of simserver
echo "backup old database of ${DB_NAME}..."
/dinstar/bin/dump-simserver-sql.sh ${ROOT_USER} ${ROOT_PWD} ${DB_NAME} ${DB_NAME}_backup_${DATE_STR}.sql

# load default database of simserver
echo "load default database of ${DB_NAME}..."
/dinstar/bin/load-simserver-sql.sh ${ROOT_USER} ${ROOT_PWD} ${REMOTE_PWD} ${DB_NAME} ${PKG_DB_SQL} auto

echo " "
echo "[SUCCESS] simserver full package has been installed."

