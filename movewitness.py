#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil

files = open(sys.argv[1]).read().splitlines()
if not os.path.isdir("witness/"):
    os.mkdir("witness/")
for f in files:
    for suf in [".DT", ".SD", ".FD"]:
        for ext in [".lrat", ".sick"]:
            if os.path.isfile(f + suf + ext):
                os.rename(f + suf + ext, "witness/" + f + suf + ext)
