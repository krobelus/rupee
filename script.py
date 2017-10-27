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
    log = logs/" + f + "." + suf + ".condlog\n\
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
    elif tf == "NULL":
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
    result = "NULL"
    if os.path.isfile("logs/" + f + ".condout"):
        o = open("logs/" + f  + ".condout")
        lines = o.read().splitlines()
        o.close()
        for line  in lines:
            result = tryResult(line, result)
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

def analyzeDratTrimOutput(file, suf, array):
    result = "NULL"
    time = "NULL"
    if os.path.isfile("logs/" + f + "." + suf + ".condout"):
        o = open("logs/" + f + "." + suf + ".condout")
        lines = o.read().splitlines()
        o.close()
        for line in lines:
            result = tryResult(line, result)
            time = tryDratTrimTime(line, time)
    else:
        print "# drat-trim output missing for instance " + file
    array.append(result)
    array.append(time)

def analyzeRupeeResult(file, suf, array):
    result = "NULL"
    noPremises = "NULL"
    noInstructions = "NULL"
    noRats = "NULL"
    noDeletions = "NULL"
    noSkipped = "NULL"
    noReasons = "NULL"
    totalTime = "NULL"
    deletionTime = "NULL"
    if os.path.isfile("logs/" + f + "." + suf + ".condout"):
        o = open("logs/" + f + "." + suf + ".condout")
        lines = o.read().splitlines()
        o.close()
        for line in lines:
            result = tryResult(line, result)
            noPremises = tryPrefixParse(line, noPremises, "nf ")
            noInstructions = tryPrefixParse(line, noInstructions, "np ")
            noRats = tryPrefixParse(line, noRats, "nr ")
            noDeletions = tryPrefixParse(line, noDeletions, "dc ")
            noSkipped = tryPrefixParse(line, noSkipped, "ds ")
            noReasons = tryPrefixParse(line, noReasons, "dr ")
            totalTime = tryPrefixParse(line, totalTime, "tt ")
            deletionTime = tryPrefixParse(line, deletionTime, "td ")
    else:
        print "# drat-trim output missing for instance " + file
    array.append(result)
    array.append(noPremises)
    array.append(noInstructions)
    array.append(noRats)
    array.append(noDeletions)
    array.append(noSkipped)
    array.append(noReasons)
    array.append(totalTime)
    array.append(deletionTime)

def analyzeVerifierOutput(file, suf, res, array):
    result = "NULL"
    time = "NULL"
    if os.path.isfile("logs/" + f + "." + suf + ".condout"):
        o = open("logs/" + f + "." + suf + ".condout")
        lines = o.read().splitlines()
        o.close()
        for line in lines:
            result = tryResult(line, result)
            time = tryPrefixParse(line, time, "t ")
    elif res == "TRUE" or (res == "FALSE" and suf[1:2] != "DT"):
        print "# C validation output missing for instance " + file + " with suffix " + suf[1:2]
    array.append(result)
    array.append(time)

def tryResult(line, result):
    if line.find("s VERIFIED") != -1:
        return "TRUE"
    elif line.find("s INCORRECT") != -1:
        return "FALSE"
    elif line.find("s NOT VERIFIED") != -1:
        return "FALSE"
    elif line.find("s TIMEOUT") != -1:
        return "TIMEOUT"
    else return result

def tryDratTrimTime(line, time):
    if(line[0:21]) == "c verification time: ":
        return(str(float(line[21:-8]) * 1000))
    else
        return result

def tryPrefixParse(line, result, pref):
    length = len(pref)
    if line[0:length] == pref:
        return line[length:]
    else:
        return result

def extractData(array):
    newarray = []
    for row in array:
        file = row[0]
        size = "NULL"
        if os.path.isfile("data/" + file + ".cnf") and os.path.isfile("data/" + file + ".drat"):
            size = str(os.path.getsize("data/" + file + ".cnf") + os.path.getsize("data/" + file + ".drat"))
        else:
            print "# CNF or DRAT file for " + file + " missing"
        nopremises = "NULL"
        if row[8] != "NULL":
            nopremises = row[8]
        elif row[21] != "NULL":
            nopremises = row[21]
        else:
            print "# number of premises was not measured for " + file
        noinstructions = "NULL"
        if row[9] != "NULL":
            noinstructions = row[9]
        elif row[22] != "NULL":
            noinstructions = row[22]
        else:
            print "# number of instructions was not measured for " + file
        norats = "NULL"
        if row[10] != "NULL":
            norats = row[10]
        elif row[23] != "NULL":
            norats = row[23]
        else:
            print "# number of RATs was not measured for " + file
        nodeletions = "NULL"
        if row[11] != "NULL":
            nodeletions = row[11]
        elif row[24] != "NULL":
            nodeletions = row[24]
        else:
            print "# number of deletions was not measured for " + file
        dtresult = row[1]
        dttime = row[2]
        dtcvresult = row[3]
        dtcvtime = row[4]
        dtcoqresult = row[5]
        dtcoqtime = row[6]
        sdresult = row[7]
        sdtime = row[14]
        sdskipped = row[12]
        sdreasons = row[13]
        sdcvresult = row[16]
        sdcvtime = row[17]
        sdcoqresult = row[18]
        sdcoqtime = row[19]
        fdresult = row[20]
        fdtime = row[27]
        fddeltime = row[28]
        fdreasons = row[26]
        fdcvresult = row[29]
        fdcvtime = row[30]
        fdcoqresult = row[31]
        fdcoqtime = row[32]
        newarray.append([file, size, nopremises, noinstructions, norats, nodeletions,
            dtresult, dttime, dtcvresult, dtcvtime, dtcoqresult, dtcoqtime, sdresult,
            sdtime, sdskipped, sdreasons, sdcvtime, sdcvtime, sdcoqresult, sdcoqtime,
            fdresult, fdtime, fddeltime, fdreasons, fdcvresult, fdcvtime, fdcoqresult,
            fdcoqtime])
    return newarray

