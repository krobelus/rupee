#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil

def generate(f, p, k):
    wr = open("jobs/" + str(k) + ".sub", "w")
    wr.write("""# Unix submit description file

universe                = vanilla
request_cpus            = 1
request_memory          = 20G
request_disk            = 10G
executable              = experiment.py
arguments               = """ + f + """ -delete
output                  = """ + f + """.condout
error                   = """ + f + """.conderr
transfer_input_files    = """ + p + f + ".cnf," + p + f + """.drat
transfer_output_files   = """ + p + f + ".out," + p + f + ".DT.sick," + p + f + ".SD.sick," + p + f + """.FD.sick
should_transfer_files   = YES
when_to_transfer_output = ON_EXIT
queue""")
    wr.close()

def clean():
    shutil.rmtree("jobs")
    os.mkdir("jobs")

clean()
files = open(sys.argv[1]).read().splitlines()
p = sys.argv[2]
k = 0
for f in files:
    k = k + 1
    generate(f, p, k)
    # os.popen("condor_submit jobs/" + str(k) + ".sub")
