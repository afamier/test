#/!/bin/sh
# stop /dinstar/bin/simsrv

# 12.04, 14.04, 16.04
RELEASE=`lsb_release -r|cut -f 2`
# i386, x86_64
ARCH=`uname -i`

SIMSRV_BIN="simsrv"
DNS_BIN="bind9"
SYSLOG_BIN="syslog-ng"

if  [ "$RELEASE" = "12.04" ]
then
    WEBAPPS_BIN="tomcat6"
elif [ "$RELEASE" = "14.04" ]
then
    WEBAPPS_BIN="tomcat6"
elif [ "$RELEASE" = "16.04" ]
then
    WEBAPPS_BIN="tomcat7"
fi

DAEMON_BIN="simsafe"


# echo and logger info
# [`date +"%Y-%m-%d %T"`], use syslog timestamp
do_logger()
{
  local my_info="$1"  
  echo "[simsrv] ${my_info}"
  echo "[simsrv] ${my_info}" | logger
}

# check and stop webapps
stop_webapps()
{
    local tempvar="`service ${WEBAPPS_BIN} status | grep "is running"`"

    do_logger "old ${WEBAPPS_BIN}: ${tempvar}"

    if [ -z "${tempvar}" ]; then
      do_logger "${WEBAPPS_BIN} has been stopped"
    else
      do_logger "service ${WEBAPPS_BIN} stop"
      service ${WEBAPPS_BIN} stop
    fi
}

#check and stop simsrv
stop_simsrv()
{
    local tempvar="`ps ax -opid,cmd | grep -v grep | grep "${SIMSRV_BIN}" | awk '{print $1}'`"

    do_logger "old ${SIMSRV_BIN}: ${tempvar}"
    if [ -z "${tempvar}" ]; then
      do_logger "${SIMSRV_BIN} has been stopped"
    else
      do_logger "kill all ${SIMSRV_BIN}"
      echo "${tempvar}" | xargs kill -9  
    fi
}

# check and stop dns
stop_dns()
{
    local tempvar="`service ${DNS_BIN} status | grep "is running"`"

    do_logger "old ${DNS_BIN}: ${tempvar}"

    if [ -z "${tempvar}" ]; then
      do_logger "${DNS_BIN} has been stopped"
    else
      do_logger "service ${DNS_BIN} stop"
      service ${DNS_BIN} stop
    fi
}

do_stop_daemon()
{
    local tempvar="`ps ax -opid,cmd | grep -v grep | grep $DAEMON_BIN | awk '{print $1}'`"
    if [ -z "$tempvar" ]; then
      echo "$DAEMON_BIN has been stopped"
    else
      echo "old daemon: $tempvar"
      echo "killall $DAEMON_BIN"
      echo "$tempvar" | xargs kill -9 
    fi
}


###### main() ######
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR]Operation not permitted! Please run cmd by root, or sudo cmd ..."
    exit 1
fi

# step 0: check and stop simsafe
do_stop_daemon

# step 1: check and stop simsrv
stop_simsrv

# step 2: check and stop webapps
stop_webapps

# step 3: check and stop dns
stop_dns


exit 0

