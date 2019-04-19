#!/bin/bash
# ./configure.sh [auto]
# system conf setting   : $FILE_CONF
# system default setting: $FILE_DEFAULT
# if $FILE_CONF not exists, auto copy $FILE_DEFAULT to $FILE_CONF

# system conf/default setting file
SYSTEM_PATH="dinstar"
FILE_CONF="/etc/dinstar/system_conf.xml"
FILE_DEFAULT="/dinstar/cfg/system_conf.xml_default"

# confirm mode
AUTO_MODE=$1

# system parameter names[27]
PARAM_NAME=( "sys_uuid"
             "sys_alias"
             "cloud_uuid"
             "sys_pub_ip"
             "stun_listen_eth"
             "stun_listen_2nd"
             "trans_listen_port"
             "web_proxy_port"
             "telnet_proxy_port"
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
PARAM_NAME2=""

# system parameter desc
PARAM_DESC=( "system{SPACE}uuid"
             "system{SPACE}alias"
             "system{SPACE}cloud{SPACE}uuid"
             "external{SPACE}ip{SPACE}address,0.0.0.0{SPACE}-{SPACE}default"
             "stun{SPACE}server{SPACE}listen{SPACE}eth{SPACE}interface"
             "stun{SPACE}server{SPACE}listen{SPACE}2nd{SPACE}interface"
             "trans{SPACE}server{SPACE}listen{SPACE}port"
             "remote-web{SPACE}proxy{SPACE}port"
             "remote-telnet{SPACE}proxy{SPACE}port"
             "service{SPACE}api{SPACE}port"
             "mysql{SPACE}url/ip"
             "mysql{SPACE}port"
             "mysql{SPACE}database"
             "mysql{SPACE}username"
             "mysql{SPACE}password"
             "mysql{SPACE}log{SPACE}table{SPACE}split{SPACE}days"
             "mysql{SPACE}pmd{SPACE}table{SPACE}split{SPACE}days"
             "mysql{SPACE}pre{SPACE}create{SPACE}split{SPACE}days"
             "provision{SPACE}url/ip"
             "provision{SPACE}username"
             "provision{SPACE}password"
             "auth{SPACE}server{SPACE}url01/ip"
             "auth{SPACE}server{SPACE}url02/ip"
             "dns{SPACE}domain01"
             "dns{SPACE}domain02"
             "max{SPACE}telnet{SPACE}connet"
             "sms{SPACE}queue{SPACE}size"
           );
PARAM_DESC2=""

# system parameter values
PARAM_VAL=""
PARAM_VAL2=""

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

    unset PARAM_NAME2
    for my_item in ${PARAM_NAME[*]}
    do
        PARAM_NAME2[$my_index]=$my_item
        my_index=$(($my_index+1))
    done

    PARAM_NUM=$my_index
}

# -------------------------------------------------
# convert multiple param_desc into array
# -------------------------------------------------
convert_param_desc_to_array()
{
    local my_item=""
    local my_index=0

    unset PARAM_DESC2
    for my_item in ${PARAM_DESC[*]}
    do
        PARAM_DESC2[$my_index]=`echo $my_item | sed "s/{SPACE}/\ /g"`
        ### echo "PARAM_DESC2[$my_index]="${PARAM_DESC2[$my_index]}
        my_index=$(($my_index+1))
    done
}


# -------------------------------------------------
# get one param value from xml file:
#     <param name="param01" value="1"/>
# param_name MUST be unique one
# return: PARAM_VAL2
# -------------------------------------------------
get_xml_param_val()
{
    local my_file=$1
    local my_param=$2

    PARAM_VAL=`cat $my_file | grep $my_param | awk '{print $3}' | cut -f 2 -d '"'`
}


# -------------------------------------------------
# set one param value from xml file:
#     <param name="param01" value="1"/>
# param_name MUST be unique one
# return: PARAM_VAL2
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

    unset PARAM_VAL2

    ### load all param
    for my_param in ${PARAM_NAME2[*]}
    do
        ### get param value
        get_xml_param_val $FILE_CONF $my_param

        ### set param value array
        if [ -z $PARAM_VAL ]; then
            printf "ERROR! Unknown param %s, load fail! \n" $my_param
            printf "Please delete the invalid file: %s, then configure again! \n" $FILE_CONF
            # rm -f $FILE_CONF
            exit 1
        fi

        PARAM_VAL2[ $my_index ]="$PARAM_VAL"

        ### echo "PARAM_VAL2[ $my_index ]="${PARAM_VAL2[ $my_index ]}

        my_index=$(($my_index+1))
    done
}

