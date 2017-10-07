#! /usr/bin/env python

import os
import sys
import fnmatch
import time

def drattrim_check(f):
    start = time.time()
    output = os.popen(drattrim_command(f)).read().splitlines()
    ttime = str(int(1000*(time.time() - start)))
    result = "FAIL"
    for line in output:
        if line.find("s VERIFIED") != -1:
            result = "TRUE"
        if line.find("s NOT VERIFIED") != -1:
            result = "FALSE"
    if result == "FAIL":
        ttime = "NONE"
    return [result, ttime]

def drattrim_sort(f, res):
    if res == "TRUE":
        os.popen("grep -v \"^[0-9]* 0\" " + f + ".DT.temp | sort -n > " + f + ".DT.lrat")
        os.popen("tail " + f + ".DT.temp | grep \"^[0-9]* 0\" >> " + f + ".DT.lrat")
    os.remove(f + ".DT.temp")

def verify(f, suf, res):
    if res == "FAIL":
        [verif, ttime] = ["NONE", "NONE"]
    elif res == "TRUE":
        [verif, ttime] = c_lratcheck(f, suf)
        # [verif, ttime] = coq_lratcheck(f, suf)
    else:
        if os.path.isfile(f + "." + suf + ".sick"):
            [verif, ttime] = c_sickcheck(f, suf)
            # [verif, ttime] = coq_sickcheck(f, suf)
        else:
            [verif, ttime] = ["NONE", "NONE"]
    return [verif, ttime]

def c_lratcheck(f, suf):
    output = os.popen(c_lratcheck_command(f, suf)).read().splitlines()
    result = "FAIL"
    for line in output:
        if line.find("s VERIFIED") != -1:
            result = "TRUE"
        if line.find("s INCORRECT") != -1:
            result = "FALSE"
        if line[:1] == "t":
            ttime = line[2:]
    if result == "FAIL":
        ttime = "FAIL"
    return [result, ttime]

def c_sickcheck(f, suf):
    output = os.popen(c_sickcheck_command(f, suf)).read().splitlines()
    result = "FAIL"
    for line in output:
        if line.find("s VERIFIED") != -1:
            result = "TRUE"
        if line.find("s INCORRECT") != -1:
            result = "FALSE"
        if line[:1] == "t":
            ttime = line[2:]
    if result == "FAIL":
        ttime = "FAIL"
    return [result, ttime]

def coq_lratcheck(f, suf):
    start = time.time()
    output = os.popen(coq_lratcheck_command(f, suf)).read().splitlines()
    ttime = str(int(1000*(time.time() - start)))
    result = "FAIL"
    for line in output:
        if line.find("True") != -1:
            result = "TRUE"
        if line.find("False") != -1:
            result = "FALSE"
    if result == "FAIL":
        ttime = "FAIL"
    return [result, ttime]

def coq_sickcheck(f, suf):
    start = time.time()
    output = os.popen(coq_sickcheck_command(f, suf)).read().splitlines()
    ttime = str(int(1000*(time.time() - start)))
    result = "FAIL"
    for line in output:
        if line.find("True") != -1:
            result = "TRUE"
        if line.find("False") != -1:
            result = "FALSE"
    if result == "FAIL":
        ttime = "FAIL"
    return [result, ttime]

def delete_files(f, suf, flag):
    if (flag != "-preserve") and os.path.isfile(f + "." + suf + ".lrat"):
        os.remove(f + "." + suf + ".lrat")

def rupeesd_check(f):
    output = os.popen(rupeesd_command(f)).read().splitlines()
    return parse_rupee(output)

def rupeefd_check(f):
    output = os.popen(rupeefd_command(f)).read().splitlines()
    return parse_rupee(output)

def parse_rupee(out):
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
    for line in out:
        if line.find("s VERIFIED") != -1:
            result = "TRUE"
        if line.find("s REJECTED") != -1:
            result = "FALSE"
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

def rupee_parameter(line, pref, prev):
    if line[:2] == pref:
        return line[3:]
    else:
        return prev

def drattrim_command(f):
    return "/home/arebolap/rupee/bin/drat-trim " + f + ".cnf " + f + ".drat -L " + f + ".DT.temp"

