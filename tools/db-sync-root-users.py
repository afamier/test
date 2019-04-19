#!/usr/bin/python
# -*- coding: UTF-8 -*-
# Input:  host root password local_password remote_password
# Output: root@127.0.0.1, set password as local_password
#         dinstar@localhost, set password as local_password
#         dinstar@127.0.0.1, set password as local_password
#         dinstar@%, set password as remote_password
###########################################################################################

import sys
import MySQLdb
import datetime
import commands

########################################### glb vars ######################################

if len( sys.argv ) < 7:
    # Usage: db-sync-users.py <host> <root> <password> <dinstar> <local_password> <remote_password>
    print "Usage: %s <host> <root> <password> <dinstar> <local_password> <remote_password> " % ( sys.argv[ 0 ] )
    print "       check and create users: "
    print "       root@127.0.0.1, set password as <local_password> "
    print "       dinstar@localhost, set password as <local_password> "
    print "       dinstar@127.0.0.1, set password as <local_password> "
    print "       dinstar@%, set password as <remote_password> "
    sys.exit()

db_name = "mysql"
db_host = sys.argv[ 1 ]
root_user = sys.argv[ 2 ]
root_password = sys.argv[ 3 ]
db_user = sys.argv[ 4 ]
local_password = sys.argv[ 5 ]
remote_password = sys.argv[ 6 ]

########################################### def func ######################################

# get param from system conf file
def get_xml_param_val( param ):
   my_cmd = "cat %s | grep %s | awk '{print $3}' | cut -f 2 -d '\"'" % ( system_conf, param )
   my_ret = commands.getoutput( my_cmd )
   return my_ret;

# check and create user
def update_db_user_by_root( my_user, my_host, my_password ):
    my_ret  = ''
    my_sql = ''

    # get user
    # SQL CMD
    my_sql = "SELECT User FROM user WHERE User='%s' and Host='%s';" % ( my_user, my_host )
    ###print "  my_sql = %s" % ( my_sql )
    try:
        # SQL RUN
        cursor.execute( my_sql )
        # SQL RESULT
        results = cursor.fetchall()
        for row in results:
            my_ret = row[ 0 ]
    except:
        print "  Error: get user %s@%s!" % ( my_user, my_host )
        return -1

    # check and create user
    if my_ret == '':
        # SQL CMD
        my_sql = "CREATE USER '%s'@'%s' IDENTIFIED BY '%s';" % ( my_user, my_host, my_password )
        ###print "  my_sql = %s" % ( my_sql )
        try:
            # SQL RUN
            cursor.execute( my_sql )
            print "  create user %s@%s" % ( my_user, my_host )
        except:
            print "  Error: create user %s@%s!" % ( my_user, my_host )
            return -1

    # update password
    # SQL CMD
    my_sql = "UPDATE mysql.user SET authentication_string=password('%s') WHERE User='%s' and Host='%s';" % ( my_password, my_user, my_host )
    ###print "  my_sql = %s" % ( my_sql )
    try:
        # SQL RUN
        cursor.execute( my_sql )
        print "  update password %s@%s" % ( my_user, my_host )
    except:
        print "  Error: update password %s@%s!" % ( my_user, my_host )
        return -1

    # update privileges
    # SQL CMD
    my_sql = "FLUSH PRIVILEGES;"
    ###print "  my_sql = %s" % ( my_sql )
    try:
        # SQL RUN
        cursor.execute( my_sql )
        print "  flush privileges to %s@%s " % ( my_user, my_host )
    except:
        print "  Error: flush privileges to %s@%s!" % ( my_user, my_host )
        return -1

    # update grant all
    # SQL CMD
    my_sql = "GRANT ALL ON *.* TO '%s'@'%s';" % ( my_user, my_host )
    ###print "  my_sql = %s" % ( my_sql )
    try:
        # SQL RUN
        cursor.execute( my_sql )
        print "  grant all to %s@%s" % ( my_user, my_host )
    except:
        print "  Error: grant all to %s@%s!" % ( my_user, my_host )
        return -1

    # update privileges
    # SQL CMD
    my_sql = "FLUSH PRIVILEGES;"
    ###print "  my_sql = %s" % ( my_sql )
    try:
        # SQL RUN
        cursor.execute( my_sql )
        print "  flush privileges to %s@%s" % ( my_user, my_host )
    except:
        print "  Error: flush privileges to %s@%s!" % ( my_user, my_host )
        return -1

    return 0


