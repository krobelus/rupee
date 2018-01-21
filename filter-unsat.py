#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil
import csv

csvfile = open(sys.argv[1], "rt") 
reader = csv.reader(csvfile)
unsatCnfs = []
for row in reader:
	if row[4] == "UNSAT":
		unsatCnfs.append(row[0])
unsatCnfs = list(set(unsatCnfs))
csvfile.close()
outfile = open(sys.argv[2], "w")
for x in unsatCnfs:
	outfile.write(x[4:] + "\n")
