#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil

def commonJob(f, suf):
    return "universe  = vanilla\n\
    request_cpus = 1\n\
    request_memory = 20G\n\
    request_disk = 10G\n\
    output = logs/" + f + "." + suf + ".condout\n\
    error = logs/" + f + "." + suf + ".conderr\n\
    logs = logs/" + f + "." + suf + ".condlog\n\
    should_transfer_files   = YES\n\
    when_to_transfer_output = ON_EXIT\n\
    queue\n"

def checkJobConditions(f, suf, tf):
    res = True;
    if suf == "DT" or suf == "SD" or suf == "FD":
        if not os.path.isfile("data/" + f + ".cnf"):
            print "# CNF file data/" + f + ".cnf missing while creating job " + f + "." + suf
            res = False
        if not os.path.isfile("data/" + f + ".drat"):
            print "# DRAT file data/" + f + ".drat missing while creating job " + f + "." + suf
            res = False
    elif (suf == "vDT" or suf == "cDT") and tf == "FALSE":
        res = False
    elif tf == "TRUE":
        if not os.path.isfile("data/" + f + ".cnf"):
            print "# CNF file data/" + f + ".cnf missing while creating job " + f + "." + suf
            res = False
        if not os.path.isfile("witness/" + f + "." + suf[1:2] + ".lrat"):
            print "# LRAT file witness/" + f + "." + suf[1:2] + ".lrat missing while creating job " + f + "." + suf
            res = False
    elif tf == "FALSE":
        if not os.path.isfile("data/" + f + ".cnf"):
            print "# CNF file data/" + f + ".cnf missing while creating job " + f + "." + suf
            res = False
        if not os.path.isfile("data/" + f + ".drat"):
            print "# DRAT file data/" + f + ".drat missing while creating job " + f + "." + suf
            res = False
        if not os.path.isfile("witness/" + f + "." + suf[1:2] + ".sick"):
            print "# LRAT file witness/" + f + "." + suf[1:2] + ".sick missing while creating job " + f + "." + suf
            res = False
    elif tf == "NONE":
        print "# execution over " + f + "." + suf[1:2] + "failed, skipping verification"
        res = False
    return res
        

def createJob(f, suf, res):
    common = commonJob(f, suf)
    write = checkJobConditions(f, suf, res)
    if write == True:
        if suf == "DT" or suf == "SD" or suf == "FD":
            transLine = "transfer_input_files = data/" + f + ".cnf,data/" + f + ".drat\n"
        if res == "FALSE" and (suf == "vSD" or suf == "vFD" or suf == "cSD" or suf == "cFD"):
            transLine = "transfer_input_files = data/" + f + ".cnf,data/" + f + ".drat,witness/" + f + "." + suf[1:2] + ".sick\n"
            execLine = "executable = bin/sickcheck\n"
            argsLine = "arguments = " + f + ".cnf " + f + ".drat " + f + "." + suf[1:2] + ".sick\n"
        if res == "FALSE" and (suf == "cSD" or suf == "cFD"):
            transLine = "transfer_input_files = data/" + f + ".cnf,data/" + f + ".drat,witness/" + f + "." + suf[1:2] + ".sick\n"
            # TODO: coq-sick-check executable & arguments
            # execLine = "executable = bin/sickcheck\n"
            # argsLine = "arguments = " + f + ".cnf " + f + ".drat " + f + "." + suf[1:2] + ".sick\n"
        if res == "TRUE" and (suf == "vDT" or suf == "vSD" or suf == "vFD"):
            transLine = "transfer_input_files = data/" + f + ".cnf,witness/" + f + "." + suf[1:2] + ".lrat\n"
            execLine = "executable = bin/lratcheck\n"
            argsLine = "arguments = " + f + ".cnf " + f + "." + suf[1:2] + ".lrat\n"
        if res == "TRUE" and (suf == "cDT" or suf == "cSD" or suf == "cFD"):
            transLine = "transfer_input_files = data/" + f + ".cnf,witness/" + f + "." + suf[1:2] + ".lrat\n"
            # TODO: coq-lrat-check executable & arguments
            # execLine = "executable = bin/lratcheck\n"
            # argsLine = "arguments = " + f + ".cnf " + f + "." + suf[1:2] + ".lrat\n"
        if suf == "DT":
            execLine = "executable = bin/drat-trim\n"
            argsLine = "arguments = " + f + ".cnf " + f + ".drat -L " + f + "." + suf + ".lrat -t 5000\n"
        if suf == "SD" or suf == "FD":
            execLine = "executable = bin/rupee\n"
            if suf == "SD":
                mode = "-skip-deletion"
            if suf == "FD":
                mode = "-full-deletion"
            argsLine = "arguments = " + f + ".cnf " + f + ".drat -lrat " + f + "." +  suf + ".lrat -recheck " + f + "." + suf + ".sick " + mode + "\n"
        wr = open("jobs/" + f + "." + suf + ".sub", "w")
        wr.write(execLine + argsLine + transLine + common)
        wr.close()

def submitJob(f, suf):
    if os.path.isfile("jobs/" + f + "." + suf + ".sub"):
        os.popen("condor_submit jobs/" + f + "." + suf + ".sub")
    else:
        print "# SUB job description for file "+  f + "." +  suf + " missing"

def parseResult(f):
    result = "NONE"
    if os.path.isfile("logs/" + f + ".condout"):
        o = open("logs/" + f  + ".condout")
        lines = o.read().splitlines()
        o.close()
        for line  in lines:
            if line.find("s VERIFIED") != -1:
                result = "TRUE"
            if line.find("s INCORRECT") != -1:
                result = "FALSE"
            if line.find("s NOT VERIFIED") != -1:
                result = "FALSE"
    return result


def getFiles(x):
    o = open(sys.argv[1])
    l = o.read().splitlines()
    o.close()
    return l

def moveWitnesses(files):
    for file in files:
        for suf in ["DT", "SD", "FD"]:
            for wit in [".lrat", ".sick"]:
                if os.path.isfile(f + "." + suf + wit):
                    os.rename(f + "." + suf + wit, "witness/" + f + "." + suf + wit)

def cleanFolder(folder):
    if os.path.exists(folder):
        shutil.rmtree(folder)
    os.mkdir(folder)


masterList = sys.argv[1]
stage = sys.argv[2]
fileList = getFiles(masterList)
if stage == "-check":
    cleanFolder("jobs")
    cleanFolder("logs")
    cleanFolder("witness")
    for file in fileList:
        for suf in ["DT", "SD", "FD"]:
            createJob(file, suf, "")
    for file in fileList:
        for suf in ["DT", "SD", "FD"]:
            submitJob(file, suf)
elif stage == "-verify:":
    moveWitnesses(fileList)
    for file in fileList:
        for suf in ["DT", "SD", "FD"]:
            res = parseResult(file + "." + suf[1:2])
            createJob(file, "v" + suf, res)
    for file in fileList:
        for suf in ["DT", "SD", "FD"]:
            submitJob(file, "v" + suf)
elif stage == "-coqverify":
    moveWitnesses(fileList)
    for file in fileList:
        for suf in ["DT", "SD", "FD"]:
            res = parseResult(file + "." + suf[1:2])
            createJob(file, "c" + suf, res)
    for file in fileList:
        for suf in ["DT", "SD", "FD"]:
            submitJob(file, "c" + suf)
