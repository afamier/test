#! /bin/sh
#############################################
#Only used to drop some tables' index before 
#updating
#tbl_dsp,tbl_pri,tbl_ss7,tbl_sip
#
#############################################

# check user role
ROLE=`id | awk '{print $1}' | sed -e 's/=/ /' -e 's/(/ /' -e 's/)/ /' |awk '{print $3}'`
if [ "${ROLE}" != "root" ]; then
    echo "[ERROR]Operation not permitted! Please run cmd by root, or sudo cmd ..."
    exit 1
fi

################variables####################

MYSQL_SUPER=$1
MYSQL_PASSWD=$2
DATABASE=$3
MYSQL_URL=$4


##################main#######################

#check dsp_index on tbl_dsp
COMMAND="select COLUMN_NAME from information_schema.statistics where table_schema='${DATABASE}' and table_name='tbl_dsp' and index_name='dsp_index'"

INDEX=`mysql -h${MYSQL_URL} -u${MYSQL_SUPER} -p${MYSQL_PASSWD} -e "$COMMAND"`
if [ $? -ne 0 ]
then
   echo "###### Check dsp_index on tbl_dsp fail! Please check it manually and run this script again!"
   exit 1
fi

INDEX=`echo $INDEX`
if [ "$INDEX" = "COLUMN_NAME ne_uuid object_id1 object_id2 object_id3" ]
then
   echo "****** ready to drop dsp_index on tbl_dsp"
   COMMAND="DROP INDEX dsp_index ON tbl_dsp"
   RESULT=`mysql -h${MYSQL_URL} -u${MYSQL_SUPER} -p${MYSQL_PASSWD} -B ${DATABASE} -e "$COMMAND"`
   if [ $? -ne 0 ]
   then
      echo "###### drop dsp_index on tbl_dsp fail! Please drop it manually and run this scrip again!"
      exit 1
   fi
else
   echo "****** Don't Need to drop dsp_index on tbl_dsp!"
fi


#check ne_index on tbl_pri
COMMAND="select COLUMN_NAME from information_schema.statistics where table_schema='${DATABASE}' and table_name='tbl_pri' and index_name='ne_index'"

INDEX=`mysql -h${MYSQL_URL} -u${MYSQL_SUPER} -p${MYSQL_PASSWD} -e "$COMMAND"`
if [ $? -ne 0 ]
then
   echo "###### Check ne_index on tbl_pri fail! Please check it manually and run this script again!"
   exit 1
fi

INDEX=`echo $INDEX`
if [ "$INDEX" = "COLUMN_NAME ne_uuid object_id1 object_id2 object_id3" ]
then
   echo "****** ready to drop ne_index on tbl_pri"
   COMMAND="DROP INDEX ne_index ON tbl_pri"
   RESULT=`mysql -h${MYSQL_URL} -u${MYSQL_SUPER} -p${MYSQL_PASSWD} -B ${DATABASE} -e "$COMMAND"`
   if [ $? -ne 0 ]
   then
      echo "###### drop ne_index on tbl_pri fail! Please drop it manually and run this scrip again!"
      exit 1
   fi
else
   echo "****** Don't Need to drop ne_index on tbl_pri!"
fi


#check ne_index on tbl_sip
COMMAND="select COLUMN_NAME from information_schema.statistics where table_schema='${DATABASE}' and table_name='tbl_sip' and index_name='ne_index'"

INDEX=`mysql -h${MYSQL_URL} -u${MYSQL_SUPER} -p${MYSQL_PASSWD} -e "$COMMAND"`
if [ $? -ne 0 ]
then
   echo "###### Check ne_index on tbl_sip fail! Please check it manually and run this script again!"
   exit 1
fi

INDEX=`echo $INDEX`
if [ "$INDEX" = "COLUMN_NAME ne_uuid object_id1 object_id2 object_id3" ]
then
   echo "****** ready to drop ne_index on tbl_sip"
   COMMAND="DROP INDEX ne_index ON tbl_sip"
   RESULT=`mysql -h${MYSQL_URL} -u${MYSQL_SUPER} -p${MYSQL_PASSWD} -B ${DATABASE} -e "$COMMAND"`
   if [ $? -ne 0 ]
   then
      echo "###### drop ne_index on tbl_sip fail! Please drop it manually and run this scrip again!"
      exit 1
   fi
else
   echo "****** Don't Need to drop ne_index on tbl_sip!"
fi


#check ne_index on tbl_ss7
COMMAND="select COLUMN_NAME from information_schema.statistics where table_schema='${DATABASE}' and table_name='tbl_ss7' and index_name='ne_index'"

INDEX=`mysql -h${MYSQL_URL} -u${MYSQL_SUPER} -p${MYSQL_PASSWD} -e "$COMMAND"`
if [ $? -ne 0 ]
then
   echo "###### Check ne_index on tbl_ss7 fail! Please check it manually and run this script again!"
   exit 1
fi

INDEX=`echo $INDEX`
if [ "$INDEX" = "COLUMN_NAME ne_uuid object_id1 object_id2 object_id3" ]
then
   echo "****** ready to drop ne_index on tbl_ss7"
   COMMAND="DROP INDEX ne_index ON tbl_ss7"
   RESULT=`mysql -h${MYSQL_URL} -u${MYSQL_SUPER} -p${MYSQL_PASSWD} -B ${DATABASE} -e "$COMMAND"`
   if [ $? -ne 0 ]
   then
      echo "###### drop ne_index on tbl_ss7 fail! Please drop it manually and run this scrip again!"
      exit 1
   fi
else
   echo "****** Don't Need to drop ne_index on tbl_ss7!"
fi

