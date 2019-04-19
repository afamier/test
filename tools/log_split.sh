#!/bin/bash
# split received log msg into $DEV_SN.log files
# ----------------------------------------------------------------------------------
# running path: /dinstar/bin
# notes:
# (1) LOG_MSG template: DEV_SN|LEVEL|LOG_INFO
#     DEV_SN and LEVEL be generated in syslog-ng template
# (2) DEV_SN format : DA00-0001-0002-0003
# (3) LOG_OUT format: [2012-12-25 19:00:00]LEVEL|LOG_INFO
# ----------------------------------------------------------------------------------
LOG_SPLIT_BIN=log_split.sh
LOG_PATH=/provision/provision/syslog-ng
DEV_SN_LEN=19
MAX_SPLIT_SIZE=10485760
MAX_FILE_SIZE=10485760

DEV_SN=""
LEVEL=""
LAST_DEV_SN=""
LAST_LEVEL=""
COUNT=0
LOG_MSG=""
LOG_INFO=""

LOG_FILE=""
LOG_BK01=""
LOG_BK02=""

DATE_INFO=`date '+%Y-%m-%d %T'`

# check and rotate device log files
# $DATE_INFO
# $LOG_FILE
# $LOG_BK01
# $LOG_BK02
check_dev_log_files()
{
  # check log file exists?
  if [ ! -e $LOG_FILE ];then
    logger "[$DATE_INFO][log_split]create device log: $LOG_FILE"
    echo "" > $LOG_FILE
    return;
  fi

  # check log file size
  tmpSize=`du -sb $LOG_FILE | awk '{print $1}'`
  if [ $tmpSize -gt $MAX_FILE_SIZE ]; then
    # rotate from bk01 to bk02
    if [ -e $LOG_BK01 ]; then
      logger "[$DATE_INFO][log_split]rotate device log.1 to log.2: $LOG_FILE"
      cp -f $LOG_BK01 $LOG_BK02
    else
      logger "[$DATE_INFO][log_split]rotate device log to log.1: $LOG_FILE"
    fi

    # rotate from file to bk01
    cp -f $LOG_FILE $LOG_BK01

    # clean log file
    echo "" > $LOG_FILE
  fi
}

# cut key info from log msg
# $LOG_MSG
# output: LEVEL
# output: DEV_SN
get_key_info_from_log_msg()
{
  # step1: DEV_SN
  DEV_SN="${LOG_MSG%%|*}"

  if [ "$DEV_SN" = "" ]; then
    DEV_SN=""
    LEVEL=""
    return;
  fi
  
  if [[ "$DEV_SN" == "*-*-*-*" ]]; then
    DEV_SN=""
    LEVEL=""
    return;
  fi
  
  # check DEV_SN valid?
  len=`expr length "$DEV_SN"`
  if [ ! $len -eq $DEV_SN_LEN ]; then
    DEV_SN=""
    LEVEL=""
    return;
  fi

  # step2: LEVEL
  tmpMsg02="${LOG_MSG#*$DEV_SN|}"
  LEVEL="${tmpMsg02%%|*}"

  if [ "$LEVEL" = "" ]; then
    DEV_SN=""
    LEVEL=""
    return;
  fi
}

####################################################################################
# BEGIN
####################################################################################

# processing log msg from stdin
echo "wait log msg from << stdin, support multiple lines"

while read LOG_MSG
do
  DATE_INFO=`date '+%Y-%m-%d %T'`

  # get key info from first line
  get_key_info_from_log_msg

  # check header keys
  if [ "$DEV_SN" = "" ]; then    
    if [ $COUNT -lt 100 ]; then
      DEV_SN="$LAST_DEV_SN"
      LEVEL="$LAST_LEVEL"    
      COUNT=$(( COUNT + 1 ))
    fi
  else
    # update last device sn
    LAST_DEV_SN="$DEV_SN"
    LAST_LEVEL="$LEVEL"
    COUNT=0
  fi

  # output log info
  if [ "$DEV_SN" = "" ]; then
    logger "[$DATE_INFO][log_split]$LOG_MSG"
  else
    # check device log file
    LOG_FILE=$LOG_PATH/$DEV_SN.log
    LOG_BK01=$LOG_PATH/$DEV_SN.log.1
    LOG_BK02=$LOG_PATH/$DEV_SN.log.2

    check_dev_log_files
          
    # cut info_tail at first "TAG" from left
    # LOG_INFO=${LOG_MSG#*$DEV_SN|}
    # echo "[$DATE_INFO]$LOG_INFO" >> $LOG_PATH/$DEV_SN.log
    echo "[$DATE_INFO]$LOG_MSG" >> $LOG_PATH/$DEV_SN.log
  fi
done