# -------------------------------------------------
# lookup all param in PARAM_NAME[]
# -------------------------------------------------
show_xml_param_list()
{
    local my_index=0
    local my_param=""
    local my_num=0

    ### print list title
    printf "\n%-4s %-24s : %-32s %s \n" "No." "Name" "Value" "Desc"
    printf "%s \n" "---------------------------------------------------------------------------"

    ### print list content
    for my_param in ${PARAM_NAME2[*]}
    do
        ### print param info
        my_num=$(($my_index+1))
        printf "%-4d %-24s : %-32s %s \n" $my_num $my_param ${PARAM_VAL2[$my_index]} "${PARAM_DESC2[$my_index]}"
        my_index=$(($my_index+1))
    done
}

# -------------------------------------------------
# update all param in PARAM_NAME[]
# -------------------------------------------------
update_xml_param_val()
{
    local my_num=0
    local my_index=0
    local my_value=""
    local my_flag=false

    while true;
    do
    {
    		if [ "$AUTO_MODE" == "auto" ];then
        		printf "To change value, please enter parameter No.( 0 - quit ): 0 \n"
						break;
    		fi

				### need confirm    		
        printf "To change value, please enter parameter No.( 0 - quit ): "
        read my_num

        ### check is null?
        if [ -z $my_num ]; then
            break;
        elif [ "$my_num" = "" ]; then
            break;
        fi

        ### check is valid number?
        my_flag=`awk 'BEGIN { if (match(ARGV[1],"^[0-9]+$") != 0) print "true"; else print "false" }' $my_num`
        if [ $my_flag == "false" ]; then
            continue;
        fi

        ### check is 0-quit?
        if [ $my_num -eq 0 ]; then
            break;
        fi

        ### check is valid index?
        if [ $my_num -gt $PARAM_NUM ]; then
            continue;
        fi

        my_index=$(($my_num-1))

        printf "Enter new value of parameter %s: " ${PARAM_NAME2[$my_index]}
        read my_value

        if [ ! -z $my_value ]; then
            PARAM_VAL2[$my_index]=$my_value
        fi

        show_xml_param_list
    }
    done
}

# -------------------------------------------------
# save all param in PARAM_NAME[]
# -------------------------------------------------
save_xml_param_val()
{
    local my_index=0
    local my_flag=""

    my_index=0

    while true;
    do
    {
        if [ $my_index -eq $PARAM_NUM ]; then
            break;
        fi

        set_xml_param_val $FILE_CONF ${PARAM_NAME2[$my_index]} ${PARAM_VAL2[$my_index]}

        my_index=$(($my_index+1))
    }
    done

    ### convert to unix format
    dos2unix $FILE_CONF

    printf "*** SUCCESS! Updated system conf file: %s \n" $FILE_CONF
}

# -------------------------------------------------
# load all param from $FILE_CONF
# -------------------------------------------------
load_system_conf_file()
{
    local my_flag=""
    local my_load=true

    ### check $FILE_CONF exists?
    if [ ! -f "$FILE_CONF" ]; then
      printf "ERROR: No found system conf file: %s \n" $FILE_CONF
      return;
    fi

    ### confirm load or not?
    while true;
    do
    {
    		if [ "$AUTO_MODE" == "auto" ];then
	          printf "\nPlease confirm to load system conf file( Y/N, default is Yes ): Y \n"
						break;
    		fi
    
        printf "\nPlease confirm to load system conf file( Y/N, default is Yes ): "
        read my_flag

        if [ -z $my_flag ]; then
            my_load=true
            break;
        elif [ "$my_flag" = "" ]; then
            my_load=true
            break;
        elif [ $my_flag = "y" ]; then
            my_load=true
            break;
        elif [ $my_flag = "Y" ]; then
            my_load=true
            break;
        elif [ $my_flag = "n" ]; then
            my_load=false
            break;
        elif [ $my_flag = "N" ]; then
            my_load=false
            break;
        else
            continue;
        fi
    }
    done

    if [ $my_load = false ]; then
        printf "INFO: system conf file %s, hasn't been loaded! \n" $FILE_CONF
			  printf "      load command: sudo /dinstar/bin/load-system-conf.sh \n"
        return;
    fi

    /dinstar/bin/load-system-conf.sh
}


###### main() ######
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR]Operation not permitted! Please run cmd by root, or sudo cmd ..."
    exit 1
fi

check_system_conf_file

convert_param_name_to_array
convert_param_desc_to_array

load_xml_param_list

printf "\nPlease confirm the system parameters:\n"
show_xml_param_list

update_xml_param_val

save_xml_param_val

### check result
### load_xml_param_list
### show_xml_param_list

# don't confirm at here!
### load_system_conf_file
/dinstar/bin/load-system-conf.sh


