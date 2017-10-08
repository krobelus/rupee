#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil

files = open("instances-slice" + sys.argv[1] + ".txt").read().splitlines()
for f in files:
    os.popen("condor_submit jobs/" + f + ".DT.sub")
    os.popen("condor_submit jobs/" + f + ".SD.sub")
    os.popen("condor_submit jobs/" + f + ".FD.sub")