def printTsvData(array, path):
    wr = open(path, "w")
    for row in array:
        first = True
        for elem in array:
            if first == True:
                wr.write("\t")
                first = False
            wr.write(elem)
        wr.write("\n")
    wr.close()
    print "# printed data to " + path

def analyzeData(array):
    print "# nothing to analyze"

def localCheck(file, suf):
    if suf == "DT":
        os.popen("./bin/drat-trim data/" + file + ".cnf data/" + file + ".drat -L witness/" + file + ".DT.lrat -t 5000 > logs/" + file + ".DT.condout")
    elif suf == "SD":
        os.popen("./bin/rupee data/" + file + ".cnf data/" + file + ".drat -lrat witness/" + file + ".SD.lrat -recheck witness/" + file + ".SD.sick -skip-deletion > logs/" + file + ".SD.condout")
    elif suf == "FD":
        os.popen("./bin/rupee data/" + file + ".cnf data/" + file + ".drat -lrat witness/" + file + ".FD.lrat -recheck witness/" + file + ".FD.sick -full-deletion > logs/" + file + ".FD.condout")

def localVerify(file, suf):
    result = parseResult(file + "." + suf)
    if result == "TRUE":
        os.popen("./bin/lrat-check data/" + file + ".cnf witness/" + file + "." + suf + ".lrat > logs/" + file + ".v" + suf + ".condout")
    elif result == "FALSE" and suf != "DT":
        os.popen("./bin/sick-check data/" + file + ".cnf data/" + file + ".drat witness/" + file + "." + suf + ".sick > logs/" + file + ".v" + suf + ".condout")

def localCoqVerify(file, suf):
    result = parseResult(file + "." + suf)
    if result == "TRUE":
        os.popen("./bin/coq-lrat-check data/" + file + ".cnf witness/" + file + "." + suf + ".lrat > logs/" + file + ".c" + suf + ".condout")
    elif result == "FALSE" and suf != "DT":
        os.popen("./bin/coq-sick-check data/" + file + ".cnf data/" + file + ".drat witness/" + file + "." + suf + ".sick > logs/" + file + ".c" + suf + ".condout")

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
            res = parseResult(file + "." + suf)
            createJob(file, "v" + suf, res)
    for file in fileList:
        for suf in ["DT", "SD", "FD"]:
            submitJob(file, "v" + suf)
elif stage == "-coqverify":
    moveWitnesses(fileList)
    for file in fileList:
        for suf in ["DT", "SD", "FD"]:
            res = parseResult(file + "." + suf)
            createJob(file, "c" + suf, res)
    for file in fileList:
        for suf in ["DT", "SD", "FD"]:
            submitJob(file, "c" + suf)
elif stage == "-localcheck":
    for file in fileList:
        for suf in ["DT", "SD", "FD"]:
            localCheck(file, suf)
elif stage == "-localverify":
    for file in fileList:
        for suf in ["DT", "SD", "FD"]:
            localVerify(file, suf)
elif stage == "-localcoqverify":
    for file in fileList:
        for suf in ["DT", "SD", "FD"]:
            localCoqVerify(file, suf)
elif stage == "-analyze":
    array = []
    for file in fileList:
        fileArray = [file]
        for suf in ["DT", "SD", "FD"]:
            if suf == "DT":
                analyzeDratTrimOutput(file, suf, fileArray)
            else:
                analyzeRupeeOutput(file, suf, fileArray)
            res = parseResult(file + "." + suf)
            analyzeVerifierOutput(file, "v" + suf, res, fileArray)
            analyzeVerifierOutput(file, "c" + suf, res, fileArray)
        array.append(fileArray)
    dataArray = extractData(array)
    printTsvData(dataArray, "logs/results.out")
    analyzeData(dataArray)

# Results:
# file name
# combined size
# no. premise clauses
# no. instructions
# no. RAT introduction instructions
# no. deletion instructions
# DRAT-trim result
# DRAT-trim checking time
# DRAT-trim validation result (C)
# DRAT-trim validation time (C)
# DRAT-trim validation result (Coq)
# DRAT-trim validation time (Coq)
# rupee-SD result
# rupee-SD checking time
# rupee-SD no. skipped deletion instructions
# rupee-SD no. reason deletion instructions
# rupee-SD validation result (C)
# rupee-SD validation time (C)
# rupee-SD validation result (Coq)
# rupee-SD validation time (Coq)
# rupee-FD result
# rupee-FD checking time
# rupee-FD deletion time
# rupee-FD no. reason deletion instructions
# rupee-FD validation result (C)
# rupee-FD validation time (C)
# rupee-FD validation result (Coq)
# rupee-FD validation time (Coq)

# 00: file
# 01: DT result
# 02: DT time
# 03: DT C verif result
# 04: DT C verif time
# 05: DT Coq verif result
# 06: DT Coq verif time
# 07: SD result
# 08: SD no. premises
# 09: SD no. instructions
# 10: SD no. RATs
# 11: SD no. deletions
# 12: SD no. skipped
# 13: SD no. reasons
# 14: SD time
# 15: SD deletion time
# 16: SD C verif result
# 17: SD C verif time
# 18: SD Coq verif result
# 19: SD Coq verif time
# 20: FD result
# 21: FD no. premises
# 22: FD no. instructions
# 23: FD no. RATs
# 24: FD no. deletions
# 25: FD no. skipped
# 26: FD no. reasons
# 27: FD time
# 28: FD deletion time
# 29: FD C verif result
# 30: FD C verif time
# 31: FD Coq verif result
# 32: FD Coq verif time