#!/bin/bash
# usage: delete-server-license.sh
# delete local server license in db, and try to reload license license.xml after server restarted!

FILE_CONF="/etc/dinstar/system_conf.xml"

SYS_UUID=""
DB_HOST=""
DB_NAME=""
DB_USER=""
DB_PWD=""

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

### get sys uuid
get_xml_param_val ${FILE_CONF} "sys_uuid"
SYS_UUID="${PARAM_VAL}"
if [ "${SYS_UUID}" = "" ]; then
  echo "[ERROR] can't get sys_uuid from ${FILE_CONF}"
  exit 1
fi

### get db host
get_xml_param_val ${FILE_CONF} "mysql_url"
DB_HOST="${PARAM_VAL}"
if [ "${DB_HOST}" = "" ]; then
  echo "[ERROR] can't get mysql_url from ${FILE_CONF}"
  exit 1
fi

### get db name
get_xml_param_val ${FILE_CONF} "mysql_db"
DB_NAME="${PARAM_VAL}"
if [ "${DB_NAME}" = "" ]; then
  echo "[ERROR] can't get mysql_db from ${FILE_CONF}"
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

### show server license in db
printf "\nserver license:{serial_no,spec_sys_uuid,srv_domain,type,max_sim_card,hbm_features}\n"
mysql -h ${DB_HOST} -u${DB_USER} -p${DB_PWD} -s << EOT
use ${DB_NAME};
select serial_no,spec_sys_uuid,srv_uuid,srv_domain,sign_type,max_sim_card,hbm_features from tbl_lic_srv where spec_sys_uuid=${SYS_UUID};
QUIT
EOT

### confirm important operation
printf "\nWarning: delete server license in ${DB_HOST}.${DB_NAME}, please confirm( Y/N, default is No ): "
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

### delete server license in db
mysql -h ${DB_HOST} -u${DB_USER} -p${DB_PWD} -s << EOT
use ${DB_NAME};
delete from tbl_lic_srv where spec_sys_uuid=${SYS_UUID};
QUIT
EOT

echo "Finished, please restart server to reload license.xml"


