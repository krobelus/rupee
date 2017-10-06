#! /usr/bin/env python

import os
import sys
import fnmatch
import time

def drattrim_check(comm1, comm2, f):
    start = time.time()
    output = os.popen(comm1).read().splitlines()
    ttime = str(int(1000*(time.time() - start)))
    result = "FAIL"
    verif = "FAIL"
    check_time = "FAIL"
    for line in output:
        if line.find("s VERIFIED") != -1:
            result = "TRUE"
        if line.find("s NOT VERIFIED") != -1:
            result = "FALSE"
    if result == "FAIL":
        ttime = "FAIL"
    if result == "TRUE":
        os.popen("grep -v \"^[0-9]* 0\" proofs/" + f + ".ulrat | sort -n > proofs/" + f + ".lrat")
        os.popen("tail proofs/" + f + ".ulrat | grep \"^[0-9]* 0\" >> proofs/" + f + ".lrat")
        os.popen("rm proofs/" + f + ".ulrat")
        output = os.popen(comm2).read().splitlines()
        for line in output:
            if line.find("s VERIFIED") != -1:
                verif = "TRUE"
            if line.find("s INCORRECT") != -1:
                verif = "FALSE"
    else:
        verif = "FAIL"
    return [result, ttime, verif]

def rupee_result(line, prev):
    if line.find("s VERIFIED") != -1:
        return "TRUE"
    elif line.find("s REJECTED") != -1:
        return "FALSE"
    else:
        return prev

def rupee_parameter(line, pref, prev):
    if line[:2] == pref:
        return line[3:]
    else:
        return prev

def rupee_check(comm):
    output = os.popen(comm).read().splitlines()
    result = "FAIL"
    nopremises = "FAIL"
    noinferences = "FAIL"
    norats = "FAIL"
    nodels = "FAIL"
    noskips = "FAIL"
    noreasons = "FAIL"
    mstotal = "FAIL"
    msparsing = "FAIL"
    mspreprocessing = "FAIL"
    mschecking = "FAIL"
    msdeletion = "FAIL"
    for line in output:
        result = rupee_result(line, result)
        nopremises = rupee_parameter(line, "nf", nopremises)
        noinferences = rupee_parameter(line, "np", noinferences)
        norats = rupee_parameter(line, "nr", norats)
        nodels = rupee_parameter(line, "dc", nodels)
        noskips = rupee_parameter(line, "ds", noskips)
        noreasons = rupee_parameter(line, "dr", noreasons)
        mstotal = rupee_parameter(line, "tt", mstotal)
        msparsing = rupee_parameter(line, "tp", msparsing)
        mspreprocessing = rupee_parameter(line, "tf", mspreprocessing)
        mschecking = rupee_parameter(line, "tb", mschecking)
        msdeletion = rupee_parameter(line, "td", msdeletion)
    return [result, nopremises, noinferences, norats, nodels, noskips, noreasons,
            mstotal, msparsing, mspreprocessing, mschecking, msdeletion]

def rupee_verify(commyes, commno, res):
    result = "FAIL"
    time = "FAIL"
    if res[0] == "TRUE":
        output = os.popen(commyes).read().splitlines()
    elif res[0] == "FALSE":
        output = os.popen(commno).read().splitlines()
    for line in output:
        if line.find("s VERIFIED") != -1:
            result = "TRUE"
        if line.find("s INCORRECT") != -1:
            result = "FALSE"
        if line[:1] == "t":
            time = line[2:]
    if result == "FAIL":
        time = "FAIL"
    return res + [result, time]

def prettyprint(res):
    print "\nSTATS"
    print "--------"
    print "instance name: " + res[31]
    print "combined size: " + res[32]
    print "no. premises: " + res[18]
    print "no. inferences: " + res[19]
    print "no. RATs: " + res[20]
    print "\nDRAT-TRIM"
    print "--------"
    print "result: " + res[0]
    print "total time: " + res[1]
    print "verification result: " + res[2]
    print "\nRUPEE-SD"
    print "--------"
    print "result: " + res[3]
    print "no. deletions: " + res[7]
    print "no. skipped deletions: " + res[8]
    print "no. reason deletions: " + res[9]
    print "total time: " + res[10]
    print "parsing time: " + res[11]
    print "preprocessing time: " + res[12]
    print "checking time: " + res[13]
    print "deletion time: " + res[14]
    print "verification result: " + res[15]
    print "verification time: " + res[16]
    print "\nRUPEE-FD"
    print "--------"
    print "result: " + res[17]
    print "no. deletions: " + res[21]
    print "no. skipped deletions: " + res[22]
    print "no. reason deletions: " + res[23]
    print "total time: " + res[24]
    print "parsing time: " + res[25]
    print "preprocessing time: " + res[26]
    print "checking time: " + res[27]
    print "deletion time: " + res[28]
    print "verification result: " + res[29]
    print "verification time: " + res[30]

allfiles = os.listdir(os.path.dirname(os.path.abspath(__file__)) + "/proofs")
cnffiles = filter(lambda x: fnmatch.fnmatch(x, '*.cnf'), allfiles)
filenames = map(lambda x: x[0:-4], cnffiles)
for f in filenames:
    drattrim_command = "./bin/drat-trim proofs/" + f + ".cnf proofs/" + f + ".drat -L proofs/" + f + ".ulrat"
    rupeesd_command = "./bin/rupee proofs/" + f + ".cnf proofs/" + f + ".drat -lrat proofs/" + f + ".lrat -skip-deletion -recheck proofs/" + f + ".sick"
    rupeefd_command = "./bin/rupee proofs/" + f + ".cnf proofs/" + f + ".drat -lrat proofs/" + f + ".lrat -full-deletion -recheck proofs/" + f + ".sick"
    lrat_command = "./bin/lratcheck proofs/" + f + ".cnf proofs/" + f + ".lrat"
    sick_command = "./bin/sickcheck proofs/" + f + ".cnf proofs/" + f + ".drat proofs/" + f + ".sick"
    print "INSTANCE " + f + "\n"
    drattrim_result = drattrim_check(drattrim_command, lrat_command, f)
    rupeefd_result = rupee_check(rupeefd_command)
    rupeefd_result = rupee_verify(lrat_command, sick_command, rupeefd_result)
    rupeesd_result = rupee_check(rupeesd_command)
    rupeesd_result = rupee_verify(lrat_command, sick_command, rupeesd_result)
    name = f
    size = str(os.path.getsize("proofs/" + f + ".cnf") + os.path.getsize("proofs/" + f + ".drat"))
    final_result = drattrim_result + rupeesd_result + rupeefd_result + [name, size]
    prettyprint(final_result)
    print "\n\n"
    w = open("proofs/" + f + ".out", 'w')
    for k in [31, 32, 18, 19, 20, 0, 1, 2, 3, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30]:
        w.write(final_result[k] + "\n")
    w.close()
