#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil

files = open("instances-" + sys.argv[1] + ".txt").read().splitlines()
for f in files:
    for suf in ["DT", "SD", "FD"]:
        if os.path.isfile("jobs/" + f + ".v" + suf + ".sub"):
            os.popen("condor_submit jobs/" + f + ".v" + suf + ".sub)