# check and delete user
def drop_db_user_by_root( my_user, my_host ):
    my_ret  = ''
    my_sql = ''

    # get user
    # SQL CMD
    my_sql = "SELECT User FROM user WHERE User='%s' and Host='%s';" % ( my_user, my_host )
    ###print "  my_sql = %s" % ( my_sql )
    try:
        # SQL RUN
        cursor.execute( my_sql )
        # SQL RESULT
        results = cursor.fetchall()
        for row in results:
            my_ret = row[ 0 ]
    except:
        print "  Error: get user %s@%s!" % ( my_user, my_host )
        return -1

    # check and drop user
    if my_ret == '':
        print "  no user %s@%s" % ( my_user, my_host )
    else:
        # drop user
        # SQL CMD
        my_sql = "DROP USER '%s'@'%s';" % ( my_user, my_host )
        ###print "  my_sql = %s" % ( my_sql )
        try:
            # SQL RUN
            cursor.execute( my_sql )
            print "  drop user %s@%s" % ( my_user, my_host )
        except:
            print "  Error: drop user %s@%s!" % ( my_user, my_host )
            return -1
    
    return 0


# check and update procedures
def update_db_procedures_by_root( my_user, my_host, my_proc ):
    my_ret  = ''
    my_sql = ''

    # get db_user@% by my_proc
    # SQL CMD
    my_sql = "SELECT REPLACE( definer, '@%%', '' ) FROM proc WHERE name ='%s' LIMIT 1;" % ( my_proc )
    ###print "  my_sql = %s" % ( my_sql )
    try:
        # SQL RUN
        cursor.execute( my_sql )
        # SQL RESULT
        results = cursor.fetchall()
        for row in results:
            my_ret = row[ 0 ]
    except:
        print "  Error: get procedure %s!" % ( my_proc )
        return -1

    # check and update procedures
    if my_ret == '':
        print "  no procedure like %s" % ( my_proc )
    elif my_ret == my_user:
        print "  no change procedures like %s" % ( my_proc )
    else:
        print "  old user: %s, new user: %s" % ( my_ret, my_user )
        # drop old user
        drop_db_user_by_root( my_ret, "localhost" )
        drop_db_user_by_root( my_ret, "127.0.0.1" )
        drop_db_user_by_root( my_ret, "%" )
        
        # update procedures
        # SQL CMD
        my_sql = "UPDATE proc SET definer='%s@%s' WHERE definer='%s@%%';" % ( my_user, my_host, my_ret )
        ###print "  my_sql = %s" % ( my_sql )
        try:
            # SQL RUN
            cursor.execute( my_sql )
            print "  update procedures like %s" % ( my_proc )
        except:
            print "  Error: update procedures like %s!" % ( my_proc )
            return -1

    return 0
            

########################################### main proc ######################################

# connect to db
db = MySQLdb.connect( db_host, root_user, root_password, db_name, charset='utf8' )

# get cursor
cursor = db.cursor()


# check root@127.0.0.1
update_db_user_by_root( root_user, "127.0.0.1", local_password )

# check dinstar@localhost
update_db_user_by_root( db_user, "localhost", local_password )

# check dinstar@127.0.0.1
update_db_user_by_root( db_user, "127.0.0.1", local_password )

# check dinstar@%
update_db_user_by_root( db_user, "%", remote_password )

# check procedures like PROC_CREATE_TREE
update_db_procedures_by_root( db_user, "%", "PROC_CREATE_TREE" )


# close connect
db.close()


