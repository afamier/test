#! /bin/bash
# database update tools

###### main() ######
# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR]Operation not permitted! Please run cmd by root, or sudo cmd ..."
    exit 1
fi

URL=$1
WEBPATH="/provision/version/patch/"
PKG="schemasync.tar.gz"

if [ -z "$URL" ]
then
   echo "Usage:   $0 <server02.dimiccs.com>"
   exit 1
fi

rm -f ${PKG}

wget ${URL}${WEBPATH}${PKG}
if [ $? != 0 ]
then
   echo "###### wget schemasync.tar.gz from $URL fail"
   exit 1
fi

tar -zxf schemasync.tar.gz

if [ $? != 0 ]
then
   echo "###### tar -zxf schemasync.tar.gz fail"
   exit 1
fi

cd schemasync
./install-deb-schemasync.sh
if [ $? != 0 ]
then
   echo "###### call schemasync/install-deb-schemasync.sh fail"
   exit 1
fi
cd -
echo "****** Install schemasync Success!"

exit 0



