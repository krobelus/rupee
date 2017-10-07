#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil

def generate(f, k):
    wr = open("jobs/" + str(k) + ".sub", "w")
    wr.write("""# Unix submit description file

universe                = vanilla
request_cpus            = 1
request_memory          = 20G
request_disk            = 10G
executable              = experiment.py
arguments               = """ + f + """ -delete
output                  = """ + f + """.condout
transfer_input_files    = """ + f + ".cnf," + f + """.drat
should_transfer_files   = Yes
when_to_transfer_output = ON_EXIT
queue""")
    wr.close()

def clean():
    shutil.rmtree("jobs")
    os.mkdir("jobs")

clean()
files = open(sys.argv[1]).read().splitlines()
k = 0
for f in files:
    k = k + 1
    generate(f[2:], k)
    # os.popen("condor_submit jobs/" + str(k) + ".sub")
    # transfer_output_files   = """ + f + ".out," + f + ".DT.sick," + ".SD.sick," + f + """.FD.sick
