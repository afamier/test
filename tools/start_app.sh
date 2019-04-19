#!/bin/sh
# start /dinstar/bin/simsrv


FILE_CONF="/etc/dinstar/system_conf.xml"
ETH_MTU=1400
PARAM_VAL=""


# 12.04, 14.04, 16.04
RELEASE=`lsb_release -r|cut -f 2`
# i386, x86_64
ARCH=`uname -i`


MODE=$1

SIMSRV_BIN="simsrv"
DNS_BIN="bind9"
SYSLOG_BIN="syslog-ng"
UDP_ECHO_SERVER_BIN="udp_echo_server"

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


# init sys setting
init_eth_mtu()
{
    local eth_n0=$1
    local eth_n1=$2
        
    # enter into bin dir
    cd /dinstar/bin

    # init eth_n0 MTU
    ifconfig ${eth_n0} mtu $ETH_MTU
    echo "${eth_n0} mtu: `cat /sys/class/net/${eth_n0}/mtu`"
    echo [`date +"%Y-%m-%d %T"`]"[script]${eth_n0} mtu: `cat /sys/class/net/${eth_n0}/mtu`" | logger

    # init eth_n1 MTU
    if [ -e /sys/class/net/${eth_n1} ]; then
	    ifconfig ${eth_n1} mtu $ETH_MTU
  	  	echo "${eth_n1} mtu: `cat /sys/class/net/${eth_n1}/mtu`"
    	echo [`date +"%Y-%m-%d %T"`]"[script]${eth_n1} mtu: `cat /sys/class/net/${eth_n1}/mtu`" | logger
    fi
}

# init sys setting
init_sys_setting()
{
    local   my_info=""

    # enter into bin dir
    cd /dinstar/bin

    # set ulimit for core-dump info
    echo [`date +"%Y-%m-%d %T"`]"[script]ulimit -c unlimited" | logger
    ulimit -c unlimited

    # init nat port 80 mapping
    my_info=`iptables -t nat -S PREROUTING |grep "\--dport 80.*\--to-ports 8888"`
    if [ -z "${my_info}" ]; then
        echo "add nat port 80 to 8888 mapping"
        echo [`date +"%Y-%m-%d %T"`]"[script]add nat port 80 to 8888 mapping" | logger
        iptables -t nat -A PREROUTING -p tcp --dport 80 -j REDIRECT --to-port 8888
    else
        echo "update nat port 80 to 8888 mapping"
        echo [`date +"%Y-%m-%d %T"`]"[script]update nat port 80 to 8888 mapping" | logger
    fi
}

# check and recover libmysqlclient.so.18 for Ubuntu 16.04
check_libmysqlclient()
{
    if [ ! -f /usr/lib/i386-linux-gnu/libmysqlclient.so.18 ]
    then
      cp -f /dinstar/bin/libmysqlclient.so.18.0.0 /usr/lib/i386-linux-gnu/libmysqlclient.so.18.0.0
      ln -s /usr/lib/i386-linux-gnu/libmysqlclient.so.18.0.0 /usr/lib/i386-linux-gnu/libmysqlclient.so.18
      do_logger "libmysqlclient.so.18 has been recovered"
    fi

    if [ ! -f /usr/lib/i386-linux-gnu/libmysqlclient.so.20 ]
    then
      cp -f /dinstar/bin/libmysqlclient.so.20.0.0 /usr/lib/i386-linux-gnu/libmysqlclient.so.20.0.0
      ln -s /usr/lib/i386-linux-gnu/libmysqlclient.so.20.0.0 /usr/lib/i386-linux-gnu/libmysqlclient.so.20
      do_logger "libmysqlclient.so.20 has been recovered"
    fi
}

# check and start dns
start_dns()
{
    local tempvar="`service $DNS_BIN status | grep "is running"`"

    do_logger "old $DNS_BIN: $tempvar"

    if [ -z "$tempvar" ]; then
      do_logger "service $DNS_BIN restart"
      service $DNS_BIN restart
    else
      do_logger "$DNS_BIN has been started"
    fi
}

