#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil

def verificationType(f, suf):
    o = open("results/" + f + "." + suf ".condout")
    oo = o.read().splitlines()
    o.close()
    result = "NONE"
    if suf == "DT":
        for line in oo:
            if line.find("s VERIFIED") != -1:
                result = "TRUE"
    else:
        for line in oo:
            if line.find("s VERIFIED") != -1:
                result = "TRUE"
            if line.find("s INCORRECT") != -1:
                result = "FALSE"
    return result

def LRATjob(f, suf):
    wr = open("jobs/" + f + ".v" + suf + ".sub", "w")
    wr.write("""# Unix submit description file

universe                = vanilla
request_cpus            = 1
request_memory          = 20G
request_disk            = 10G
executable              = bin/lratcheck
arguments               = """ + f + ".cnf " + f + "." + suf + """.lrat
output                  = """ + f + ".v" + suf + """.condout
error                   = """ + f + ".v" + suf + """.conderr
transfer_input_files    = data/""" + f + ".cnf,witness/" + f + "." + suf + """.lrat
should_transfer_files   = YES
when_to_transfer_output = ON_EXIT
queue""")
    wr.close()

def SICKjob(f, suf):
    wr = open("jobs/" + f + ".v" + suf + ".sub", "w")
    wr.write("""# Unix submit description file

universe                = vanilla
request_cpus            = 1
request_memory          = 20G
request_disk            = 10G
executable              = bin/sickcheck
arguments               = """ + f + ".cnf " + f + ".drat " + f + "." + suf + """.lrat
output                  = """ + f + ".v" + suf + """.condout
error                   = """ + f + ".v" + suf + """.conderr
transfer_input_files    = data/""" + f + ".cnf,data/" + f + ".drat,witness/" + f + "." + suf + """.sick
should_transfer_files   = YES
when_to_transfer_output = ON_EXIT
queue""")
    wr.close()

files = open(sys.argv[1]).read().splitlines()
for f in files:
    for suf in ["DT", "SD", "FD"]:
        type = verificationType(f, suf)
        if type == "TRUE":
            if os.path.isfile("witness/" + f + "." + suf + ".lrat"):
                LRATjob(f, suf)
            else:
                print "missing file: witness" + f + "." + suf + ".lrat"
        if type == "FALSE":
            if os.path.isfile("witness/" + f + "." + suf + ".sick"):
                SICKjob(f, suf)
            else:
                print "missing file: witness" + f + "." + suf + ".sick"
