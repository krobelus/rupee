#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil

solversPath = ""  # TODO	
checkersPath = ""  # TODO
formulasPath = "" # TODO
proofsPath = ""  # TODO
witnessPath = "" # TODO
outputsPath = "" # TODO
timeout = 5000

def tryRemove(file):
	if os.path.isFile(file):
		os.remove(file)

def solverPath(solver):
	return solversPath + solver

def checkerPath(checker):
	return checkersPath + checker

def formulaPath(instance):
	return formulasPath  + instance + ".cnf"

def proofPath(solver, instance):
	return proofsPath + instance + "." + solver + ".drat"

def lratPath(solver, instance, checker):
	return witnessPath + instance + "." + solver + "."  + checker + ".lrat"

def sickPath(solver, instance, checker):
	return witnessPath + instance + "." + solver + "."  + checker + ".sick"

def solverOutputPath(solver, instance):
	return outputsPath + instance + "." + solver + ".SS.out"

def checkerOutputPath(solver, instance, checker):
	return proofsPath + instance + "." + solver + "."  + checker + ".out"

def validatorOutputPath(solver, instance, checker):
	return proofsPath + instance + "." + solver + ".v"  + checker + ".out"

def coqOutputPath(solver, instance, checker):
	return proofsPath + instance + "." + solver + ".c"  + checker + ".out"

def solverResult(solver, instance): # TODO: sat/unsat/to/mo/null
	return "null"

def checkerResult(solver, instance, checker): #TODO accept/reject/to/mo/null
	return "null"

def solverCommand(solver, instance):
	return "./" + solversPath(solver) + " " + formulaPath(solver, instance) + " " + proofPath(solver, instance) + " > " + solverOutputPath(solver, instance)

def checkerCommand(solver, instance, checker):
	if checker == "DT":
		return "./" + checkerPath("drat-trim") + " " + formulaPath(solver, instance) + " " + proofPath(solver, instance) +
		" -L " +  lratPath(solver, instance, checker) + " -t " + str(timeout) + " > " + checkerOutputPath(solver, instance, checker)
	elif checker == "SD":
		return "./" + checkerPath("rupee") + " " + formulaPath(solver, instance) + " " + proofPath(solver, instance) +
		" -lrat " + lratPath(solver, instance, checker)  +" -recheck " + sickPath(solver, instance, checker) + " -skip-deletion > " +
		checkerOutputPath(solver, instance, checker)
	elif  checker == "FD":
		return "./" + checkerPath("rupee") + " " + formulaPath(solver, instance) + " " + proofPath(solver, instance) +
		" -lrat " + lratPath(solver, instance, checker)  +" -recheck " + sickPath(solver, instance, checker) + " -full-deletion > " +
		checkerOutputPath(solver, instance, checker)

def validatorCommand(solver, instance, checker, result):
	if result == "accept":
		return "./" + checkerPath("lratcheck") + " " + formulaPath(solver, instance) + " " + lratPath(solver, instance, checker) +
		" > " + validatorOutputPath(solver, instance, checker)
	if result == "reject":
		return "./" + checkerPath("sickcheck") + " " + formulaPath(solver, instance) + " " + proofPath(solver, instance) + " "  +
		sickPath(solver, instance, checker) + " > " + validatorOutputPath(solver, instance, checker)

def coqCommand(solver, instance, checker, result):
	if result  == "accept":
		return "./" + checkerPath("coq-lrat-check") + " " + formulaPath(solver, instance) + " " + lratPath(solver, instance, checker) +
		" > " + coqOutputPath(solver, instance, checker)
	if result == "reject":
		return "./" + checkerPath("coq-sick-check") + " " + formulaPath(solver, instance) + " " + proofPath(solver, instance) + " "  +
		sickPath(solver, instance, checker) + " > " + coqOutputPath(solver, instance, checker)

def cleanFiles(solver, instance):
	tryRemove(proofPath(solver, instance))
	tryRemove(solverOutputPath(solver, instance))
	for checker in ["DT", "SD", "FD"]:
		tryRemove(lratPath(solver, instance, checker))
		tryRemove(sickPath(solver, instance, checker))
		tryRemove(checkerOutputPath(solver, instance, checker))
		tryRemove(validatorOutputPath(solver, instance, checker))
		tryRemove(coqOutputPath(solver, instance, checker))

def runSolver(solver, instance):
	os.popen(solverCommand(solver, instance))

def checkAndValidateDratTrim(solver, instance):
	if solverResult(solver, instance) == "unsat":
		checkerCommand(solver, instance, "DT")
		result = checkerResult(solver, instance, "DT")
		if result == "accept":
			validatorCommand(solver, instance, "DT", result)
			coqCommand(solver, instance, "DT", result)
	tryRemove(proofPath(solver, instance))
	tryRemove(lratPath(solver, instance, checker))
	tryRemove(sickPath(solver, instance, checker))

def checkAndValidateRupee(solver, instance, deletion):
	if deletion == "full":
		deletion = "FD"
	else:
		deletion = "SD"
	if solverResult(solver, instance) == "unsat":
		checkerCommand(solver, instance, deletion)
		result = checkerResult(solver, instance, deletion)
		if result == "accept" or result == "reject":
			validatorCommand(solver, instance, deletion, result)
			coqCommand(solver, instance, deletion, result)
	tryRemove(proofPath(solver, instance))
	tryRemove(lratPath(solver, instance, checker))
	tryRemove(sickPath(solver, instance, checker))

solverName = sys.argv[1]
instanceName = sys.argv[2]
cleanFiles(solverName, instanceName)
runSolver(solverName, instanceName)
checkAndValidateDratTrim(solverName, instanceName)
checkAndValidateRupee(solverName, instanceName, "full")
checkAndValidateRupee(solverName, instanceName, "skip")
tryRemove(proofPath(solver, instance))