#check and start simsrv
start_simsrv()
{
    local tempvar="`ps ax | grep -v grep | grep /dinstar/bin/$SIMSRV_BIN | awk '{print $1}'`"

    do_logger "old $SIMSRV_BIN: $tempvar"

    if [ -z "$tempvar" ]; then
      do_logger "/dinstar/bin/$SIMSRV_BIN &"
      /dinstar/bin/$SIMSRV_BIN &
    else
      do_logger "$SIMSRV_BIN has been started"
    fi
}

# check and start webapps $1
start_webapps()
{
    local mode=$1
    local tempvar="`service $WEBAPPS_BIN status | grep "is running"`"

    do_logger "old $WEBAPPS_BIN: $tempvar"

    if [ -z "$tempvar" ]; then
      do_logger "service $WEBAPPS_BIN restart"
      service $WEBAPPS_BIN restart
    elif [ -z ${mode} ]; then
      do_logger "$WEBAPPS_BIN has been started, force restart..."
      service $WEBAPPS_BIN restart
    else
      do_logger "$WEBAPPS_BIN has been started"
    fi
}

# check and start udp_echo_server
start_udp_echo_server()
{
    local tempvar="`ps ax -opid,cmd | grep -v grep | grep $UDP_ECHO_SERVER_BIN | awk '{print $1}'`"

    do_logger "old $UDP_ECHO_SERVER_BIN: $tempvar"

    ###tempvar=""
    if [ -z "$tempvar" ]; then
    	do_logger "start $UDP_ECHO_SERVER_BIN"
    	/dinstar/bin/$UDP_ECHO_SERVER_BIN 2019 &
    else
      do_logger "$UDP_ECHO_SERVER_BIN has been started"
    fi
}

# check and start log_split via syslog-ng restart
start_log_split()
{
    local tempvar="`service $SYSLOG_BIN status | grep "is running"`"

    do_logger "old $SYSLOG_BIN: $tempvar"

    if [ -z "$tempvar" ]; then
    	do_logger "start $SYSLOG_BIN"
	    service $SYSLOG_BIN start
    else
      do_logger "$SYSLOG_BIN has been started"
    fi
}

do_start_daemon()
{
    local tempvar="`ps ax -opid,cmd | grep -v grep | grep $DAEMON_BIN | awk '{print $1}'`"
    ###echo "current daemon pid: $tempvar"
    if [ -z "$tempvar" ]; then
      echo "start daemon $DAEMON_BIN"
      /dinstar/bin/$DAEMON_BIN &
    else
      echo "old daemon: $tempvar"
      echo "$DAEMON_BIN has been started"
    fi
}


###### main() ######
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR]Operation not permitted! Please run cmd by root, or sudo cmd ..."
    exit 1
fi

### init eth mtu
get_xml_param_val $FILE_CONF "stun_listen_eth"
ETH_NAME0=$PARAM_VAL

### get system conf info
get_xml_param_val $FILE_CONF "stun_listen_2nd"
ETH_NAME1=$PARAM_VAL

init_eth_mtu ${ETH_NAME0} ${ETH_NAME1}


# enable core file
ulimit -c unlimited

# step 0: load system-conf.xml
if [ -z ${MODE} ]; then
  ### warning: sometimes get ip fail after reboot!!!
  /dinstar/bin/configure.sh auto

  ### warning: update openssh-server
  dpkg-reconfigure openssh-server
fi

# step 1: init sys setting
init_sys_setting

# step 2: init libmysqlclient.so
check_libmysqlclient

# step 3: check and start log_split
start_log_split

# step 4: check and start udp_echo_server
start_udp_echo_server



# step 5: check and start dns
start_dns

# step 6: check and start webapps
start_webapps ${MODE}

# step 7: check and start simsrv
start_simsrv

# step 8: check and start simsafe
do_start_daemon

exit 0

