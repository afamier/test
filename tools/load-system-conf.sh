#!/bin/sh
# -------------------------------------------------------------------------
# load /etc/dinstar/system_conf.xml into simserver
# all server env setting at here!
# -------------------------------------------------------------------------
# $FILE_CONF keywords:
# {SYS_UUID}            --- 01. system uuid
# {SYS_ALIAS}           --- 02. system alias
# {CLOUD_UUID}          --- 03. system cloud uuid
# {SYS_PUB_IP}          --- 04. system assigned public IP
# {STUN_LISTEN_ETH}     --- 05  stun server listen eth
# {STUN_LISTEN_2ND}     --- 06  stun server listen optional
# {TRANS_LISTEN_PORT}   --- 07. trans server listen port
# {WEB_PROXY_PORT}      --- 08. remote-web proxy port
# {TELNET_PROXY_PORT}   --- 09. remote-telnet proxy port
# {SERVICE_API_PORT}    --- 10. service api port
# {DB_URL}              --- 11. mysql url/ip
# {DB_PORT}             --- 12. mysql port
# {DB_NAME}             --- 13. mysql database
# {DB_USER}             --- 14. mysql username
# {DB_PASSWORD}         --- 15. mysql password
# {DB_LOG_SPLITS}       --- 16. mysql log table split days
# {DB_PMD_SPLITS}       --- 17.  mysql pmd table split days
# {DB_PRE_SPLITS}       --- 18.  mysql pmd table split days
# {FTP_SERVER}          --- 19. provision url/ip
# {FTP_USER}            --- 20. provision username
# {FTP_PASSWORD}        --- 21. provision password
# {DNS_SERVER01}        --- 22. auth server url/ip, 0212
# {DNS_SERVER02}        --- 23. auth server url/ip, 0212
# {DNS_DOMAIN01}        --- 24. dns domain 01, 0308
# {DNS_DOMAIN02}        --- 25. dns domain 02, 0308
# {TELNET_MAX_CONNECT}  --- 26. max telnet connect, 0312
# {MAX_SMS_QUEUE_SIZE}  --- 27. max sms queue size, 0322
# -------------------------------------------------------------------------
# Notes: SYS_ETH_IP, get setting of STUN_LISTEN_ETH.
#        SYS_2ND_IP, get setting of STUN_LISTEN_2ND.
# -------------------------------------------------------------------------

# 12.04, 14.04, 16.04
RELEASE=`lsb_release -r|cut -f 2`
# i386, x86_64
ARCH=`uname -i`

FILE_CONF="/etc/dinstar/system_conf.xml"

APP_DIR="/dinstar"
RESTUND_DIR="/dinstar/app-restund"
if  [ "$RELEASE" = "12.04" ]
then
    WEBAPPS_DIR="/var/lib/tomcat6/webapps"
elif [ "$RELEASE" = "14.04" ]
then
    WEBAPPS_DIR="/var/lib/tomcat6/webapps"
elif [ "$RELEASE" = "16.04" ]
then
    WEBAPPS_DIR="/var/lib/tomcat7/webapps"
fi

TMP_SED_RULE="/tmp/my_sed_rule"
TMP_SED_FILE="/tmp/my_sed_file.tmp"
PARAM_VAL=""

SYS_UUID=""
SYS_ALIAS=""
CLOUD_UUID=""
SYS_PUB_IP=""
SYS_ETH_IP=""
SYS_2ND_IP=""
STUN_LISTEN_ETH=""
STUN_LISTEN_2ND=""
TRANS_LISTEN_PORT=""
WEB_PROXY_PORT=""
TELNET_PROXY_PORT=""
SERVICE_API_PORT=""
DB_URL=""
DB_PORT=""
DB_NAME=""
DB_USER=""
DB_PASSWORD=""
DB_LOG_SPLITS=""
DB_PMD_SPLITS=""
DB_PRE_SPLITS=""
FTP_SERVER=""
FTP_USER=""
FTP_PASSWORD=""
DNS_SERVER01=""
DNS_SERVER02=""
DNS_DOMAIN01=""
DNS_DOMAIN02=""
TELNET_MAX_CONNECT=""
MAX_SMS_QUEUE_SIZE=""
TOMCAT_VER=""

BACKUP_CONFIG2="/dinstar/cfg/syslog-ng.conf"
SIMSRV_CONFIG2="/etc/syslog-ng/syslog-ng.conf"

BACKUP_CONFIG3="/dinstar/cfg/syslog-ng"
SIMSRV_CONFIG3="/etc/logrotate.d/syslog-ng"

BACKUP_CONFIG4="/dinstar/cfg/logrotate-simsrv"
SIMSRV_CONFIG4="/etc/logrotate.d/logrotate-simsrv"

