#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil

files = open("instances-" + sys.argv[1] + ".txt").read().splitlines()
for f in files:
    if os.path.isfile("jobs/" + f + ".DT.conderr"):
        os.popen("git add jobs/" + f + ".DT.conderr")
    if os.path.isfile("jobs/" + f + ".DT.condout"):
        os.popen("git add jobs/" + f + ".DT.condout")
    if os.path.isfile("jobs/" + f + ".vDT.conderr"):
        os.popen("git add jobs/" + f + ".vDT.conderr")
    if os.path.isfile("jobs/" + f + ".vDT.condout"):
        os.popen("git add jobs/" + f + ".vDT.condout")
    if os.path.isfile("jobs/" + f + ".SD.conderr"):
        os.popen("git add jobs/" + f + ".SD.conderr")
    if os.path.isfile("jobs/" + f + ".SD.condout"):
        os.popen("git add jobs/" + f + ".SD.condout")
    if os.path.isfile("jobs/" + f + ".vSD.conderr"):
        os.popen("git add jobs/" + f + ".vSD.conderr")
    if os.path.isfile("jobs/" + f + ".vSD.condout"):
        os.popen("git add jobs/" + f + ".vSD.condout")
    if os.path.isfile("jobs/" + f + ".FD.conderr"):
        os.popen("git add jobs/" + f + ".FD.conderr")
    if os.path.isfile("jobs/" + f + ".FD.condout"):
        os.popen("git add jobs/" + f + ".FD.condout")
    if os.path.isfile("jobs/" + f + ".vFD.conderr"):
        os.popen("git add jobs/" + f + ".vFD.conderr")
    if os.path.isfile("jobs/" + f + ".vFD.condout"):
        os.popen("git add jobs/" + f + ".vFD.condout")
