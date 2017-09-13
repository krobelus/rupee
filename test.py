#! /usr/bin/env python

import os
import fnmatch

allfiles = os.listdir(os.path.dirname(os.path.abspath(__file__)) + "/proofs")
cnffiles = filter(lambda x: fnmatch.fnmatch(x, '*.cnf'), allfiles)
filenames = map(lambda x: x[0:-4], cnffiles)
for f in filenames:
    command = "./bin/rupee proofs/" + f + ".cnf proofs/" + f + ".drat -lrat proofs/" + f + ".lrat -full-deletion -recheck"
    print f
    verification = os.popen(command)
    for line in verification:
        if line == "s VERIFIED\n":
            print "DRAT proof accepted by rupee"
            # command2 = "./coq-lrat/Interface.native proofs/" + f + ".cnf proofs/" + f + ".lrat"
            # verification2 = os.popen(command2)
            # for line in verification2:
            #     if line == "True\n":
            #         print "LRAT certificate validated by Coq"
            #     if line == "False\n":
            #         print "LRAT certificate rejected by Coq"
            command2 = "./bin/lratcheck proofs/" + f + ".cnf proofs/" + f + ".lrat"
            verification2 = os.popen(command2)
            for line in verification2:
                print line
        if line == "s BUG\n":
            print "Bug detected in rupee"
        if line == "s REJECTED\n":
            print "DRAT proof rejected by rupee"
    print ""
