#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil

files = open("instance-2016.txt").read().splitlines()
if not os.path.isdir("results/"):
    os.mkdir("results/")
for f in files:
    for suf in [".DT",".SD",".FD",".vDT",".vSD",".vFD"]:
        for ext in [".conderr",".condout"]:
            if os.path.isfile(f + suf + ext):
                os.rename(f + suf + ext, "results/" + f + suf + ext)
                os.popen("git add results/" + f + suf + ext)
