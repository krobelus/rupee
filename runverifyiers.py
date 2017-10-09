#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil

files = open("instances-" + sys.argv[1] + ".txt").read().splitlines()
for f in files:
    os.popen("condor_submit jobs/" + f + ".vDT.sub")
    os.popen("condor_submit jobs/" + f + ".vSD.sub")
    os.popen("condor_submit jobs/" + f + ".vFD.sub")
