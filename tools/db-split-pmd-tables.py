#!/usr/bin/python
# -*- coding: UTF-8 -*-

import MySQLdb
import datetime
import commands

system_conf = "/etc/dinstar/system_conf.xml"

########################################### def func ######################################

# get param from system conf file
def get_xml_param_val( param ):
   my_cmd = "cat %s | grep %s | awk '{print $3}' | cut -f 2 -d '\"'" % ( system_conf, param )
   my_ret = commands.getoutput( my_cmd )
   return my_ret;

########################################### glb vars ######################################

db_host = get_xml_param_val( "mysql_url" )
db_user = get_xml_param_val( "mysql_user" )
db_password = get_xml_param_val( "mysql_pwd" )
db_name = get_xml_param_val( "mysql_db" )
db_split_num = int( get_xml_param_val( "mysql_pmd_splits" ) )
db_split_pre = int( get_xml_param_val( "mysql_pre_splits" ) )

# including myISAM pmd tables
db_tables = [
  'tbl_pmd_domain_15',
  'tbl_pmd_domain_24',
  'tbl_pmd_grp_15',
  'tbl_pmd_grp_24',
  'tbl_pmd_gwp_15',
  'tbl_pmd_gwp_24',
  'tbl_pmd_gw_15',
  'tbl_pmd_gw_24',
  'tbl_pmd_ne_15',
  'tbl_pmd_ne_24',
  'tbl_pmd_sim_15',
  'tbl_pmd_sim_24',
  'tbl_pmd_sys_15',
  'tbl_pmd_sys_24',
  ]

########################################### main proc ######################################

my_table = '' 
sql = ''
begin_date = ''
end_date = ''
my_date = ''
p_end_date = ''
p_count = 0

begin_date = ( datetime.date.today() - datetime.timedelta( days = db_split_num ) ).strftime( '%Y%m%d' )
end_date = ( datetime.date.today() + datetime.timedelta( days = db_split_pre ) ).strftime( '%Y%m%d' )

print "db split pmd tables: %s@%s " % ( db_name, db_host )
print "--------------------------------------------------------------------"
print "update new begin_date   = %s" % ( begin_date )
print "data split partitions   = %d" % ( db_split_num )
print "pre-create partitions   = %d" % ( db_split_pre )
print "update end_date(pre)    = %s " % ( end_date )
print "--------------------------------------------------------------------"

# connect to db
db = MySQLdb.connect( db_host, db_user, db_password, db_name, charset='utf8' )

# get cursor
cursor = db.cursor()

