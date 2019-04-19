#! /bin/bash

################################## VARIABLES & FUNCTIONS ######################################

FILE_CONF="/etc/dinstar/system_conf.xml"
BIN_DIR="/dinstar/bin"
BAK_DIR="/provision/backup"
MAX_FILES=10

PARAM_VAL=""
DB_HOST=""
DB_USER=""
DB_PWD=""
DB_NAME=""
DB_RET=""
TABLE_LIST=""
DUMP_ARG="-n --hex-blob"
CUR_TIME=`date +%Y%m%d_%H%M%S`

# no dump tables, other tables need only to backup data without definitions
NO_DUMP_TABLES=(
    "tbl_alarm_log" \
    "tbl_cdr" \
    "tbl_ddr" \
    "tbl_ddr_tr" \
    "tbl_loc_log" \
    "tbl_log_user" \
    "tbl_mail_log" \
    "tbl_run_log" \
    "tbl_sms" \
    "tbl_sms_bill" \
    "tbl_smsr" \
    "tbl_smss" \
    "tbl_ussd" \
    "tbl_pmd_domain_15" \
    "tbl_pmd_domain_24" \
    "tbl_pmd_grp_15" \
    "tbl_pmd_grp_24" \
    "tbl_pmd_gw_15" \
    "tbl_pmd_gw_24" \
    "tbl_pmd_gwp_15" \
    "tbl_pmd_gwp_24" \
    "tbl_pmd_ne_15" \
    "tbl_pmd_ne_24" \
    "tbl_pmd_sim_15" \
    "tbl_pmd_sim_24" \
    "tbl_pmd_sys_15" \
    "tbl_pmd_sys_24" \
    "tbl_pmd_agp_15" \
    "tbl_pmd_agp_24" \
    "tbl_pmd_dsp_15" \
    "tbl_pmd_dsp_24" \
    "tbl_pmd_eth_15" \
    "tbl_pmd_eth_24" \
    "tbl_pmd_lan_15" \
    "tbl_pmd_lan_24" \
    "tbl_pmd_pri_15" \
    "tbl_pmd_pri_24" \
    "tbl_pmd_sip_15" \
    "tbl_pmd_sip_24" \
    "tbl_pmd_ss7_15" \
    "tbl_pmd_ss7_24" \
    "tbl_pmd_tgp_15" \
    "tbl_pmd_tgp_24" \
    )

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

get_list_by_no_dump_tables()
{
   # get all tables
   ALLTABLES=`mysql -h${DB_HOST} -u${DB_USER} -p${DB_PWD} -B ${DB_NAME} -e "show tables"|grep "^tbl_"`
   if [ $? -ne 0 ]
   then
      echo "###### show all tables in ${DB_NAME} fail"
      return 1
   fi
   
   if [ "${ALLTABLES}" = "" ]
   then
      echo "###### no tables in ${DB_NAME}"
      return 1
   fi
   
   #get all tables need backup
   for table in ${ALLTABLES}
   do
      #check whether table is in NO_DUMP_TABLES      
      index=0
      tbs_num=${#NO_DUMP_TABLES[*]}
      need_flag=1
      
      while [ ${index} -lt ${tbs_num} ]
      do
         if [ -n "${NO_DUMP_TABLES[$index]}" ]
         then   
            if [ "${table}" = "${NO_DUMP_TABLES[$index]}" ]
            then
               need_flag=0
               break;
            fi
         fi
        
         index=`expr ${index} + 1`
      done
      
      if [ ${need_flag} -eq 1 ]
      then
         TABLE_LIST=`echo ${TABLE_LIST} ${table}`
      fi
   done
   
   return 0
}

rotate_backup_files()
{   
   #cp old.n to older.m
   index=`expr ${MAX_FILES} - 1`
   while [ ${index} -gt 0 ]
   do
      src=${BAK_DIR}/${DB_NAME}_backup.sql.$((${index} - 1))
      dst=${BAK_DIR}/${DB_NAME}_backup.sql.${index}
      
      if [ -e "${src}" ]
      then   
         mv ${src} ${dst}
         if [ $? -ne 0 ]
         then
            echo "###### rotate ${src} to ${dst} fail"
            return 1
         fi
      fi
        
      index=`expr ${index} - 1`
   done
   
   #cp new to old.0
   src=${BAK_DIR}/${DB_NAME}_backup.sql
   dst=${BAK_DIR}/${DB_NAME}_backup.sql.0
   
   if [ -e "${src}" ]
   then   
      mv ${src} ${dst}
      if [ $? -ne 0 ]
      then
         echo "###### rotate ${src} to ${dst} fail"
         return 1
      fi
   fi
   
   return 0
} 

###### main() ######
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR]Operation not permitted! Please run cmd by root, or sudo cmd ..."
    exit 1
fi

if [ ! -d ${BAK_DIR} ]; then
  echo "create backup dir: ${BAK_DIR}"
  mkdir -p ${BAK_DIR}
fi

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

### get db name
get_xml_param_val ${FILE_CONF} "mysql_db"
DB_NAME="${PARAM_VAL}"
if [ "${DB_NAME}" = "" ]; then
  echo "[ERROR] can't get mysql_db from ${FILE_CONF}"
  exit 1
fi

# backup database file
DB_RET=`mysql -h${DB_HOST} -u${DB_USER} -p${DB_PWD} -e "show databases like '${DB_NAME}'"|egrep "^${DB_NAME}$"`
if [ "${DB_RET}" != "${DB_NAME}" ]; then    
  echo "****** db ${DB_NAME} backup fail!"
  exit 1
fi

# get tables need backup
get_list_by_no_dump_tables
if [ $? -ne 0 ]; then
    exit 1
fi

#todo backup
mysqldump -h${DB_HOST} -u${DB_USER} -p${DB_PWD} -B ${DB_NAME} ${DUMP_ARG} --tables ${TABLE_LIST} >${BAK_DIR}/${DB_NAME}_backup.sql
if [ $? -ne 0 ]; then
  exit 1
fi
  
# chmod backup files
chmod 600 ${BAK_DIR}/${DB_NAME}_backup.sql

# rotate backup files
rotate_backup_files
if [ $? -ne 0 ]; then
    exit 1
fi


ls -lhst --full-time ${BAK_DIR}

echo "****** ${DB_NAME} Backup success!"
exit 0

