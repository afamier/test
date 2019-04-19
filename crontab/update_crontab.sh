#!/bin/bash
# start crontab

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PKG_NAME="crontab"

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# echo and logger info
# [`date +"%Y-%m-%d %T"`], use syslog timestamp
do_logger()
{
  local my_info="$1"  
  echo "[cloudenv/${PKG_NAME}] ${my_info}"
  echo "[cloudenv/${PKG_NAME}] ${my_info}" | logger
}


################################## MAIN BODY ##############################################
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
  echo "[ERROR] Operation not permitted! Please run cmd by root, or sudo cmd ..."
  exit 1
fi

echo "---------------- update ${PKG_NAME}"

# --- check and start crontab
crontab /dinstar/cloudenv/crontab/crontab_file
crontab -l

do_logger "crontab has been started"

