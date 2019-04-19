#!/bin/bash
# usage: dump-simserver-sql.sh <root> <password> <db_name> <simserver_local_xxxx.sql>
# readme of mysqldump
# 1. dump db definitions: 	mysqldump -uroot -p123456 -R --hex-blob --opt -d simserver > simserver_def.sql
# 2. dump db data:        	mysqldump -uroot -p123456 -R --hex-blob -t simserver > simserver_data.sql
# 3. dump db def and data:  mysqldump -uroot -p123456 -R --hex-blob simserver > simserver_all.sql
# 4. dump spec tables:      mysqldump -uroot -p123456 -R --hex-blob simserver --table tbl_version > simserver_cfg.sql

FILE_CONF="/etc/dinstar/system_conf.xml"

ROOT_USER=$1
ROOT_PWD=$2
DB_NAME=$3
DB_FILE=$4

PARAM_VAL=""
DB_HOST=""

if [ -z $4 ];then
  echo "Usage: $0 <root> <password> <db_name> <simserver_local_xxxx.sql>"
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

### check system_conf.xml
if [ ! -f $FILE_CONF ]; then 
  echo "[ERROR] dump simserver sql fail! $FILE_CONF not found"
  exit 1
fi

### get db host
get_xml_param_val ${FILE_CONF} "mysql_url"
DB_HOST="${PARAM_VAL}"
if [ "${DB_HOST}" = "" ]; then
  echo "[ERROR] can't get mysql_url from ${FILE_CONF}"
  exit 1
fi

### show databases
echo "Database List at ${DB_HOST}:"
mysql -h ${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -s << EOF
show databases;
QUIT
EOF

### dump start...
echo "****** ${DB_HOST}->${DB_NAME} dump start ..."

### dump database: simserver
mysqldump -h${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -R --hex-blob -f --default-character-set=utf8 ${DB_NAME} > ${DB_FILE}
if [ $? -ne 0 ]; then
  echo "****** ${DB_HOST}->${DB_NAME} dump database fail!"
  exit 1
fi

ls -la ${DB_FILE}
echo "****** ${DB_HOST}->${DB_NAME} dump database success!"

exit 0

