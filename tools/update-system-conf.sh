#!/bin/bash
# ./update-system-conf.sh
# system conf setting   : $FILE_CONF
# system default setting: $FILE_DEFAULT
# if $FILE_CONF not exists, auto copy $FILE_DEFAULT to $FILE_CONF
# if new param was added, put it into $FILE_CONF by default setting of $FILE_DEFAULT

# system conf/default setting file
SYSTEM_PATH="dinstar"
FILE_CONF="/etc/dinstar/system_conf.xml"
FILE_DEFAULT="/dinstar/cfg/system_conf.xml_default"

# system parameter names[21]
PARAM_NAME=( "sys_uuid"
             "sys_alias"
             "cloud_uuid"
             "sys_pub_ip"
             "stun_listen_eth"
             "stun_listen_2nd"
             "trans_listen_port"
             "web_proxy_port"
             "service_api_port"
             "mysql_url"
             "mysql_port"
             "mysql_db"
             "mysql_user"
             "mysql_pwd"
             "mysql_log_splits"
             "mysql_pmd_splits"
             "mysql_pre_splits"
             "prov_url"
             "prov_user"
             "prov_pwd"
             "dns_url01"
             "dns_url02"
             "dns_domain01"
             "dns_domain02"
             "telnet_max_connect"
             "max_sms_queue_size"
           );
PARAM_NAME_NEW=""

# old system_conf.xml value
PARAM_VAL_OLD=""

# system_conf.xml_default value
PARAM_VAL_DEFAULT=""

# new system_conf.xml value
PARAM_VAL_NEW=""

# system parameter count
PARAM_NUM=0

# -------------------------------------------------
# check $FILE_CONF file
# -------------------------------------------------
check_system_conf_file()
{
    ### check /etc/$SYSTEM_PATH exists?
    if [ ! -d "/etc/$SYSTEM_PATH" ]; then
      printf "create new system conf dir: /etc/%s \n" $SYSTEM_PATH
      mkdir /etc/$SYSTEM_PATH
    fi

    ### check $FILE_CONF exists?
    if [ ! -f "$FILE_CONF" ]; then
      printf "create new system conf file: %s \n" $FILE_CONF
      ### check $FILE_DEFAULT exists?
      if [ ! -f "$FILE_DEFAULT" ]; then
          printf "\nERROR! no system default file: %s \n" $FILE_DEFAULT
          printf "Please install package at first! \n"
          exit 1
      fi

      cp $FILE_DEFAULT $FILE_CONF
    fi
}

# -------------------------------------------------
# convert multiple param_name into array
# -------------------------------------------------
convert_param_name_to_array()
{
    local my_item=""
    local my_index=0

    unset PARAM_NAME_NEW
    for my_item in ${PARAM_NAME[*]}
    do
        PARAM_NAME_NEW[$my_index]=$my_item
        my_index=$(($my_index+1))
    done

    PARAM_NUM=$my_index
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

    PARAM_VAL_OLD=`cat $my_file | grep $my_param | awk '{print $3}' | cut -f 2 -d '"'`
}


# -------------------------------------------------
# get one param value from xml file:
#     <param name="param01" value="1"/>
# param_name MUST be unique one
# return: PARAM_VAL_DEFAULT
# -------------------------------------------------
get_default_param_val()
{
    local my_file=$1
    local my_param=$2

    PARAM_VAL_DEFAULT=`cat $my_file | grep $my_param | awk '{print $3}' | cut -f 2 -d '"'`
}


# -------------------------------------------------
# set one param value from xml file:
#     <param name="param01" value="1"/>
# param_name MUST be unique one
# return: PARAM_VAL_NEW
# -------------------------------------------------
set_xml_param_val()
{
    local my_file=$1
    local my_param=$2
    local my_value=$3

    sed -i "/<.*name=\"$my_param\"/ s/value=\".*\"/value=\"$my_value\"/" $my_file
    if [ $? -ne 0 ]; then
    {
        printf "ERROR! set xml param %s fail, value=%s! \n" $my_param $my_value
        return 1
    }
    fi

    return 0
}

# -------------------------------------------------
# load all param in PARAM_NAME[]
# -------------------------------------------------
load_xml_param_list()
{
    local my_index=0
    local my_param=""

    unset PARAM_VAL_NEW

    ### load all param
    for my_param in ${PARAM_NAME_NEW[*]}
    do
        ### get xml param value
        get_xml_param_val $FILE_CONF $my_param

        ### get default param value
        get_default_param_val $FILE_DEFAULT $my_param

        ### set param value array
        if [ -z $PARAM_VAL_OLD ]; then
	          if [ -z $PARAM_VAL_DEFAULT ]; then
                printf "WARNING! deleted param %s, discard! \n" $my_param
            else
                printf "WARNING! new param %s, load default setting: %s \n" $my_param $PARAM_VAL_DEFAULT
				        PARAM_VAL_NEW[ $my_index ]="$PARAM_VAL_DEFAULT"
            fi
        else
            ### printf "INFO, old param %s, setting: %s \n" $my_param $PARAM_VAL_OLD
		        PARAM_VAL_NEW[ $my_index ]="$PARAM_VAL_OLD"
        fi

        ### echo "PARAM_VAL_NEW[ $my_index ]="${PARAM_VAL_NEW[ $my_index ]}
        my_index=$(($my_index+1))
    done
}

# -------------------------------------------------
# save all param in PARAM_NAME[]
# -------------------------------------------------
save_xml_param_val()
{
    local my_index=0

    my_index=0

    while true;
    do
    {
        if [ $my_index -eq $PARAM_NUM ]; then
            break;
        fi

        ### printf "INFO, set param %s, setting: %s \n" ${PARAM_NAME_NEW[$my_index]} ${PARAM_VAL_NEW[$my_index]}
        set_xml_param_val $FILE_CONF ${PARAM_NAME_NEW[$my_index]} ${PARAM_VAL_NEW[$my_index]}

        my_index=$(($my_index+1))
    }
    done

    ### convert to unix format
    dos2unix $FILE_CONF

    printf "*** SUCCESS! Imported system conf file: %s \n" $FILE_CONF
}


###### main() ######
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR]Operation not permitted! Please run cmd by root, or sudo cmd ..."
    exit 1
fi

printf "\n\nLoad setting of system parameters ... \n"

check_system_conf_file

convert_param_name_to_array

load_xml_param_list

### based on new system_conf.xml_default
cp $FILE_DEFAULT $FILE_CONF

save_xml_param_val

