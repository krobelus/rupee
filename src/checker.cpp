#include <stdlib.h>

#include "structs.hpp"
#include "extra.hpp"
#include "database.hpp"
#include "proof.hpp"
#include "watchlist.hpp"
#include "model.hpp"
#include "revision.hpp"
#include "latency.hpp"
#include "chain.hpp"
#include "checker.hpp"

namespace Checker {

int* ptr;
int literal;
long* candidate;

bool allocate(checker& c) {
    if(!WatchList::allocate(c.watch)) { return false; }
    if(!Model::allocate(c.stack)) { return false; }
    if(!Revision::allocate(c.cone)) { return false; }
    if(!Latency::allocate(c.rat)) { return false; }
    if(!Chain::allocate(c.tvr)) { return false; }
    return true;
}

void deallocate(checker& c) {
    WatchList::deallocate(c.watch);
    Model::deallocate(c.stack);
    Revision::deallocate(c.cone);
    Latency::deallocate(c.rat);
    Chain::deallocate(c.tvr);
}

bool endOfPremises(checker &c, proof &r) {
    return !(c.position < r.noPremises);
}

bool endOfProof(checker &c, proof &r) {
    return !(c. position < r.used);
}

bool startOfPremises(checker &c, proof &r) {
    return c.position <= 0;
}

bool startOfProof(checker &c, proof &r) {
    return c.position <= r.noPremises;
}

void next(checker &c, proof &r, database& d, int& signal) {
    ++c.position;
    getInstruction(c, r, d);
    if(c.kind == Constants::InstructionDeletion) {
        if(!deleteClause(c, d)) { return false; }
    } else {
        if(!introduceClause(c, d, Constants::DirectionForward)) { return false; }
    }
}

void getInstruction(checker& c, proof& r, database& d) {
	c.offset = r.array[c.position];
	c.pointer = Database::getPointer(d, c.offset);
	c.pivot = r.pivots[c.position];
	c.kind = r.kinds[c.position];
}

bool introduceClause(checker& c, database& d, bool direction) {
    Database::setClauseActive(d, c.pointer);
    Blablabla::log("Introducing clause " + Blablabla::clauseToString(c.pointer) + ".");

    if(direction == Constants::DirectionForward) {

    } else {

    }

}


bool initialize(checker& c, proof& r, database& d) {
    Blablabla::log("Preprocessing premise clauses.");
    Blablabla::increase();
    Model::assumeLiteral(c.stack, Constants::ReservedLiteral);
    if(!Model::hardPropagate(c.stack, c.watch, d)) { return false; }
    for(c.position = 0; c.position < r.noPremises; ++c.position) {
        getPremise(c, r, d);
        Blablabla::log("Introducing clause " + Blablabla::clauseToString(c.pointer));
        Blablabla::increase();
        if(!introduceClause(c, d)) { return false; }
        Blablabla::decrease();
    }
    Blablabla::log("End of premises.");
    Blablabla::decrease();
    return true;
}

bool preprocessProof(checker& c, proof& r, database& d, bool& conflict) {
    Blablabla::log("Preprocessing proof.");
    Blablabla::increase();
    conflict = false;
    for(c.position = r.noPremises; c.position < r.used; ++c.position) {
        getInstruction(c, r, d);
        if(!preprocessInstruction(c, r, d, conflict)) { return false; }
        if(conflict) {
            Blablabla::decrease();
            Blablabla::log("Conflict detected. Scheduled for verification.");
            return true;
        }
    }
    Blablabla::decrease();
    Blablabla::log("No conflict was detected.");
    return true;
}

bool verifyProof(checker& c, proof& r, database& d, bool& verified) {
    Blablabla::log("Verifying proof.");
    Blablabla::increase();
    verified = true;
    while(verified && c.position >= r.noPremises) {
        getInstruction(c, r, d);
        if(!verifyInstruction(c, r, d, verified)) { return false; }
        --c.position;
    }
    Blablabla::decrease();
    if(!verified) {
        Blablabla::log("Proof verification failed.");
    } else  {
        Blablabla::log("Proof verification succeeded.");
    }
    return true;
}

bool introduceClause(checker &c, database& d) {
    Database::setClauseActive(d, c.pointer);
    Blablabla::log("Introducing clause " + Blablabla::clauseToString(c.pointer) + ".");
    Blablabla::increase();
    if(Database::isEmpty(c.pointer)) {
        if(!WatchList::addConflict(c.watch, c.pointer, c.offset)) { return false; }
    } else {
        if(!WatchList::setWatches(c.watch, c.stack, c.pointer, c.offset)) { return false; }
        if(!Model::hardPropagate(c.stack, c.watch, d)) { return false; }
    }
    Blablabla::decrease();
    return true;
}

bool deleteClause(checker &c, database& d) {
    Database::setClauseInactive(d, c.pointer);
    Blablabla::log("Deleting clause " + Blablabla::clauseToString(c.pointer) + ".");
    Blablabla::increase();
    WatchList::findAndRemoveWatch(c.watch, c.pointer[0], c.offset);
    WatchList::findAndRemoveWatch(c.watch, c.pointer[1], c.offset);
    if(Database::isFlag(c.pointer, Constants::ConflictBit, Constants::ConflictFlag)) {
        Database::setFlag(c.pointer, Constants::ConflictBit, Constants::SatisfiableFlag);
        WatchList::findAndRemoveWatch(c.watch, Constants::ConflictWatchlist, c.offset);
    }
    if(Database::isFlag(c.pointer, Constants::PseudounitBit, Constants::ReasonFlag)) {
        Blablabla::log("Clause was a reason clause.");
        Revision::getCone(c.cone, c.stack, d, c.pointer);
        if(!Revision::reviseList(c.cone, c.watch, c.stack, d)) { return false; }
    } else {
        Blablabla::log("Clause was not a reason clause.");
    }
    Blablabla::decrease();
    return true;
}

bool checkRup(checker &c, database& d, int* clause, bool& rup) {
    Blablabla::log("Checking RUP for clause " + Blablabla::clauseToString(clause) + ".");
    Blablabla::increase();
    if((rup = WatchList::isConflict(c.watch))) {
        Blablabla::log("Clause follows from current model by ex falso quodlibet.");
        Model::assignSoftConflict(c.stack, c.watch.array[Constants::ConflictWatchlist][0]);
    } else {
        for(ptr = clause; (literal = *ptr) != Constants::EndOfClause; ++ptr) {
            if(Model::isSatisfied(c.stack, literal)) {
                Blablabla::log("Clause subsumed by current model.");
                Model::assignSoftConflict(c.stack, c.stack.reasons[literal]);
                rup = true;
            } else if(!Model::isFalsified(c.stack, literal)) {
                Model::assumeLiteral(c.stack, -literal);
            }
        }
        if(!rup) {
            if(!Model::softPropagate(c.stack, c.watch, d, rup)) { return false; }
        }
    }
    if(rup) {
        Blablabla::log("RUP check succeeded.");
        Chain::getChain(c.tvr, c.stack, d);
        Blablabla::logChain(c.tvr, d, c.stack);
        Chain::markChain(c.tvr, c.stack, d);
    } else {
        Blablabla::log("RUP check failed.");
    }
    Model::reset(c.stack, d);
    Blablabla::decrease();
    return true;
}

bool checkRat(checker &c, database& d, int* clause, int pivot, bool& rat) {
    rat = true;
    Blablabla::log("Checking RAT for clause " + Blablabla::clauseToString(clause) + " with pivot " + Blablabla::litToString(pivot) + ".");
    Blablabla::increase();
    Latency::loadClause(c.rat, clause, pivot);
    Latency::findResolvableClauses(c.rat, c.watch, d);
    Blablabla::logResolutionCandidates(c.rat, d);
    candidate = c.rat.array;
    while(rat && *candidate != Constants::EndOfWatchList) {
        Blablabla::log("Resolution candidate " + Blablabla::clauseToString(Database::getPointer(d, *candidate)) + ".");
        Blablabla::increase();
        if(Latency::computeResolvent(c.rat, Database::getPointer(d, *candidate))) {
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
    Blablabla::log("Preprocessing instruction    " + Blablabla::instructionToString(r, d, c.position));
    Blablabla::increase();
    if(c.kind == Constants::InstructionDeletion) {
        if(!deleteClause(c, d)) { return false; }
    } else {
        if(!introduceClause(c, d)) { return false; }
        if(Parameters::failFast && WatchList::isConflict(c.watch)) {
            Database::setFlag(c.pointer, Constants::VerificationBit, Constants::ScheduledFlag);
            stop = true;
        }
    }
    // if(r.used - 1 >= c.position) {
    //     stop = true;
    //     Database::setFlag(c.pointer, Constants::VerificationBit, Constants::ScheduledFlag);
    // }
    Blablabla::decrease();
    return true;
}

bool verifyInstruction(checker& c, proof& r, database& d, bool& verified) {
    Blablabla::log("Verifying instruction    " + Blablabla::instructionToString(r, d, c.position));
    Blablabla::increase();
    getInstruction(c, r, d);
    if(c.kind == Constants::InstructionIntroduction) {
        if(!deleteClause(c, d)) { return false; }
        if(Database::isFlag(c.pointer, Constants::VerificationBit, Constants::ScheduledFlag)) {
            if(!checkRup(c, d, c.pointer, verified)) { return false; }
            if(!verified) {
                if(!checkRat(c, d, c.pointer, c.pivot, verified)) { return false; }
            }
            if(verified) {
                Blablabla::log("Introduction instruction verified.");
            } else {
                Blablabla::log("Instruction verification failed.");
            }
        }
    } else {
        if(!introduceClause(c, d)) { return false; }
        Blablabla::log("Nothing to check in deletion instructions.");
    }
    Blablabla::decrease();
    return true;
}

}
