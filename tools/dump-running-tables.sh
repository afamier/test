#!/bin/bash
# usage: dump-running-tables.sh <root> <password> <db_name> <simserver_running_xxxx.sql>
# readme of mysqldump
# 1. dump db definitions: 	mysqldump -uroot -p123456 -R --hex-blob --opt -d simserver > simserver_def.sql
# 2. dump db data:        	mysqldump -uroot -p123456 -R --hex-blob -t simserver > simserver_data.sql
# 3. dump db def and data:  mysqldump -uroot -p123456 -R --hex-blob simserver > simserver_all.sql
# 4. dump spec tables:      mysqldump -uroot -p123456 -R --hex-blob simserver --table tbl_version > simserver_cfg.sql

################################## VARIABLES & FUNCTIONS ######################################

FILE_CONF="/etc/dinstar/system_conf.xml"

ROOT_USER=$1
ROOT_PWD=$2
DB_NAME=$3
DB_FILE=$4

PARAM_VAL=""
DB_HOST=""
DB_RET=""
TABLE_LIST=""
DUMP_ARG="-R --hex-blob -f --default-character-set=utf8"
CUR_TIME=`date +%Y%m%d_%H%M%S`

# tables need to dump definition and data
DUMP_TABLES=(
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


get_list_by_dump_tables()
{
    #get all tables need dump
    index=0
    tbl_num=${#DUMP_TABLES[*]}

    while [ ${index} -lt ${tbl_num} ]
    do
        # add INTABLES into dump list
        table="${DUMP_TABLES[$index]}"
        if [ -n ${table} ]
        then   
            TABLE_LIST=`echo ${TABLE_LIST} ${table}`
        fi
        
        index=`expr ${index} + 1`
    done
    return 0
}


###### main() ######
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR]Operation not permitted! Please run cmd by root, or sudo cmd ..."
    exit 1
fi

if [ -z $4 ];then
    echo "Usage: $0 <root> <password> <db_name> <simserver_running_xxxx.sql>"
    exit 1
fi

# get db host
get_xml_param_val ${FILE_CONF} "mysql_url"
DB_HOST="${PARAM_VAL}"
if [ "${DB_HOST}" = "" ]; then
    echo "[ERROR] can't get mysql_url from ${FILE_CONF}"
    exit 1
fi

# check database
DB_RET=`mysql -h${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -e "show databases like '${DB_NAME}'"|egrep "^${DB_NAME}$"`
if [ "${DB_RET}" != "${DB_NAME}" ]; then    
    echo "###### ${DB_HOST}->${DB_NAME} no database found!"
    exit 1
fi

# get tables need dump
get_list_by_dump_tables
if [ -z ${TABLE_LIST} ]; then
    echo "###### ${DB_HOST}->${DB_NAME} no table list found!"
    exit 1
fi

### dump start...
echo "****** ${DB_HOST}->${DB_NAME} dump start ..."

# dump tables
echo "mysqldump -h${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -B ${DB_NAME} ${DUMP_ARG} --tables ${TABLE_LIST} > ${DB_FILE}"
mysqldump -h${DB_HOST} -u${ROOT_USER} -p${ROOT_PWD} -B ${DB_NAME} ${DUMP_ARG} --tables ${TABLE_LIST} > ${DB_FILE} 
if [ $? -ne 0 ]; then
    echo "###### ${DB_HOST}->${DB_NAME} dump running tables fail!"
    exit 1
fi
  
echo "****** ${DB_HOST}->${DB_NAME} dump running tables success!"
exit 0


