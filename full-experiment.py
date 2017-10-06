#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil

def generate(f):
    wr = open("jobs/" + f + ".sub")
    wr.write("# Unix submit description file\n")
    wr.write("\n")
    wr.write("executable              = ../experiment.py\n")
    wr.write("arguments               = \"" + f + "\"\n")
    wr.write("output                  = " + f + ".condout\n")
    wr.write("errors                  = " + f + ".conderr\n")
    wr.write("transfer_input_files    = " + f + ".cnf," + f + ".drat\n")
    wr.write("transfer_output_files   = " + f + ".out," + f + ".DT.sick" + ".SD.sick," + f + ".FD.sick\n")
    wr.write("should_transfer_files   = Yes\n")
    wr.write("when_to_transfer_output = ON_EXIT\n")
    wr.write("queue\n")

def clean():
    shutil.rmtree("jobs")
    os.mkdir("jobs")

clean()
files = open(sys.args[1]).read().splitlines()
for f in files:
    generate(f)
    # os.popen("grep -v \"^[0-9]* 0\" " + f + ".DT.temp | sort -n > " + f + ".DT.lrat")
