#include <iostream>
#include <string>

#include "structs.hpp"
#include "extra.hpp"
#include "database.hpp"
#include "proof.hpp"

namespace ProofReader {

// bool triggered;
// int triglit;
int* ptr;
int literal;
bool conflict;
long* candidate;

bool allocate(proofreader& c) {
    if(!WatchList::allocate(c.watch)) { return false; }
    if(!Model::allocate(c.stack)) { return false; }
    if(!Revision::allocate(c.cone)) { return false; }
    if(!Latency::allocate(c.rat)) { return false; }
    if(!Chain::allocate(c.tvr)) { return false; }
    return true;
}

void deallocate(proofreader& c) {
    WatchList::deallocate(c.watch);
    Trail::deallocate(c.stack);
}

void getPremise(checker& c, proof& r, database& d) {
	c.offset = r.proofarray[c.position];
	c.pointer = Database::getPointer(d, c.offset);
}

void getInstruction(checker& c, proof& r, database& d) {
	c.offset = r.proofarray[c.position];
	c.pointer = Database::getPointer(d, c.offset);
	c.pivot = r.proofpivots[c.position];
	c.kind = c.pivot % 2;
	c.pivot >>= 1;
}

bool initialize(checker& c, proof& r, database& d) {
    Blablabla::log("Processing premise clauses.");
    Blablabla::increase();
    Model::assumeLiteral(c.stack, Constants::ReservedLiteral);
    if(!Model::hardPropagate(c.stack, c.watch, d)) { return false; }
    for(c.position = 0; c.position < r.noPremises; ++c.position) {
        getPremise(c, r, d);
        if(!introduceClause(c, d)) { return false; }
    }
    Blablabla::decrease();
    return true;
}

bool introduceClause(checker &c, database& d) {
    Database::setClauseActive(d, c.pointer);
    Blablabla::log("Introducing clause " + Blablabla::clauseToString(c.pointer) + ".");
    if(Database::isEmpty(c.pointer)) {
        return WatchList::addConflict(c.watch, c.pointer, c.offset);
    } else {
        if(!WatchList::setWatches(c.watch, c.stack, c.pointer, c.offset)) { return false; }
        return Model::hardPropagate(c.stack, c.watch, d);
    }
}

bool deleteClause(checker &c, database& d) {
    Database::setClauseInactive(d, c.pointer);
    Blablabla::log("Deleting clause " + Blablabla::clauseToString(c.pointer) + ".");
    Blablabla::increase();
    WatchList::findAndRemoveWatch(c.watch, c.pointer[0], c.offset);
    WatchList::findAndRemoveWatch(c.watch, c.pointer[1], c.offset);
    if(Database::getFlag(c.pointer, Constants::ConflictBit, Constants::ConflictFlag)) {
        Database::setFlag(c.pointer, Constants::ConflictBit, Constants::SatisfiableFlag);
        WatchList::findAndRemoveWatch(c.watch, Constants::ConflictWatchlist, c.offset);
    }
    if(Database::getFlag(c.pointer, Constants::PseudounitBit, Constants::ReasonFlag)) {
        Blablabla::log("Clause was a reason clause.");
        Revision::getCone(c.cone, c.stack, d, c.pointer);
        Revision::reviseList(c.cone, c.watch, c.stack, d);
    } else {
        Blablabla::log("Clause was not a reason clause.");
    }
    Blablabla::decrease();
}

bool checkRup(checker &c, database& d, int* clause, bool& rup) {
    Blablabla::log("Checking RUP for clause " + Blablabla::clauseToString(clause) + ".");
    Blablabla::increase();
    if(rup = WatchList::isConflict(c.watch)) {
        Blablabla::log("Clause follows from current model by ex falso quodlibet.");
    } else {
        for(ptr = clause; (literal = *ptr) != Constants::EndOfClause; ++ptr) {
            if(Model::isSatisfied(c.stack, literal)) {
                Blablabla::log("Clause subsumed by current model.");
                Model::reset(c.stack);
                rup = true;
            } else if(!Model::isFalsified(c.stack, literal)) {
                Model::assumeLiteral(m, -literal);
            }
        }
        if(!rup) {
            if(!Model::softPropagate(m, wl, d, rup)) { return false; }
        }
    }
    if(rup) {
        Blablabla::log("RUP check succeeded.");
    } else {
        Blablabla::log("RUP check failed.");
    }
    Blablabla::decrease();
    return true;
}

bool checkRat(checker &c, database& d, int* clause, int pivot, bool& rat) {
    rat = true;
    Blablabla::log("Checking RAT for clause " + Blablabla::clauseToString(clause) + " with pivot " + Blablabla::litToString(pivot) + ".");
    Blablabla::increase();
    if(Latency::loadClause(c.rat, clause, pivot)) { return false; }
    Latency::findResolvableClauses(c.rat, c.watch, d);
    Blablabla::logResolutionCandidates(c.rat, d);
    candidate = c.rat.array;
    while(rat && *candidate != Constants::EndOfWatchList) {
        Blablabla::log("Resolution candidate " + Blablabla::clauseToString(*candidate) ".");
        Blablabla::increase();
        if(Latency::computeResolvent(c.rat, Database::getPointer(*candidate))) {
            if(!checkRup(c, d, c.rat.resolvent, rat)) { return false; }
        } else {
            Blablabla::log("Resolvent is tautological.");
        }
        Blablabla::decrease();
    }
    if(rat) {
        Blablabla::log("RAT check succeeded.");
    } else {
        Blablabla::log("RAT check failed.");
    }
    Blablabla::decrease();
    return true;
}

bool preprocessInstruction(checker& c, proof& r, database& d, bool& stop) {
    Blablabla::log("Preprocessing instruction " + Blablabla::instructionToString(r, d, c.position) + ".");
    Blablabla::increase();
    getInstruction(c, r, d);
    if(c.kind == Constants::InstructionDeletion) {
        if(!deleteClause(c, d)) { return false; }
    } else {
        if(!introduceClause(c, d)) { return false; }
        if(Parameters::failFast && WatchList::isConflict(c.watch)) {
            Database::setFlag(c.pointer, Constants::VerificationBit, Constants::ScheduledFlag);
            stop = true;
        }
    }
    if(r.used - 1 >= c.position) {
        stop = true;
        Database::setFlag(c.pointer, Constants::VerificationBit, Constants::ScheduledFlag);
    }
    Blablabla::decrease();
    return true;
}

bool verifyInstruction(checker& c, proof& r, database& d, bool& verified) {
    Blablabla::log("Verifying instruction " + Blablabla::instructionToString(r, d, c.position) + ".");
    Blablabla::increase();
    getInstruction(c, r, d);
    if(Database::isFlag(c.pointer, Constants::VerificationBit, Constants::ScheduledFlag) {
        if(c.kind == Constants::InstructionIntroduction) {
            if(!deleteClause(c, d)) { return false; }
            if(!checkRup(c, d, c.pointer, verified)) { return false; }
            if(!verified) {
                if(!checkRat(c, d, c.pointer, c.pivot, verified)) { return false; }
            }
            if(verified) {
                Blablabla::log("Introduction instruction verified.");
            } else {
                Blablabla::log("Instruction verification failed.");
            }
        } else {
            if(!introduceClause(c, d)) { return false; }
            Blablabla::log("Nothing to check in deletion instructions.");
        }
    } else {
        Blablabla::log("Instruction not scheduled for verification.");
    }
    Blablabla::decrease();
    return true;
}

}