BACKUP_CONFIG9="/dinstar/cfg/named.conf.options"
SIMSRV_CONFIG9="/etc/bind/named.conf.options"

DEFAULT_CONFIG10="/dinstar/cfg/named.conf.local"
BACKUP_CONFIG10="/etc/dinstar/named.conf.local"
SIMSRV_CONFIG10="/etc/bind/named.conf.local"

BACKUP_CONFIG11="/etc/dinstar/db.dns_domain01"
SIMSRV_CONFIG11="/etc/bind/db.dns_domain01"

BACKUP_CONFIG12="/etc/dinstar/db.dns_domain02"
SIMSRV_CONFIG12="/etc/bind/db.dns_domain02"


# -------------------------------------------------
# get one param value from xml file:
#     <param name="param01" value="1"/>
# param_name MUST be unique one
# return: PARAM_VAL
# -------------------------------------------------
get_xml_param_val()
{
    local my_file=$1
    local my_param=$2

    PARAM_VAL=`cat $my_file | grep $my_param | awk '{print $3}' | cut -f 2 -d '"'`
}

# -------------------------------------------------
# replace keys in one file
# usage: replace_keys <filename> <key> <value>
# -------------------------------------------------
replace_keys()
{
  local my_file=$1
  local my_param=$2
  local my_value=$3
  local my_value2=""
  
  # patch for special char!!!
  my_value2=$(echo $my_value | sed -e 's/\//\\\//g')

  rm -f ${TMP_SED_RULE}
  rm -f ${TMP_SED_FILE}

  echo "s/$my_param/$my_value2/g" > ${TMP_SED_RULE}

  ###echo "sed -f ${TMP_SED_RULE} $my_file > ${TMP_SED_FILE}"
  sed -f ${TMP_SED_RULE} $my_file > ${TMP_SED_FILE}

  if [ -e ${TMP_SED_FILE} ]; then
    tmpSize=`du -sb ${TMP_SED_FILE} | awk '{print $1}'`
    if [ ${tmpSize} -ne 0 ]; then
      cp -f ${TMP_SED_FILE} $my_file
      rm -f ${TMP_SED_FILE}
    else
      echo "[ERROR] $my_file"
    fi
  else
    echo "[ERROR] $my_file"
  fi
}

# check and recover conf file
recover_config()
{
    if [ ! -f $1 ]; then
      echo "no system conf: $1"
      echo [`date +"%Y-%m-%d %T"`]"[script]no system conf: $1" | logger
      return;
    fi

    # recover system conf
    echo "old system conf: $tempvar"
    echo [`date +"%Y-%m-%d %T"`]"[script]old system conf: $tempvar" | logger

    echo "cp -f $1 $2"
    echo [`date +"%Y-%m-%d %T"`]"[script]cp -f $1 $2" | logger
    cp -f $1 $2
}

#check and recover system conf files
recover_config_all()
{
    recover_config $BACKUP_CONFIG2 $SIMSRV_CONFIG2
    recover_config $BACKUP_CONFIG3 $SIMSRV_CONFIG3
    recover_config $BACKUP_CONFIG4 $SIMSRV_CONFIG4
    recover_config $BACKUP_CONFIG9 $SIMSRV_CONFIG9

    ### dns is optional
    if [ ! -f $BACKUP_CONFIG10 ]; then
        cp $DEFAULT_CONFIG10 $BACKUP_CONFIG10
    fi
    recover_config $BACKUP_CONFIG10 $SIMSRV_CONFIG10
    
    # db.dns_domain01 --- recover only once
    if [ ! -f $SIMSRV_CONFIG11 ]; then
        recover_config $BACKUP_CONFIG11 $SIMSRV_CONFIG11
    fi

    # db.dns_domain02 --- recover only once
    if [ ! -f $SIMSRV_CONFIG12 ]; then
        recover_config $BACKUP_CONFIG12 $SIMSRV_CONFIG12
    fi
}


# -------------------------------------------------
# get one param value from tomcat:
# return: PARAM_VAL
# -------------------------------------------------
get_tomcat_ver()
{
  local ver1=`dpkg -l | grep tomcat6 | awk '{if(NR==1)print}' | awk '{print $2}'`
  local ver2=`dpkg -l | grep tomcat7 | awk '{if(NR==1)print}' | awk '{print $2}'`
  local ver3=`dpkg -l | grep tomcat8 | awk '{if(NR==1)print}' | awk '{print $2}'`

  if [ ! -z "$ver1" ]; then
    PARAM_VAL="tomcat6"
  elif [ ! -z "$ver2" ]; then
    PARAM_VAL="tomcat7"
  elif [ ! -z "$ver3" ]; then
    PARAM_VAL="tomcat8"
  fi
}


###### main() ######
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR]Operation not permitted! Please run cmd by root, or sudo cmd ..."
    exit 1
