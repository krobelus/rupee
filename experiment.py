#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil

def DTjob(f):
    wr = open("jobs/" + f + ".DT.sub", "w")
    wr.write("""# Unix submit description file

universe                = vanilla
request_cpus            = 1
request_memory          = 20G
request_disk            = 10G
executable              = bin/drat-trim
arguments               = """ + f + ".cnf " + f + ".drat -L " + f + """.DT.temp -t 5000
output                  = """ + f + """.DT.condout
error                   = """ + f + """.DT.conderr
transfer_input_files    = data/""" + f + ".cnf,data/" + f + """.drat
should_transfer_files   = YES
when_to_transfer_output = ON_EXIT
queue""")
    wr.close()

def SDjob(f):
    wr = open("jobs/" + f + ".SD.sub", "w")
    wr.write("""# Unix submit description file

universe                = vanilla
request_cpus            = 1
request_memory          = 20G
request_disk            = 10G
executable              = bin/rupee
arguments               = """ + f + ".cnf " + f + ".drat -lrat " + f + ".SD.lrat -skip-deletion -recheck " + f + """.SD.sick
output                  = """ + f + """.SD.condout
error                   = """ + f + """.SD.conderr
transfer_input_files    = data/""" + f + ".cnf,data/" + f + """.drat
should_transfer_files   = YES
when_to_transfer_output = ON_EXIT
queue""")
    wr.close()

def SDjob(f):
    wr = open("jobs/" + f + ".SD.sub", "w")
    wr.write("""# Unix submit description file

universe                = vanilla
request_cpus            = 1
request_memory          = 20G
request_disk            = 10G
executable              = bin/rupee
arguments               = """ + f + ".cnf " + f + ".drat -lrat " + f + ".FD.lrat -full-deletion -recheck " + f + """.FD.sick
output                  = """ + f + """.FD.condout
error                   = """ + f + """.FD.conderr
transfer_input_files    = data/""" + f + ".cnf,data/" + f + """.drat
should_transfer_files   = YES
when_to_transfer_output = ON_EXIT
queue""")
    wr.close()

def clean():
    shutil.rmtree("jobs")
    os.mkdir("jobs")

clean()
files = open(sys.argv[1]).read().splitlines()
for f in files:
    DTjob(f)
    SDjob(f)
    FDjob(f)
