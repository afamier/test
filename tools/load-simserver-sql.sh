#!/bin/bash
# usage: load-simserver-sql.sh <root> <password> <remote_user_pwd> <db_name> <simserver_local_xxxx.sql> [auto]
# readme of mysqldump
# 1. dump db definitions: 	mysqldump -uroot -p123456 -R --opt -d simserver > simserver_def.sql
# 2. dump db data:        	mysqldump -uroot -p123456 -R -t simserver > simserver_data.sql
# 3. dump db def and data:  mysqldump -uroot -p123456 -R simserver > simserver_all.sql
# 4. dump spec tables:      mysqldump -uroot -p123456 -R simserver --table tbl_version > simserver_cfg.sql

FILE_CONF="/etc/dinstar/system_conf.xml"

ROOT_USER=$1
ROOT_PWD=$2
REMOTE_PWD=$3
DB_NAME=$4
DB_FILE=$5
AUTO_MODE=$6

PARAM_VAL=""
DB_HOST=""
DB_USER=""
DB_PWD=""

if [ -z $5 ];then
  echo "Usage: $0 <root> <password> <remote_user_pwd> <db_name> <simserver_local_xxxx.sql> [auto]"
  echo "notes:"
  echo "(1) root            - mysql root user."
  echo "(2) password        - mysql root password."
  echo "(3) remote_user_pwd - mysql user remote password."
  echo "(4) db_name         - default is simserver."
  echo "(5) simserver_local_xxxx.sql, default file is /dinstar/cfg/simserver_local.sql"
  echo "(6) if remote database used, must set remote_user_pwd as same as mysql user pwd."
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

### get db host
get_xml_param_val ${FILE_CONF} "mysql_url"
DB_HOST="${PARAM_VAL}"
if [ "${DB_HOST}" = "" ]; then
  echo "[ERROR] can't get mysql_url from ${FILE_CONF}"
  exit 1
fi

### get db user
get_xml_param_val ${FILE_CONF} "mysql_user"
DB_USER="${PARAM_VAL}"
if [ "${DB_USER}" = "" ]; then
  echo "[ERROR] can't get mysql_user from ${FILE_CONF}"
  exit 1
fi

### get db pwd
get_xml_param_val ${FILE_CONF} "mysql_pwd"
DB_PWD="${PARAM_VAL}"
if [ "${DB_PWD}" = "" ]; then
  echo "[ERROR] can't get mysql_pwd from ${FILE_CONF}"
  exit 1
fi

### check DB_FILE
if [ ! -f $DB_FILE ];then
  echo "[ERROR] no sql file, discard! $DB_FILE"
  exit 1
fi

### confirm important operation
if [ "${AUTO_MODE}" = "auto" ]; then
	printf "\nWarning: all data in ${DB_NAME} will be deleted! Please confirm( Y/N, default is No ): Y"
else
	printf "\nWarning: all data in ${DB_NAME} will be deleted! Please confirm( Y/N, default is No ): "
	read my_input
	if [ -z $my_input ]; then
	  exit 1
	elif [ "$my_input" = "" ]; then
	  exit 1
	elif [ $my_input = "N" ]; then
	  exit 1
	elif [ $my_input = "n" ]; then
	  exit 1
	fi
fi

### load start...
echo "Started, load into ${DB_HOST}->${DB_NAME}, please wait a moment ..."

### check and create database: simserver
echo "Check and create database: ${DB_HOST}->${DB_NAME}"
mysql -h ${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -s << EOT
create database if not exists ${DB_NAME} default charset utf8 collate utf8_general_ci;
QUIT
EOT

### load from db sql
mysql -h ${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} ${DB_NAME} < ${DB_FILE}

### check and create root@127.0.0.1 for database: simserver
echo "Check and create user: ${ROOT_USER}@127.0.0.1, password: ${ROOT_PWD}"
mysql -h ${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -s << EOF
CREATE USER '${ROOT_USER}'@'127.0.0.1' IDENTIFIED BY '${ROOT_PWD}';
update mysql.user set authentication_string=password('${ROOT_PWD}') where User='${ROOT_USER}' and Host='127.0.0.1';
flush privileges;
grant all on *.* to ${ROOT_USER}@127.0.0.1;
flush privileges;
QUIT
EOF

### check and create xxxxxx@localhost for database: simserver
echo "Check and create user: ${DB_USER}@localhost, password: ${DB_PWD}"
mysql -h ${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -s << EOF
CREATE USER '${DB_USER}'@'localhost' IDENTIFIED BY '${DB_PWD}';
update mysql.user set authentication_string=password('${DB_PWD}') where User='${DB_USER}' and Host='localhost';
flush privileges;
grant all on *.* to ${DB_USER}@localhost;
flush privileges;
QUIT
EOF

### check and create xxxxxx@127.0.0.1 for database: simserver
echo "Check and create user: ${DB_USER}@127.0.0.1, password: ${DB_PWD}"
mysql -h ${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -s << EOF
CREATE USER '${DB_USER}'@'127.0.0.1' IDENTIFIED BY '${DB_PWD}';
update mysql.user set authentication_string=password('${DB_PWD}') where User='${DB_USER}' and Host='127.0.0.1';
flush privileges;
grant all on *.* to ${DB_USER}@127.0.0.1;
flush privileges;
QUIT
EOF

### check and create xxxxxx@% for database: simserver
echo "Check and create user: ${DB_USER}@%, password: ${REMOTE_PWD}"
mysql -h ${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -s << EOF
CREATE USER '${DB_USER}'@'%' IDENTIFIED BY '${DB_PWD}';
update mysql.user set authentication_string=password('${DB_PWD}') where User='${DB_USER}' and Host='%';
flush privileges;
grant all on *.* to ${DB_USER};
flush privileges;
QUIT
EOF

### update mysql proc xxxxxx@%
echo "Update mysql proc definer: database ${DB_NAME} user ${DB_USER}@%"
mysql -h ${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -s << EOF
use mysql;
select definer from proc where definer='dinstar@%' limit 3;
update proc set definer='${DB_USER}@%' where definer='dinstar@%';
select definer from proc where definer='${DB_USER}@%' limit 3;
flush privileges;
QUIT
EOF

### show databases
echo "Database List at ${DB_HOST}:"
mysql -h ${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -s << EOF
show databases;
QUIT
EOF

echo "Finished, load ${DB_HOST}->${DB_NAME} from sql file."


