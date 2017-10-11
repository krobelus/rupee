#! /usr/bin/env python

import os
import sys
import fnmatch
import time

path = sys.argv[1]
arglist = sys.argv[2:]

allfiles = filter(lambda x: os.path.isfile(path + x), os.listdir(path))
dratfiles = filter(lambda x: x[-4:] == "drat", allfiles)
instances = map(lambda x: path + x[:-5], dratfiles)
print instances
args = ""
for arg in arglist:
	args = args + " " + arg
for instance in instances:
	if os.path.isfile(instance + ".lrat"):
		os.remove(instance + ".lrat")
	if os.path.isfile(instance + ".sick"):
		os.remove(instance + ".sick")
	out = os.popen("bin/rupee " + instance + ".cnf " + instance + ".drat -lrat " + instance + ".lrat -recheck " + instance + ".sick " + args)
	print out.read()
	out.close()
