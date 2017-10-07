#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil

def generate(f, k):
    wr = open("jobs/" + str(k) + ".sub", "w")
    wr.write("# Unix submit description file\n")
    wr.write("\n")
    wr.write("executable              = experiment.py\n")
    wr.write("arguments               = " + f + " -delete \n")
    wr.write("output                  = " + f + ".condout\n")
    wr.write("errors                  = " + f + ".conderr\n")
    wr.write("transfer_input_files    = " + f + ".cnf," + f + ".drat\n")
    wr.write("transfer_output_files   = " + f + ".out," + f + ".DT.sick" + ".SD.sick," + f + ".FD.sick\n")
    wr.write("should_transfer_files   = Yes\n")
    wr.write("when_to_transfer_output = ON_EXIT\n")
    wr.write("queue\n")
    wr.close()

def clean():
    shutil.rmtree("jobs")
    os.mkdir("jobs")

clean()
files = open(sys.argv[1]).read().splitlines()
k = 0
for f in files:
    k = k + 1
    generate(f, k)
    os.popen("condor_submit jobs/" + f + ".sub")
