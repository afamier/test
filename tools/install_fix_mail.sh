#! /bin/bash
#install libesmtp6 dpk
################################## VARIABLES & FUNCTIONS ######################################


install="###### install"


################################## MAIN BODY #############################################
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR]Operation not permitted! Please run cmd by root, or sudo cmd ..."
    exit 1
fi

URL=$1
WEBPATH="/provision/version/patch/"
PKG="libesmtp6_fix.tar.gz"

if [ -z "$URL" ]
then
   echo "Usage:   $0 <server02.dinstarcloud.com>"
   exit 1
fi

rm -f ${PKG}

wget ${URL}${WEBPATH}${PKG}
if [ $? != 0 ]
then
   echo "###### wget ${PKG} from $URL fail"
   exit 1
fi

tar -zxf ${PKG}

if [ $? != 0 ]
then
   echo "###### tar -zxf ${PKG} fail"
   exit 1
fi

cd libesmtp6
./install-get-libesmtp6.sh
if [ $? != 0 ]
then
   echo "###### call libesmtp6/install-get-libesmtp6.sh fail"
   exit 1
fi
cd -
echo "****** fix libesmtp6 Success!"

exit 0


