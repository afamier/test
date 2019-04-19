#!/bin/bash
# usage: drop-simserver-sql.sh <root> <password> <db_name> 

FILE_CONF="/etc/dinstar/system_conf.xml"

ROOT_USER=$1
ROOT_PWD=$2
DB_NAME=$3

PARAM_VAL=""
DB_HOST=""

if [ -z $3 ];then
  echo "Usage: $0 <root> <password> <db_name>"
  exit 1
fi

###### main() ######
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR]Operation not permitted! Please run cmd by root, or sudo cmd ..."
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

###################################################
# main()
###################################################

### get db host
get_xml_param_val ${FILE_CONF} "mysql_url"
DB_HOST="${PARAM_VAL}"
if [ "${DB_HOST}" = "" ]; then
  echo "[ERROR] can't get mysql_url from ${FILE_CONF}"
  exit 1
fi

### show databases and drop spec database
echo "Database List at ${DB_HOST}:"
mysql -h ${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -s << EOF
show databases;
drop database ${DB_NAME};
QUIT
EOF

### drop start...
echo "drop database ${DB_HOST}->${DB_NAME}, please wait a moment ..."
mysql -h ${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -s << EOF
drop database ${DB_NAME};
QUIT
EOF

echo "Finished, drop database ${DB_HOST}->${DB_NAME}."

