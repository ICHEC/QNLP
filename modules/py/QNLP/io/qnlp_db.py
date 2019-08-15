###############################################################################

import sqlite3
import os
from typing import Dict, Tuple

###############################################################################

class qnlp_db:
    """
    Class for providing DB access and output from the corpus tagging operations.
    The resulting file will then be loadable using the C++ QNLP code.
    """
    def __init__(self, db_name, db_path):
        self.db_name = db_name
        self.db_path = db_path
        self.db_full_path = os.path.join(db_path, db_name)
        self.db_conn = None      

###############################################################################

    def create_table(self, table_name="qnlp"):
        """
        Create the database table for tagging the required data. The DB has columns
        for type (noun, verb, etc.), bin_id (binary string representing the type),
        weight (calculable from frequency of occurrence relative to other terms),
        and mapping_dir (1 indicates string to bin_id, or -1 bin_id to string mapping).
        """
        cr_tbl = """CREATE TABLE {}(
                    id INTEGER PRIMARY KEY, type TEXT, 
                    name TEXT,     bin_id INTEGER, 
                    weight REAL,   mapping_dir INTEGER
                    );""".format(table_name)

        conn = self.connect_db()
        c = conn.cursor()

        try:
            c.execute(cr_tbl)

        except sqlite3.OperationalError as oe:
            remove_db = input("Table '{}' already exists. Remove? y/n: ".format(table_name))
            if remove_db is "y":
                self.drop_table(table_name)
                self.create_table(table_name)

        except Exception as e:
            print("SQLITE exception thrown: {0}".format(e), "Exiting program.")
            exit()

        finally:
            conn.commit()


###############################################################################

    def drop_table(self, table_name="qnlp"):
        """Drops the table if it exists."""
        dr_tbl = """DROP TABLE if EXISTS {};""".format(table_name)
        try:
            conn = self.connect_db()
            c = conn.cursor()
            c.execute(dr_tbl)
            conn.commit()

        except Exception as e:
            print("DB access error: {0}".format(e))

###############################################################################

    def connect_db(self):
        """If there is an active DB connection, return it. If not, create one."""
        if self.db_conn is not None:
            return self.db_conn
        try:
            self.db_conn = sqlite3.connect(self.db_full_path+".sqlite")

            return self.db_conn
        except Exception as e:
            print("DB access error: {0}".format(e))
        return None

###############################################################################

    def close_db(self):
        """If there is an active connection, close it."""
        if self.db_conn:
            self.db_conn.close()
            self.db_conn = None
        print("Database %s closed" % self.db_full_path)
        
###############################################################################

    def db_insert(self, values: Dict, data_type="noun", table_name="qnlp"):
        """
        Insert the tag to binary encoding mapping into the DB.

        values      -- Dict mapping string to binary value, and binary value to string.
        data_type   -- String to indicate the type of data to be stored
        table_name  -- Name of table to store in DB
        """

        #Proposed modification; weight set to 0 currently
        '''
        The DB insert operation below assumes the value field of a key in DB is a tuple,
        containing (binary_id, weight of occurrence), where weight of occurrence cant be
        determined by the proximity of the word to other words; essentially a count in the 
        simplest case. The mapping checks to see if the index is convertible to a numeric
        type. If so, this will imply the reverse mapping (ie qubit result to string val), 
        and is indicated by -1. Otherwise, this will be a forward mapping, and given by 1.
        '''
        conn = self.connect_db()
        c = conn.cursor()
        self.create_table(table_name)
        
        for k,v in values.items():
            if not str(k).isnumeric():
                c.execute('''INSERT INTO {} 
                    (type, name, bin_id, weight ) VALUES(?,?,?,?)'''.format(table_name), 
                    (data_type, k, int(v,2), 0 )
                )
                print (data_type, k, int(v,2), 0 )
        conn.commit()

###############################################################################


    def db_load(self, data_type="noun", table_name="qnlp", direction="forward"):
        """
        Load the tag to binary encoded mapping into from DB.

        data_type   -- Data type to load from the DB (noun, verb, etc)
        table_name  -- Database table to load data
        direction   -- Direction of the returned mapping (forward: tag -> int, reverse: int -> tag)
        """

        conn = self.connect_db()
        c = conn.cursor()

        c.execute('''SELECT name, bin_id FROM {} WHERE type=?'''.format(table_name), (data_type,) )
    
        all_rows = c.fetchall()
        dat = {}

        for r in all_rows:
            if( direction == "forward" ):
                dat.update({r[0] : [r[1]]})
            elif( direction == "reverse" ):
                #No need to carry second term as list for reverse mapping
                dat.update({r[1] : r[0]})
            else:
                print("Direction not understood. Please try again")
                exit()

        return dat

###############################################################################

    def db_print(self, table_name="qnlp"):
        """Prints all the available data stored in the DB"""

        conn = self.connect_db()
        c = conn.cursor()

        c.execute('''SELECT type, name, bin_id, weight FROM {}'''.format(table_name),
                    #        WHERE type=? AND bin_id=? AND mapping=?''', 
                    #    ()
        )
        print("################################################################")
        print("type\t\tbin_id\t\tweight\t\tmapping_dir")
        print("################################################################")
        all_rows = c.fetchall()
        for row in all_rows:
            print('{0}\t\t{1}\t\t{2}\t\t{3}'.format(row[0], row[1], row[2], row[3] ))

###############################################################################