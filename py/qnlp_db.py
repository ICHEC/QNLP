###############################################################################

import sqlite3
import os
from typing import Dict, Tuple

###############################################################################

"""
Class for providing DB access and output from the corpus tagging operations.
The resulting file will then be loadable using the C++ QNLP code.
"""
class qnlp_db:
    def __init__(self, db_name, db_path):
        self.db_name = db_name
        self.db_path = db_path
        self.db_full_path = os.path.join(db_path, db_name)
        self.db_conn = None
        self.create_table()

    def __del__(self):
        print("Database %s closed" % self.db_full_path)

###############################################################################

    def create_table(self):
        """
        Create the database table for tagging the required data. The DB has columns
        for type (noun, verb, etc.), bin_id (binary string representing the type),
        weight (calculable from frequency of occurrence relative to other terms),
        and mapping_dir (1 indicates string to bin_id, or -1 bin_id to string mapping).
        """
        cr_tbl = """CREATE TABLE qnlp(
                    id INTEGER PRIMARY KEY, type TEXT, 
                    name TEXT,     bin_id INTEGER, 
                    weight REAL,   mapping_dir INTEGER
                    );"""
        try:
            conn = self.connect_db()
            c = conn.cursor()
            c.execute(cr_tbl)
            conn.commit()
        except Exception as e:
            print("DB access error: {0}".format(e))
            remove_db = input("Attempt to remove the database and retry? y/n: ")
            if remove_db is "y":
                self.drop_table()
                self.create_table()
            else:
                exit()

###############################################################################

    def drop_table(self):
        """Drops the qnlp table if it exists."""
        dr_tbl = """DROP TABLE if EXISTS qnlp;"""
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
        
###############################################################################

    def db_insert(self, values: Dict, data_type="noun"):
        """
        Insert the tag to binary encoding mapping into the DB.

        values -- Dict mapping string to binary value, and binary value to string.
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
        
        for k,v in values.items():
            if not str(k).isnumeric():
                c.execute('''INSERT INTO qnlp 
                    (type, name, bin_id, weight ) VALUES(?,?,?,?)''', 
                    (data_type, k, int(v,2), 0 )
                )
                print (data_type, k, int(v,2), 0 )
        conn.commit()

###############################################################################

    def db_print(self):
        """Prints all the available data stored in the DB"""

        conn = self.connect_db()
        c = conn.cursor()

        c.execute('''SELECT type, name, bin_id, weight FROM qnlp''',
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