fi

printf "Load system conf file: %s ... \n" $FILE_CONF

### step 1: check backup config.xml file
if [ ! -e ${FILE_CONF} ]; then
  printf "ERROR! No system conf file: %s \n" $FILE_CONF
  exit 1
fi

### get system conf info
get_xml_param_val $FILE_CONF "sys_uuid"
SYS_UUID=$PARAM_VAL

get_xml_param_val $FILE_CONF "sys_alias"
SYS_ALIAS=$PARAM_VAL

get_xml_param_val $FILE_CONF "cloud_uuid"
CLOUD_UUID=$PARAM_VAL

get_xml_param_val $FILE_CONF "stun_listen_eth"
STUN_LISTEN_ETH=$PARAM_VAL
SYS_ETH_IP=`ifconfig $STUN_LISTEN_ETH | grep "inet addr:" | awk '{if(NR==1)print}' | awk '{print $2}' | cut -f 2 -d ':'`
if [ "${SYS_ETH_IP}" = "" ]; then
  printf "\n"
  printf "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n"
  printf "!  ERROR! wrong stun_listen_eth: %s, not exists \n" ${STUN_LISTEN_ETH}
  printf "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n"

  logger "[script]ERROR! wrong stun_listen_eth: ${STUN_LISTEN_ETH}, not exists"
  exit 1
fi

get_xml_param_val $FILE_CONF "stun_listen_2nd"
STUN_LISTEN_2ND=$PARAM_VAL
if [ "${STUN_LISTEN_2ND}" = "null" ]; then
  SYS_2ND_IP="0.0.0.0"
elif [ "${STUN_LISTEN_2ND}" = "-" ]; then
  SYS_2ND_IP="0.0.0.0"
else
  SYS_2ND_IP=`ifconfig $STUN_LISTEN_2ND | grep "inet addr:" | awk '{if(NR==1)print}' | awk '{print $2}' | cut -f 2 -d ':'`
  if [ "${SYS_2ND_IP}" = "" ]; then
    printf "\n"
    printf "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n"
    printf "!  ERROR! wrong stun_listen_2nd: %s, not exists \n" ${STUN_LISTEN_2ND}
    printf "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n"

	logger "[script]ERROR! wrong stun_listen_2nd: ${STUN_LISTEN_2ND}, not exists \n"
    exit 1
  fi
fi

get_xml_param_val $FILE_CONF "sys_pub_ip"
SYS_PUB_IP=$PARAM_VAL
if [ "${SYS_PUB_IP}" = "0.0.0.0" ]; then
  SYS_PUB_IP="${SYS_ETH_IP}"
fi

get_xml_param_val $FILE_CONF "trans_listen_port"
TRANS_LISTEN_PORT=$PARAM_VAL

get_xml_param_val $FILE_CONF "web_proxy_port"
WEB_PROXY_PORT=$PARAM_VAL

get_xml_param_val $FILE_CONF "telnet_proxy_port"
TELNET_PROXY_PORT=$PARAM_VAL

get_xml_param_val $FILE_CONF "service_api_port"
SERVICE_API_PORT=$PARAM_VAL

get_xml_param_val $FILE_CONF "mysql_url"
DB_URL=$PARAM_VAL

get_xml_param_val $FILE_CONF "mysql_port"
DB_PORT=$PARAM_VAL

get_xml_param_val $FILE_CONF "mysql_db"
DB_NAME=$PARAM_VAL

get_xml_param_val $FILE_CONF "mysql_db"
DB_NAME=$PARAM_VAL

get_xml_param_val $FILE_CONF "mysql_user"
DB_USER=$PARAM_VAL

get_xml_param_val $FILE_CONF "mysql_pwd"
DB_PASSWORD=$PARAM_VAL

get_xml_param_val $FILE_CONF "mysql_log_splits"
DB_LOG_SPLITS=$PARAM_VAL

get_xml_param_val $FILE_CONF "mysql_pmd_splits"
DB_PMD_SPLITS=$PARAM_VAL

get_xml_param_val $FILE_CONF "mysql_pre_splits"
DB_PRE_SPLITS=$PARAM_VAL

get_xml_param_val $FILE_CONF "prov_url"
FTP_SERVER=$PARAM_VAL

get_xml_param_val $FILE_CONF "prov_user"
FTP_USER=$PARAM_VAL

get_xml_param_val $FILE_CONF "prov_pwd"
FTP_PASSWORD=$PARAM_VAL

get_xml_param_val $FILE_CONF "dns_url01"
DNS_SERVER01=$PARAM_VAL

get_xml_param_val $FILE_CONF "dns_url02"
DNS_SERVER02=$PARAM_VAL

get_xml_param_val $FILE_CONF "dns_domain01"
DNS_DOMAIN01=$PARAM_VAL

