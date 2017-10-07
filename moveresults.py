#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil

if not os.path.exists("results"):
    os.makedirs("results")
files = open(sys.argv[1]).read().splitlines()
for f in files:
    os.rename(f + ".out", "results/" + f + ".out")
    os.rename(f + ".DT.sick", "results/" + f + ".DT.sick")
    os.rename(f + ".SD.sick", "results/" + f + ".SD.sick")
    os.rename(f + ".FD.sick", "results/" + f + ".FD.sick")
