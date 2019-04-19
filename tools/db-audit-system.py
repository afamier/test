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

# including innoDb cfg tables
db_tables = [
    'tbl_zone',
    'tbl_site',
    'tbl_pkg',
    'tbl_policy',
    'tbl_rule',
    'tbl_grp',
    'tbl_sim',
    'tbl_paid_grp',
    'tbl_paid_list',
    'tbl_sml',
    'tbl_ussl',
    'tbl_call',
    'tbl_sms',
    'tbl_ussd',
    'tbl_snum',
    'tbl_dnum',
    'tbl_mcc',
    'tbl_sms_task',
    'tbl_ne',
    'tbl_gw',
    'tbl_bk',
    'tbl_port',
    'tbl_gwp',
    'tbl_bkp',
    'tbl_agp',
    'tbl_tgp',
    'tbl_eth',
    'tbl_lan',
    'tbl_dsp',
    'tbl_ss7',
    'tbl_pri',
    'tbl_sip',
    'tbl_pm_head',
    'tbl_pmd_gwp_15',
    'tbl_pmd_sim_cur',
    'tbl_pmd_grp_cur',
    'tbl_pmd_domain_cur',
    'tbl_pmd_ne_cur',
    'tbl_pmd_tgp_cur',
    'tbl_pmd_eth_cur',
    'tbl_pmd_lan_cur',
    'tbl_pmd_dsp_cur',
    'tbl_pmd_ss7_cur',
    'tbl_pmd_agp_cur',
    'tbl_alarm',
    'tbl_alarm_domain_desc',
    ]

########################################### main proc ######################################
my_table = '' 
sql = ''

print "db audit system: %s@%s " % ( db_name, db_host )
print "--------------------------------------------------------------------"

# connect to db
db = MySQLdb.connect( db_host, db_user, db_password, db_name, charset='utf8' )

# get cursor
cursor = db.cursor()

# loop processing tables
for my_table in db_tables:
    # one by one
    print "db_audit processing table: %s" % ( my_table )
    # SQL CMD
    sql = "DELETE t1 FROM %s as t1 LEFT JOIN tbl_domain AS t2 ON ( t1.domain_uuid = t2.uuid ) WHERE t2.uuid IS NULL " % ( my_table )
    ###print "  sql = %s" % ( sql )
    try:
        # SQL RUN
        cursor.execute( sql )
        # SQL COMMIT
        db.commit()    
    except:
        print "  Error: roll back!"
        # SQL ROLLBACK
        db.rollback()
        break

# audit tbl_lic_domain
my_table = 'tbl_lic_domain'
print "db_audit processing table: %s" % ( my_table )
# SQL CMD
sql = "DELETE t1 FROM %s AS t1 LEFT JOIN tbl_domain AS t2 ON ( t1.spec_domain_uuid = t2.uuid ) WHERE t2.uuid IS NULL " % ( my_table )
###print "  sql = %s" % ( sql )
try:
    # SQL RUN
    cursor.execute( sql )
    # SQL COMMIT
    db.commit()    
except:
    print "  Error: roll back!"
    # SQL ROLLBACK
    db.rollback()


# last check tbl_node
my_table = 'tbl_node'
print "db_audit processing table: %s" % ( my_table )
# SQL CMD
sql = "DELETE FROM %s " % ( my_table )
###print "  sql = %s" % ( sql )
try:
    # SQL RUN
    cursor.execute( sql )
    # SQL COMMIT
    db.commit()    
except:
    print "  Error: roll back!"
    # SQL ROLLBACK
    db.rollback()


# close connect
db.close()


