#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil

def generate(f, k):
    wr = open("jobs/" + str(k) + ".sub", "w")
    wr.write("""# Unix submit description file\n\n
    request_cpus            = 1\n
    request_memory          = 20G\n
    request_disk            = 10G\n
    executable              = experiment.py\n
    arguments               = """ + f + """ -delete \n
    output                  = """ + f + """.condout\n
    errors                  = """ + f + """.conderr\n
    transfer_input_files    = """ + f + ".cnf," + f + """.drat\n
    transfer_output_files   = """ + f + ".out," + f + ".DT.sick," + ".SD.sick," + f + """.FD.sick\n
    should_transfer_files   = Yes\n
    when_to_transfer_output = ON_EXIT\n
    queue\n""")
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
    os.popen("condor_submit jobs/" + str(k) + ".sub")
