#! /bin/bash
#  DB_FILE="/dinstar/cfg/simserver_local.sql"
################################## VARIABLES & FUNCTIONS ######################################

FILE_CONF="/etc/dinstar/system_conf.xml"
BIN_DIR="/dinstar/bin"

ROOT_USER=$1
ROOT_PWD=$2
REMOTE_PWD=$3
DB_NAME=$4
DB_FILE=$5
UPDATE_MODE=$6

TEMP_DB="${DB_NAME}_temp"

PARAM_VAL=""
DB_HOST=""
DB_USER=""
DB_PWD=""

INITABLES=( "tbl_alarm_desc" \
            "tbl_cause_desc" \
            "tbl_country" \
            "tbl_enum_def" \
            "tbl_grp_tmp" \
            "tbl_gsm_code" \
            "tbl_mcc_list" \
            "tbl_module" \
            "tbl_network_code" \
            "tbl_persist" \
            "tbl_pkg_tmp" \
            "tbl_product_type" \
            "tbl_vendor" \
            "tbl_province" \
            "tbl_city" \
            "tbl_district" \
            "tbl_band" \
          )

if [ -z $5 ];then
  echo "Usage: $0 <root> <password> <remote_user_pwd> <db_name> <simserver_local_xxxx.sql> [force]"
  echo "notes:"
  echo "(1) root            - mysql root user."
  echo "(2) password        - mysql root password."
  echo "(3) remote_user_pwd - mysql user remote password."
  echo "(4) db_name         - default is simserver."
  echo "(5) force           - clear tbl_alarm for index conflicted issue."
  echo "(6) simserver_local_xxxx.sql, default file is /dinstar/cfg/simserver_local.sql"
  echo "(7) if remote database used, must set remote_user_pwd as same as mysql user pwd."
  echo "(8) database update tool depends on 'schemasync', please install it from provision server."
  exit 1
fi

###### main() ######
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR]Operation not permitted! Please run cmd by root, or sudo cmd ..."
    exit 1
fi


# simsrv_data.sql: only all tables data
DATA_ARG="-tcn --replace"

# which tables initial data has been changed
DATA_TBS="--tables ${INITABLES[*]}"

# simsrv_procedures.sql: only events and procedures
PROCED_ARG="-tdER"

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

backup_old_database()
{
   #check if database is not exist
   command=`mysql -h${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -e "show databases like '${DB_NAME}'"|egrep "^${DB_NAME}$"`

   if [ "$command" != "${DB_NAME}" ]
   then
       echo "****** db ${DB_NAME} doesn't exist please run \" load-simserver-sql.sh\""
       return 1
   fi

   echo "****** backup ${DB_NAME} will take a long time, please wait! "
   #todo backup
   ${BIN_DIR}/dump-simserver-sql.sh ${ROOT_USER} ${ROOT_PWD} ${DB_NAME} ./${DB_NAME}_backup_`date +%Y%m%d_%H%M%S`.sql
   if [ $? -ne 0 ]
   then
      return 1
   fi

   return 0
}

