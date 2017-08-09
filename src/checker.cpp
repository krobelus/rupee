#include <stdlib.h>

#include "structs.hpp"
#include "extra.hpp"

namespace Checker {

//-------------------------------
// Memory management
//-------------------------------

bool allocate(checker& c) {
    #ifdef VERBOSE
    Blablabla::log("Initializing checking data structures");
    Blablabla::increase();
    #endif
    if(!WatchList::allocate(c.watch)) { return false; }
    if(!Model::allocate(c.stack)) { return false; }
    if(!Revision::allocate(c.cone)) { return false; }
    if(!Latency::allocate(c.rat)) { return false; }
    if(!Witness::allocate(c.tvr)) { return false; }
    if(!Clause::allocate(c.res)) { return false; }
    if(!Recheck::allocate(c.kk)) { return false; }
    #ifdef VERBOSE
    Blablabla::decrease();
    #endif
    return true;
}

void deallocate(checker& c) {
    WatchList::deallocate(c.watch);
    Model::deallocate(c.stack);
    Revision::deallocate(c.cone);
    Latency::deallocate(c.rat);
    Witness::deallocate(c.tvr);
    Clause::deallocate(c.res);
    Recheck::deallocate(c.kk);
}

//-------------------------------
// Stage checking
//-------------------------------

bool isFinished(checker& c) {
    return (c.stage == Constants::StageCorrect) || (c.stage == Constants::StageIncorrect);
}

bool isVerified(checker& c) {
    return (c.stage == Constants::StageCorrect);
}

bool isBuggy(checker& c) {
    return (c.stage == Constants::StageBug);
}

//-------------------------------
// Main loops
//-------------------------------

bool preprocessProof(checker& c, proof& r, database& d) {
    #ifdef VERBOSE
    Blablabla::log("Preprocessing proof forward");
    Blablabla::increase();
    Blablabla::logModel(Objects::Checker.stack);
    #endif
    c.stage = Constants::StagePreprocessing;
    c.position = 0;
    while(c.stage == Constants::StagePreprocessing) {
        if(!Proof::nextInstruction(r, c.position, c.offset, c.pivot, c.kind)) {
            #ifdef VERBOSE
            Blablabla::log("End of proof reached without unit propagation contradiction");
            #endif
            c.stage = Constants::StageIncorrect;
        } else {
            c.pointer = Database::getPointer(d, c.offset);
            if(c.kind == Constants::InstructionDeletion) {
                if(!Checker::preprocessDeletion(c, d)) { return false; }
            } else {
                if(!Checker::preprocessIntroduction(c, d)) { return false; }
                if(Model::isContradicted(c.stack)) {
                    #ifdef VERBOSE
                    Blablabla::log("Contradiction by unit propagation found, closing proof");
                    #endif
                    if(!Proof::closeProof(r, d, c.position)) { return false; }
                    Proof::nextInstruction(r, c.position, c.offset, c.pivot, c.kind);
                    c.pointer = Database::getPointer(d, c.offset);
                    if(!Checker::preprocessIntroduction(c, d)) { return false; }
                    c.stage = Constants::StageVerifying;
                }
            }
        }
    }
    #ifdef VERBOSE
    Blablabla::logDatabase(d);
    Blablabla::logProof(r, d);
    Blablabla::decrease();
    #endif
    return true;
}

bool verifyProof(checker& c, proof& r, database& d) {
    #ifdef VERBOSE
    Blablabla::log("Verifying proof backwards");
    Blablabla::increase();
    #endif
    c.position = r.used;
    while(c.stage == Constants::StageVerifying) {
        if(!Proof::prevInstruction(r, c.position, c.offset, c.pivot, c.kind)) {
            #ifdef VERBOSE
            Blablabla::log("Start of proof reached without incorrect inferences detected");
            #endif
            c.stage = Constants::StageCorrect;
        } else {
            c.pointer = Database::getPointer(d, c.offset);
            if(c.kind == Constants::InstructionDeletion) {
                if(!Checker::verifyDeletion(c, d)) { return false; }
            } else {
                if(!Checker::verifyIntroduction(c,d)) { return false; }
            }
        }
    }
    #ifdef VERBOSE
    Blablabla::decrease();
    #endif
    return true;
}

bool recheckInstruction(checker& c, database& d, proof& r) {
    if(Parameters::recheck) {
        #ifdef VERBOSE
        Blablabla::log("Rechecking instruction " + std::to_string(c.kk.instruction));
        Blablabla::increase();
        #endif
        c.stage = Constants::StageRecheck;
        recheckFormula(c, d, r);
        if(c.stage == Constants::StageRecheck) {
            #ifdef VERBOSE
            Blablabla::log("Instruction: +++ " + Blablabla::clauseToString(Database::getPointer(d, c.kk.clause)));
            #endif
            recheckNaturalModel(c, d);
            if(c.stage == Constants::StageRecheck) {
                recheckResolventModel(c, d);
                if(c.stage == Constants::StageRecheck) {
                    c.stage = Constants::StageReject;
                }
            }
        }
        #ifdef VERBOSE
        Blablabla::decrease();
        #endif
    } else {
        c.stage = Constants::StageReject;
    }
    return true;
}

//-------------------------------
// Preprocessing commands
//-------------------------------

bool preprocessIntroduction(checker& c, database& d) {
    #ifdef VERBOSE
    Blablabla::log("Preprocessing instruction:  +++" + Blablabla::clauseToString(c.pointer));
    Blablabla::increase();
    #endif
    Database::setClauseActive(c.pointer);
    if(!WatchList::insertWatches(c.watch, c.stack, c.offset, c.pointer)) { return false; }
    if(!Model::propagateModel(c.stack, c.watch, d, Constants::HardPropagation)) { return false; }
    #ifdef VERBOSE
    Blablabla::decrease();
    #endif
    return true;
}

bool preprocessDeletion(checker& c, database& d) {
    #ifdef VERBOSE
    Blablabla::log("Preprocessing instruction:  ---" + Blablabla::clauseToString(c.pointer));
    Blablabla::increase();
    #endif
    Database::setClauseInactive(c.pointer);
    WatchList::removeWatches(c.watch, c.offset, c.pointer);
    if(Parameters::deletionMode == Constants::DeletionModeSkip) {
        #ifdef VERBOSE
        Blablabla::log("Deletion instruction correct on DRAT-trim mode.");
        #endif
    } else {
        if(Database::isFlag(c.pointer, Constants::PseudounitBit, Constants::ReasonFlag)) {
            #ifdef VERBOSE
            if(Parameters::deletionMode == Constants::DeletionModeUPRedundant) {
                Blablabla::log("Deleting a reason clause, checking UP equivalence");
            } else {
                Blablabla::log("Deleting a reason clause, revising model");
            }
            #endif
            if(!Revision::getCone(c.cone, c.stack, d, c.pointer)) { return false; }
            if(!Revision::reviseCone(c.cone, c.watch, c.stack, d)) { return false; }
            if(!Model::propagateModel(c.stack, c.watch, d, Constants::HardPropagation)) { return false; }
            if(Parameters::deletionMode == Constants::DeletionModeUPRedundant) {
                if(Revision::checkCone(c.cone, c.stack)) {
                    #ifdef VERBOSE
                    Blablabla::log("Deletion instruction correct on UP-redundancy mode");
                    Blablabla::logRevision(c.cone, d);
                    #endif
                } else {
                    c.stage = Constants::StageIncorrect;
                    #ifdef VERBOSE
                    Blablabla::log("Deletion instruction incorrect on UP-redundancy mode");
                    #endif
                }
            } else {
                Revision::resetCone(c.cone);
                #ifdef VERBOSE
                Blablabla::log("Deletion instruction correct on unrestricted deletion mode");
                #endif
            }
        } else {
            #ifdef VERBOSE
            Blablabla::log("Deleting a passive clause, nothing to check");
            #endif
            if(!Revision::voidRevision(c.cone)) { return false; }
        }
    }
    #ifdef VERBOSE
    Blablabla::decrease();
    #endif
    return true;
}

//-------------------------------
// Verification commands
//-------------------------------

bool verifyIntroduction(checker& c, database& d) {
    #ifdef VERBOSE
    Blablabla::log("Verifying instruction:  +++" + Blablabla::clauseToString(c.pointer));
    Blablabla::increase();
    #endif
    Database::setClauseInactive(c.pointer);
    WatchList::removeWatches(c.watch, c.offset, c.pointer);
    Model::unpropagateModel(c.stack, d, c.pointer);
    #ifdef VERBOSE
    Blablabla::logModel(c.stack);
    #endif
    if(Database::isFlag(c.pointer, Constants::VerificationBit, Constants::ScheduledFlag)) {
        if(!checkInference(c, d)) { return false; }
    } else {
        #ifdef VERBOSE
        Blablabla::log("Clause is not scheduled for verification, skipping");
        #endif
    }
    #ifdef VERBOSE
    Blablabla::decrease();
    #endif
    return true;
}

bool verifyDeletion(checker& c, database& d) {
    #ifdef VERBOSE
    Blablabla::log("Verifying instruction:  ---" + Blablabla::clauseToString(c.pointer));
    Blablabla::increase();
    #endif
    Database::setClauseActive(c.pointer);
    if(Parameters::deletionMode == Constants::DeletionModeSkip) {
        if(!WatchList::insertWatches(c.watch, c.stack, c.offset, c.pointer)) { return false; }
        #ifdef VERBOSE
        Blablabla::log("Nothing to do in DRAT-trim mode.");
        #endif
    } else {
        Revision::applyRevision(c.cone, c.stack, d);
        if(!WatchList::insertWatches(c.watch, c.stack, c.offset, c.pointer)) { return false; }
        if(!Revision::alignCone(c.cone, c.watch, c.stack, d)) { return false; }
        Revision::resetCone(c.cone);
        Witness::recordDeletion(c.tvr, c.offset);
    }
    #ifdef VERBOSE
    Blablabla::decrease();
    #endif
    return true;
}

//-------------------------------
// Rechecking commands
//-------------------------------

void recheckFormula(checker& c, database& d, proof& r) {
    #ifdef VERBOSE
    Blablabla::log("Checking accumulated formula");
    Blablabla::increase();
    #endif
    c.position = 0;
    while(c.position < c.kk.instruction) {
        Proof::nextInstruction(r, c.position, c.offset, c.pivot, c.kind);
        c.pointer = Database::getPointer(d, c.offset);
        if(c.kind == Constants::InstructionDeletion) {
            Database::setFlag(c.pointer, Constants::RecheckBit, Constants::InactiveFlag);
        } else {
            Database::setFlag(c.pointer, Constants::RecheckBit, Constants::ActiveFlag);
        }
    }
    if(!Database::recheckActivity(d)) {
        #ifdef VERBOSE
        Blablabla::log("Incorrect accumulated formula");
        #endif
        c.stage = Constants::StageBug;
    } else {
        #ifdef VERBOSE
        Blablabla::log("Accumulated formula is correct");
        #endif
    }
    #ifdef VERBOSE
    Blablabla::decrease();
    #endif
}

void recheckNaturalModel(checker& c, database& d) {
    #ifdef VERBOSE
    Blablabla::log("Checking natural model");
    Blablabla::increase();
    Blablabla::log("Natural model:");
    Blablabla::logRecheckModel(c.kk.natlits);
    #endif
    if(!Recheck::checkClause(c.kk, d) || !Database::checkUpModel(d, c.kk.natlits)) {
        c.stage = Constants::StageBug;
    } else {
        #ifdef VERBOSE
        Blablabla::log("Natural model is correct");
        #endif
    }
    #ifdef VERBOSE
    Blablabla::decrease();
    #endif
}

void recheckResolventModel(checker& c, database& d) {
    #ifdef VERBOSE
    Blablabla::log("Checking resolvent model");
    Blablabla::increase();
    Blablabla::log("Resolvent model:");
    Blablabla::logRecheckModel(c.kk.reslits);
    Blablabla::log("Resolution clause: " + Blablabla::clauseToString(Database::getPointer(d, c.kk.resolvent)));
    #endif
    if(!Recheck::checkResolvent(c.kk, d) || !Database::checkUpModel(d, c.kk.reslits)) {
        c.stage = Constants::StageBug;
    } else {
        #ifdef VERBOSE
        Blablabla::log("Resolvent model is correct");
        #endif
    }
    #ifdef VERBOSE
    Blablabla::decrease();
    #endif
}


//-------------------------------
// RUP/RAT checks
//-------------------------------

int* itptr;
int lit;
bool check;

void precheckRup(checker& c, database& d, int* pointer, bool& result) {
    #ifdef VERBOSE
    Blablabla::increase();
    #endif
    if(Model::isContradicted(c.stack)) {
        #ifdef VERBOSE
        Blablabla::log("Current formula is UP-unsatisfiable");
        #endif
        Witness::extractDependencies(c.tvr, c.stack, d, Constants::ConflictLiteral);
        result = Constants::CheckCorrect;
    } else {
        #ifdef VERBOSE
        Blablabla::log("Assumming complementary literals");
        #endif
        for(itptr = pointer; (lit = *itptr) != Constants::EndOfList; ++itptr) {
            if(!Model::assumeLiteral(c.stack, -lit)) {
                #ifdef VERBOSE
                Blablabla::log("Assumming literal " + Blablabla::litToString(lit) + " contradicts partial model");
                #endif
                Witness::extractDependencies(c.tvr, c.stack, d, lit);
                result = Constants::CheckCorrect;
            }
        }
    }
}

bool checkRup(checker& c, database& d, bool& result, bool* copy, int counterpivot, long reason) {
    if(result == Constants::CheckIncorrect) {
        if(counterpivot != Constants::ConflictLiteral) {
            Model::propagateLiteral(c.stack, counterpivot, reason, Database::getPointer(d, reason), Constants::SoftPropagation);
        }
        if(!Model::propagateModel(c.stack, c.watch, d, Constants::SoftPropagation)) { return false; }
        if(Model::isContradicted(c.stack)) {
            #ifdef VERBOSE
            Blablabla::log("Reached soft contradiction");
            #endif
            Witness::extractDependencies(c.tvr, c.stack, d, Constants::ConflictLiteral);
            result = Constants::CheckCorrect;
        } else {
            #ifdef VERBOSE
            Blablabla::log("UP-stable model found");
            #endif
            if(Parameters::recheck) {
                Recheck::copyModel(c.stack, copy);
            }
            result = Constants::CheckIncorrect;
        }
    }
    #ifdef VERBOSE
    Blablabla::decrease();
    #endif
    Model::resetModel(c.stack);
    return true;
}

bool checkInference(checker& c, database& d) {
    if(!Witness::openWitness(c.tvr, c.offset)) { return false; }
    check = Constants::CheckIncorrect;
    if(Database::isFlag(c.pointer, Constants::OriginalityBit, Constants::PremiseFlag)) {
        #ifdef VERBOSE
        Blablabla::log("Clause is a premise, skipping");
        #endif
        if(!Witness::setRupWitness(c.tvr)) { return false; }
        check = Constants::CheckCorrect;
    } else {
        #ifdef VERBOSE
        Blablabla::log("Checking RUP property");
        #endif
        precheckRup(c, d, c.pointer, check);
        if(!checkRup(c, d, check, c.kk.natlits, Constants::ConflictLiteral, 0)) { return false; }
        if(check == Constants::CheckCorrect) {
            #ifdef VERBOSE
            Blablabla::log("RUP check succeeded");
            #endif
            if(!Witness::setRupWitness(c.tvr)) { return false; }
            if(!Witness::dumpAndSchedule(c.tvr, c.stack, d)) { return false; }
        } else {
            #ifdef VERBOSE
            Blablabla::log("RUP check failed");
            #endif
            if(c.pivot != Constants::ConflictLiteral) {
                #ifdef VERBOSE
                Blablabla::log("Checking RAT property upon literal " + Blablabla::litToString(c.pivot));
                #endif
                if(!Witness::setRatWitness(c.tvr, c.pivot)) { return false; }
                if(!Latency::collectCandidates(c.rat, c.watch, d, c.pivot)) { return false; }
                Clause::setInnerClause(c.res, c.pointer, c.pivot);
                #ifdef VERBOSE
                Blablabla::increase();
                #endif
                if(!Latency::checkResolventsRup(c.rat, c, c.res, d, c.pivot, check)) { return false; }
                #ifdef VERBOSE
                Blablabla::decrease();
                if(check == Constants::CheckCorrect) {
                    Blablabla::log("RAT check succeeded");
                } else {
                    Blablabla::log("RAT check failed");
                }
                #endif
            } else {
                #ifdef VERBOSE
                Blablabla::log("No RAT pivot provided");
                #endif
            }
        }
    }
    if(check == Constants::CheckCorrect) {
        if(!Witness::closeWitness(c.tvr)) { return false; }
    } else {
        c.stage = Constants::StageIncorrect;
        if(Parameters::recheck) {
            c.kk.instruction = c.position;
        }
    }
    return true;
}

}
