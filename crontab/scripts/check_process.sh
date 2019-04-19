#!/bin/bash
# check and start process again

PROC_NAME="proc_name.py"
PROC_PATH="/xxx/bin/proc_name.py"

###### main() ######
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR] operation not permitted! Please run cmd by root, or sudo cmd ..."
    exit 1
fi

# --- check and start process again
PROC_PID=`ps ax -opid,cmd | grep -v grep | grep $PROC_NAME`

if [ -z "$PROC_PID" ]; then
    echo "[script]@@@ start $PROC_NAME again: $PROC_PATH ..."
    echo "[script]@@@ start $PROC_NAME again: $PROC_PATH ..." | logger
    sudo $PROC_PATH &
else
    echo "[script] check $PROC_NAME OK"
    echo "[script] check $PROC_NAME OK" | logger
fi