get_xml_param_val $FILE_CONF "dns_domain02"
DNS_DOMAIN02=$PARAM_VAL

get_xml_param_val $FILE_CONF "telnet_max_connect"
TELNET_MAX_CONNECT=$PARAM_VAL

get_xml_param_val $FILE_CONF "max_sms_queue_size"
MAX_SMS_QUEUE_SIZE=$PARAM_VAL

get_tomcat_ver
TOMCAT_VER="${PARAM_VAL}"


### default related conf files: xxx_default.yyy
### running related conf files: xxx.yyy, cut off "_default"
default_list="${WEBAPPS_DIR}/web/WEB-INF/classes/database_default.properties \
              ${WEBAPPS_DIR}/web/WEB-INF/classes/simcloud_default.properties \
              ${WEBAPPS_DIR}/web/WEB-INF/classes/log4j_default.properties \
              ${APP_DIR}/cfg/config_default \
              ${APP_DIR}/cfg/named.conf.local_default \
              ${APP_DIR}/cfg/db.dns_domain01_default \
              ${APP_DIR}/cfg/db.dns_domain02_default \
             "

### import system conf info into related conf files
my_default=""
my_running=""
for my_default in ${default_list}
do
    printf "deal with : %s ... \n" ${my_default}
    my_running=`echo $my_default | sed "s/_default//g"`
    printf "copy to   : %s \n" ${my_running}
    cp $my_default $my_running
    chmod 644 $my_running
    replace_keys "${my_running}" "{SYS_UUID}" "${SYS_UUID}"
    replace_keys "${my_running}" "{SYS_ALIAS}" "${SYS_ALIAS}"
    replace_keys "${my_running}" "{CLOUD_UUID}" "${CLOUD_UUID}"
    replace_keys "${my_running}" "{SYS_PUB_IP}" "${SYS_PUB_IP}"
    replace_keys "${my_running}" "{SYS_ETH_IP}" "${SYS_ETH_IP}"
    replace_keys "${my_running}" "{SYS_2ND_IP}" "${SYS_2ND_IP}"
    replace_keys "${my_running}" "{STUN_LISTEN_ETH}" "${STUN_LISTEN_ETH}"
    replace_keys "${my_running}" "{STUN_LISTEN_2ND}" "${STUN_LISTEN_2ND}"
    replace_keys "${my_running}" "{TRANS_LISTEN_PORT}" "${TRANS_LISTEN_PORT}"
    replace_keys "${my_running}" "{WEB_PROXY_PORT}" "${WEB_PROXY_PORT}"
    replace_keys "${my_running}" "{TELNET_PROXY_PORT}" "${TELNET_PROXY_PORT}"
    replace_keys "${my_running}" "{SERVICE_API_PORT}" "${SERVICE_API_PORT}"
    replace_keys "${my_running}" "{DB_URL}" "${DB_URL}"
    replace_keys "${my_running}" "{DB_PORT}" "${DB_PORT}"
    replace_keys "${my_running}" "{DB_NAME}" "${DB_NAME}"
    replace_keys "${my_running}" "{DB_USER}" "${DB_USER}"
    replace_keys "${my_running}" "{DB_PASSWORD}" "${DB_PASSWORD}"
    replace_keys "${my_running}" "{DB_LOG_SPLITS}" "${DB_LOG_SPLITS}"
    replace_keys "${my_running}" "{DB_PMD_SPLITS}" "${DB_PMD_SPLITS}"
    replace_keys "${my_running}" "{DB_PRE_SPLITS}" "${DB_PRE_SPLITS}"
    replace_keys "${my_running}" "{FTP_SERVER}" "${FTP_SERVER}"
    replace_keys "${my_running}" "{FTP_USER}" "${FTP_USER}"
    replace_keys "${my_running}" "{FTP_PASSWORD}" "${FTP_PASSWORD}"
    replace_keys "${my_running}" "{DNS_SERVER01}" "${DNS_SERVER01}"
    replace_keys "${my_running}" "{DNS_SERVER02}" "${DNS_SERVER02}"
    replace_keys "${my_running}" "{DNS_DOMAIN01}" "${DNS_DOMAIN01}"
    replace_keys "${my_running}" "{DNS_DOMAIN02}" "${DNS_DOMAIN02}"
    replace_keys "${my_running}" "{TELNET_MAX_CONNECT}" "${TELNET_MAX_CONNECT}"
    replace_keys "${my_running}" "{MAX_SMS_QUEUE_SIZE}" "${MAX_SMS_QUEUE_SIZE}"
    replace_keys "${my_running}" "{TOMCAT_VER}" "${TOMCAT_VER}"
done


# check and update system conf files
recover_config_all


printf "*** SUCCESS! Loaded system conf file: %s \n" $FILE_CONF