fine_tuning_on_database()
{
   #strip index in some special tables.
   ${BIN_DIR}/strip_index_0212.sh ${ROOT_USER} ${ROOT_PWD} ${DB_NAME} ${DB_HOST}
   if [ $? -ne 0 ]
   then
      echo "###### run ${BIN_DIR}/strip_index_0212.sh fail"
      return 1
   else
      echo "****** run ${BIN_DIR}/strip_index_0212.sh success"
   fi

   # clear tbl_alarm for index conflicted issue!
   if [ "${UPDATE_MODE}" = "force" ]
   then
       COMMAND="truncate tbl_alarm"
       mysql -h${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -B ${DB_NAME} -e "$COMMAND"
       if [ $? -ne 0 ]
       then
           echo "###### run $COMMAND fail"
           return 1
       else
           echo "****** force tuning on db ${DB_NAME} success"
       fi
   fi

   return 0
}

drop_initial_tables()
{
   tbs_num=${#INITABLES[*]}

   index=0
   while [ ${index} -lt ${tbs_num} ]
   do
      if [ -n "${INITABLES[$index]}" ]
      then
         COMMAND="DROP TABLE IF EXISTS ${INITABLES[$index]}"
         mysql -h${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -B ${DB_NAME} -e "$COMMAND"
         if [ $? -ne 0 ]
         then
            echo "###### run $COMMAND fail"
            return 1
         fi
      fi

      index=`expr ${index} + 1`
   done

   return 0
}

import_simserver_temp()
{
   #drop old simserver_local if exists

   if [ "${TEMP_DB}" == "${DB_NAME}" ]
   then
      echo "###### temp db(${TEMP_DB}) is equal to target db(${DB_NAME})"
      return 1
   fi

   COMMAND="DROP DATABASE IF EXISTS ${TEMP_DB}"
   mysql -h${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -e "$COMMAND"
   if [ $? -ne 0 ]
   then
      echo "###### run $COMMAND fail"
      return 1
   fi

   #create new simserver_local
   COMMAND="CREATE DATABASE ${TEMP_DB}"
   mysql -h${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -e "$COMMAND"
   if [ $? -ne 0 ]
   then
      echo "###### run $COMMAND fail"
      return 1
   fi

   #import simserver_local
   mysql -h${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} ${TEMP_DB} < ${DB_FILE}
   if [ $? -ne 0 ]
   then
      echo "###### run ${DB_FILE} fail"
      return 1
   fi

   return 0
}

dump_initial_data_temp()
{
   #dump initial data
   mysqldump -h${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} --hex-blob -B ${TEMP_DB} $DATA_ARG $DATA_TBS|sed 's/ @@/ @/g ' >/tmp/simsrv_data.sql
   if [ $? -ne 0 ]
   then
       echo "###### Creat ${TEMP_DB} tables' data error!"
       return 1
   fi

   sed -i "s/^USE \`${TEMP_DB}\`/USE \`${DB_NAME}\`/" simsrv_data.sql
   if [ $? -ne 0 ]
   then
       echo "###### replace from ${TEMP_DB} to ${DB_NAME} fail"
       return  1
   fi
   return 0
}

dump_all_procedures_temp()
{
   #dump all procedures
   mysqldump -h${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} --hex-blob -B ${TEMP_DB} $PROCED_ARG|sed  's/DEFINER=`dinstar`@`%`//g ' >/tmp/simsrv_procedures.sql
   if [ $? -ne 0 ]
   then
       echo "###### Creat dump procedures from ${TEMP_DB} error!"
       return 1
   fi

   sed -i "s/^USE \`${TEMP_DB}\`/USE \`${DB_NAME}\`/" simsrv_procedures.sql
   if [ $? -ne 0 ]
   then
       echo "###### replace from ${TEMP_DB} to ${DB_NAME} fail"
       return  1
   fi
   return 0
}


drop_db_temp()
{
  mysql -h${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -e "drop database ${TEMP_DB}"
  if [ $? -ne 0 ]
  then
      echo -e "\033[1m\c"
      echo "###### drop database ${TEMP_DB} fail"
      echo "Please delete database \"${TEMP_DB}\" by yourself after install!"
      sleep 5
      return  1
  fi

  return 0
}

fix_patch_script()
{
  SRC_SQL="$1"
  DST_SQL="$2"

  awk '
  BEGIN{
   FS = ","
#   print "FS:" FS
  }

  ($1 ~ /^ALTER TABLE `[a-zA-Z0-9_]*`/) && (NF >= 2) {
    OFS="\n"
    ret = match($1,/^ALTER TABLE `[a-zA-Z0-9_-]*`/)
    if(ret == 0)
    {
      print $0
      next
    }
    prefix = substr($1,RSTART,RLENGTH)
#    print "prefix:" prefix
    tmpRowStr = substr($1,RSTART+RLENGTH,length($1)-(RLENGTH+RSTART)+1)
#    print "tmpRowStr:" tmpRowStr

    for(i=2; i<=NF; i++)
    {
       ret = match($i,/^ ADD|DROP|MODIFY COLUMN `[a-zA-Z0-9_-]*`/)
       if(ret == 0)
       {
#         this field belongs to current Row
          tmpRowStr = tmpRowStr "," $i
       }
       else
       {
#         output old Row & create new Row
          print prefix tmpRowStr ";"
          tmpRowStr = $i
       }
    }
#   output the last field ,it belongs to new Row and has a ";" already.
    print  prefix tmpRowStr
    next
  }

  1 {print $0}
  ' "$SRC_SQL" >"$DST_SQL"
}

simserver_schemasync()
{
   #del old patch file
   rm -f ${DB_NAME}_sync.*.*.sql

   schemasync --tag="sync" mysql://${ROOT_USER}:${ROOT_PWD}@${DB_HOST}:3306/${TEMP_DB} mysql://${ROOT_USER}:${ROOT_PWD}@${DB_HOST}:3306/${DB_NAME}
   if [ $? -ne 0 ]
   then
       echo "###### schemasync from ${DB_NAME} to ${TEMP_DB} fail"
       return 1
   fi

   PATCH="${DB_NAME}_sync.`date +%Y%m%d`.patch.sql"
   if [ ! -e "$PATCH" ]
   then
       echo "****** ${TEMP_DB} already is up-to-date"
       #drop_db_temp
       return 0
   fi

   TMP_PATCH="${DB_NAME}_patch.sql"

   if [ ! -e "$TMP_PATCH" ]
   then
       touch $TMP_PATCH
       if [ $? -ne 0 ]
       then
           echo "###### touch $TMP_PATCH fail"
           return  1
       fi

       chmod 0777 $TMP_PATCH
       if [ $? -ne 0 ]
       then
           echo "###### chmod $TMP_PATCH fail"
           return  1
       fi
   fi

#  7£¨awk fix the patck
   fix_patch_script $PATCH $TMP_PATCH
   if [ $? -ne 0 ]
   then
       echo "###### run ./convert_sql.sh $PATCH $TMP_PATCH fail"
       return  1
   fi

#  8£¨load the patch into simserver
   mysql -h${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -B ${DB_NAME} < $TMP_PATCH
   if [ $? -ne 0 ]
   then
       echo "###### mysql -h${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -B ${DB_NAME} < $TMP_PATCH fail"
       return  1
   fi

   return 0
}

create_simserver_procedures()
{
  #create procedures from sql file
  sed -i '/SET @@/d ' ./simsrv_procedures.sql
  mysql -h${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -B ${DB_NAME} < ./simsrv_procedures.sql
  if [ $? -ne 0 ]
  then
      echo "###### cread procedure from ./simsrv_procedures.sql error"
      return 1
  fi

  return 0
}

################################## MAIN BODY ##############################################

rm -f /tmp/simsrv_data.sql
rm -f /tmp/simsrv_procedures.sql


### get db host
get_xml_param_val ${FILE_CONF} "mysql_url"
DB_HOST="${PARAM_VAL}"
if [ "${DB_HOST}" = "" ]; then
  echo "[ERROR] can't get mysql_url from ${FILE_CONF}"
  exit 1
fi

# don't allow at rds.dimiccs.com
if [ "${DB_HOST}" = "rds.dimiccs.com" ]; then
  echo "[ERROR] don't allow at rds.dimiccs.com! please update rds manually."
  exit 1
fi
if [ "${DB_HOST}" = "rds.changyoumifi.com" ]; then
  echo "[ERROR] don't allow at rds.dimiccs.com! please update rds manually."
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
if [ ! -f ${DB_FILE} ];then
  echo "[ERROR] no sql file, discard! ${DB_FILE}"
  exit 1
fi


#1, backup the old database
backup_old_database
if [ $? -ne 0 ]
then
    echo "###### backup db ${DB_NAME} fail"
    exit 1
else
    echo "****** backup db ${DB_NAME} success"
fi

#2, special operation on old db before updating for new structures.
fine_tuning_on_database
if [ $? -ne 0 ]
then
    echo "###### fine tuning on db ${DB_NAME} fail"
    exit 1
else
    echo "****** fine tuning on db ${DB_NAME} success"
fi

drop_initial_tables
if [ $? -ne 0 ]
then
    echo "###### drop initail tables on db ${DB_NAME} fail"
    exit 1
else
    echo "****** drop initail tables on db ${DB_NAME} success"
fi

#3, import simserver_local.sql >> ${TEMP_DB}
import_simserver_temp
if [ $? -ne 0 ]
then
    echo "###### import '${TEMP_DB}' fail"
    exit 1
else
    echo "****** import '${TEMP_DB}' success"
fi

#4, dump procedures functions >>simserver_produces.sql
dump_all_procedures_temp
if [ $? -ne 0 ]
then
    echo "###### dump all procedures from '${TEMP_DB}' fail"
    exit 1
else
    echo "****** dump all procedures from '${TEMP_DB}' success"
fi


#5, dump special tables' data >>simserver_data.sql  ”√--replace—°œÓ
dump_initial_data_temp
if [ $? -ne 0 ]
then
    echo "###### dump initial data from '${TEMP_DB}' fail"
    exit 1
else
    echo "****** dump initial data from '${TEMP_DB}' success"
fi

#6£¨schemasync compare simserver and simerver_local, than create the patch£¨
#   the signle quote in comments can bring out bug in patch,so we don't update the comments defaultly
simserver_schemasync
if [ $? -ne 0 ]
then
    echo "###### schemasync fail"
    exit 1
else
    echo "***** schemasync success"
fi

#9£¨load simserver_data.sql into simserver
mysql -h${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -B ${DB_NAME} < ./simsrv_data.sql
if [ $? -ne 0 ]
then
    echo "###### import data for simsrv db fail"
    exit 1
else
    echo "****** import initial data success"
fi

#10£¨load simserver_procedures.sql into simserver
create_simserver_procedures
if [ $? -ne 0 ]
then
    echo "###### call create_simserver_procedures() fail"
    exit 1
fi

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
mysql -h 127.0.0.1 -u${ROOT_USER} -p${ROOT_PWD} -s << EOF
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

echo "****** Update Whole '${DB_NAME}' Success!"
exit 0