# loop processing tables
for my_table in db_tables:
    # one by one    
    print "db_split processing table: %s" % ( my_table )
    for index in range( 1, ( 1 + db_split_pre ) ):
        my_date = ( datetime.date.today() + datetime.timedelta( days = index ) ).strftime( '%Y%m%d' )
        p_end_date = ''
       
        # get spec partition
        # SQL CMD
        sql = "SELECT REPLACE(partition_name,'p','') FROM INFORMATION_SCHEMA.PARTITIONS WHERE table_schema='%s' and table_name='%s' AND partition_name='p%s' " % ( db_name, my_table, my_date )
        ###print "  sql = %s" % ( sql )
        try:
            # SQL RUN
            cursor.execute( sql )
            # SQL RESULT
            results = cursor.fetchall()
            for row in results:
                p_end_date = row[ 0 ]
        except:
            print "  Error: get spec partition!"
            break

        # SQL DEBUG
        ###print "  check p_end_date = %s" % ( p_end_date )

        if p_end_date == '':
            # create new partition
            # SQL CMD
            sql = "ALTER TABLE %s ADD PARTITION ( PARTITION p%s VALUES LESS THAN ( TO_DAYS('%s') ) )" % ( my_table, my_date, my_date )
            ###print "  sql = %s" % ( sql )
            try:
                # SQL RUN
                cursor.execute( sql )
                print "  create p_end_date = %s" % ( my_date )
            except:
                print "  Error: create new partition!"
                break

    # clean older partition
    while 1:
        p_end_date = ''
        # SQL CMD
        sql = "SELECT REPLACE(partition_name,'p','') FROM INFORMATION_SCHEMA.PARTITIONS WHERE table_schema='%s' and table_name='%s' AND partition_name<>'' ORDER BY partition_ordinal_position ASC LIMIT 1" % ( db_name, my_table )
        ###print "  sql = %s" % ( sql )
        try:
            # SQL RUN
            cursor.execute( sql )

            # SQL RESULT
            results = cursor.fetchall()
            for row in results:
                p_end_date = row[ 0 ]
                ###print "get oldest p_end_date = %s" % ( p_end_date )
        except:
            print "  Error: get oldest partition!"
            break

        # SQL DEBUG
        # drop oldest partition
        if p_end_date <> '' and p_end_date < begin_date:
            # SQL CMD
            sql = "ALTER TABLE %s DROP PARTITION p%s" % ( my_table, p_end_date )
            ###print "  sql = %s" % ( sql )
            try:
                # SQL RUN
                cursor.execute( sql )
                print "  drop oldest p_end_date = %s" % ( p_end_date )

            except:
                print "  Error: delete oldest partition!"
                break
                
        else:
            # clean finished
            break

    # clean newer partition
    while 1:
        p_end_date = ''
        # SQL CMD
        sql = "SELECT REPLACE(partition_name,'p','') FROM INFORMATION_SCHEMA.PARTITIONS WHERE table_schema='%s' and table_name='%s' AND partition_name<>'' ORDER BY partition_ordinal_position DESC LIMIT 1" % ( db_name, my_table )
        ###print "  sql = %s" % ( sql )
        try:
            # SQL RUN
            cursor.execute( sql )

            # SQL RESULT
            results = cursor.fetchall()
            for row in results:
                p_end_date = row[ 0 ]
                ###print "  get newest p_end_date = %s" % ( p_end_date )
        except:
            print "  Error: get newest partition!"
            break

        # SQL DEBUG
        # drop newest partition
        if p_end_date <> '' and p_end_date > end_date:
            # SQL CMD
            sql = "ALTER TABLE %s DROP PARTITION p%s" % ( my_table, p_end_date )
            ###print "  sql = %s" % ( sql )
            try:
                # SQL RUN
                cursor.execute( sql )
                print "  drop newest p_end_date = %s" % ( p_end_date )

            except:
                print "  Error: delete newest partition!"
                break
                
        else:
            # clean finished
            break

    # clean null partition
    while 1:
        p_end_date = ''
        # SQL CMD
        sql = "SELECT REPLACE(partition_name,'p','') FROM INFORMATION_SCHEMA.PARTITIONS WHERE table_schema='%s' and table_name='%s' AND partition_name='' ORDER BY partition_ordinal_position DESC LIMIT 1" % ( db_name, my_table )
        ###print "  sql = %s" % ( sql )
        try:
            # SQL RUN
            cursor.execute( sql )

            # SQL RESULT
            results = cursor.fetchall()
            for row in results:
                p_end_date = row[ 0 ]
                ###print "  get newest p_end_date = %s" % ( p_end_date )
        except:
            print "  Error: get newest partition!"
            break

        # SQL DEBUG
        # drop newest partition
        if p_end_date <> '' and p_end_date > end_date:
            # SQL CMD
            sql = "ALTER TABLE %s DROP PARTITION p%s" % ( my_table, p_end_date )
            ###print "  sql = %s" % ( sql )
            try:
                # SQL RUN
                cursor.execute( sql )
                print "  drop newest p_end_date = %s" % ( p_end_date )

            except:
                print "  Error: delete newest partition!"
                break
                
        else:
            # clean finished
            break

    # last check partition count
    # SQL RUN
    sql = "SELECT COUNT( partition_name ) FROM INFORMATION_SCHEMA.PARTITIONS WHERE table_schema='%s' and table_name='%s' AND partition_name<>'' " % ( db_name, my_table )
    ###print "  sql = %s" % ( sql )
    try:
        cursor.execute( sql )

        # SQL RESULT
        results = cursor.fetchall()
        for row in results:
            p_count = row[ 0 ]
    except:
        print "  Error: check partition count!"
        break

    # SQL DEBUG
    print "  update p_count = %d" % ( p_count )
    print "--------------------------------------------------------------------"

# close connect
db.close()