def rupeesd_command(f):
    return "/home/arebolap/rupee/bin/rupee " + f + ".cnf " + f + ".drat -lrat " + f + ".SD.lrat -skip-deletion -recheck " + f + ".SD.sick"

def rupeefd_command(f):
    return "/home/arebolap/rupee/bin/rupee " + f + ".cnf " + f + ".drat -lrat " + f + ".FD.lrat -full-deletion -recheck " + f + ".FD.sick"

def c_lratcheck_command(f, suf):
    return "/home/arebolap/rupee/bin/lratcheck " + f + ".cnf " + f + "." + suf + ".lrat"

def c_sickcheck_command(f, suf):
    return "/home/arebolap/rupee/bin/sickcheck " + f + ".cnf " + f + ".drat " + f + "." + suf + ".sick"

def coq_lratcheck_command(f, suf):
    return "/home/arebolap/rupee/bin/coq-lrat-check " + f + ".cnf " + f + "." + suf + ".lrat"

def coq_sickcheck_command(f, suf):
    return "/home/arebolap/rupee/bin/coq-sick-check " + f + ".cnf " + f + ".drat " + f + "." + suf + ".sick"

def write_line(w, x, l):
    print l + ": " + x
    w.write(x + "\n")

def write_results(f, size, dt_result, dt_verif, sd_result, sd_verif, fd_result, fd_verif):
    w = open(f + ".out", 'w')
    print "\n\nSTATS"
    print "--------"
    write_line(w, f, "instance name")
    write_line(w, size, "combined size")
    write_line(w, fd_result[1], "no. premises")
    write_line(w, fd_result[2], "no. inferences")
    write_line(w, fd_result[3], "no. RATs")
    print "\nDRAT-TRIM"
    print "--------"
    write_line(w, dt_result[0], "result")
    write_line(w, dt_result[1], "total time")
    write_line(w, dt_verif[0], "verification result")
    write_line(w, dt_verif[1], "verification time")
    print "\nRUPEE-SD"
    print "--------"
    write_line(w, sd_result[0], "result")
    write_line(w, sd_result[4], "no. deletions")
    write_line(w, sd_result[5], "no. skipped deletions")
    write_line(w, sd_result[6], "no. skipped reasons")
    write_line(w, sd_result[7], "total time")
    write_line(w, sd_result[8], "parsing time")
    write_line(w, sd_result[9], "checking time")
    write_line(w, sd_result[10], "deletion time")
    write_line(w, sd_verif[0], "verification result")
    write_line(w, sd_verif[1], "verification time")
    print "\nRUPEE-FD"
    print "--------"
    write_line(w, fd_result[0], "result")
    write_line(w, fd_result[4], "no. deletions")
    write_line(w, fd_result[5], "no. skipped deletions")
    write_line(w, fd_result[6], "no. skipped reasons")
    write_line(w, fd_result[7], "total time")
    write_line(w, fd_result[8], "parsing time")
    write_line(w, fd_result[9], "checking time")
    write_line(w, fd_result[10], "deletion time")
    write_line(w, fd_verif[0], "verification result")
    write_line(w, fd_verif[1], "verification time")
    print "\n\n"
    w.close()

f = sys.argv[1]
flag = sys.argv[2]
print "[" + flag + "]"
size = str(os.path.getsize( f + ".cnf") + os.path.getsize( f + ".drat"))
print "INSTANCE " + f + "\n"
print "\texecuting drat-trim"
dt_result = drattrim_check(f)
print "\tverifying"
drattrim_sort(f, dt_result[0])
dt_verif = verify(f, "DT", dt_result[0])
delete_files(f, "DT", flag)
print "\texecuting rupee-sd"
sd_result = rupeesd_check(f)
print "\tverifying"
sd_verif = verify(f, "SD", sd_result[0])
delete_files(f, "SD", flag)
print "\texecuting rupee-fd"
fd_result = rupeefd_check(f)
print "\tverifying"
fd_verif = verify(f, "FD", fd_result[0])
delete_files(f, "FD", flag)
write_results(f, size, dt_result, dt_verif, sd_result, sd_verif, fd_result, fd_verif)
