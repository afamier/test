#!/bin/bash
# crontab task per day

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PKG_NAME="crontab"
FILE_DIR="/dinstar/cloudenv/crontab/day"
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


# echo and logger info
# [`date +"%Y-%m-%d %T"`], use syslog timestamp
do_logger()
{
  local my_info="$1"  
  echo "[cloudenv/${PKG_NAME}] ${my_info}"
  echo "[cloudenv/${PKG_NAME}] ${my_info}" | logger
}


# find and execute scripts under sub-dir
do_scripts()
{
  cd $1

  # do shell commands
  for file in ./*.sh
  do
    if test -f $file
    then
      echo "$1/$file"
      do_logger "$1/$file"
      $1/$file
    fi
  done

  # do python commands
  for file in ./*.py
  do
    if test -f $file
    then
      echo "$1/$file"
      do_logger "$1/$file"
      $1/$file
    fi
  done

  cd ..
}


################################## MAIN BODY ##############################################
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR] operation not permitted! Please run cmd by root, or sudo cmd ..."
    exit 1
fi

# do scripts
do_scripts ${FILE_DIR}


