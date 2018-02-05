#include <iostream>

#include "core.hpp"
#include "database.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "sickchecker.hpp"


//************************************************************
// Class SickChecker
//************************************************************
// Builders

SickChecker::SickChecker(ClauseDatabase& clausedb, DratProof& proof,
				SickInstruction& sick) :
	m_clausedb(clausedb),
	m_proof(proof),
	m_sick(sick),
	m_array(10000U)
{}

//------------------------------------------------------------
// Interface methods

void SickChecker::instructionCheck() {
	DratInference inference = m_proof.getInstruction(m_sick.instruction());
	if(!inference.isIntroduction()) {
		Core::reject("SICK instruction does not correspond to an introduction instruction in DRAT file");
		return;
	}
	if(inference.getPivot() != m_sick.pivot()) {
		Core::reject("SICK pivot does not correspond to the pivot at the SICK instruction in DRAT file");
		return;
	}
	ClauseIterator it1(m_clausedb.getClause(m_sick.naturalClause()));
	ClauseIterator it2(m_clausedb.getClause(inference.getClause()));
	if(!it1.equals(it2)) {
		Core::reject("SICK natural clause does not correspond to the clause at the SICK instruction in DRAT file");
		return;
	}
	if(m_sick.pivot() != ZeroLiteral && m_sick.resolventClause() == 0UL) {
		Core::reject("SICK resolution clause does not occur in the accumulated formula at the SICK instruction");
		return;
	}
}

void SickChecker::naturalCheck() {
	ClauseIterator clause(m_clausedb.getClause(m_sick.naturalClause()));
	ClauseIterator model(m_clausedb.getClause(m_sick.naturalModel()));
	m_array.insertClause(model);
	if(m_array.tautology()) {
		Core::reject("SICK natural model is inconsistent");
		return;
	}
	if(!m_array.containsReversed(clause)) {
		Core::reject("SICK natural model does not falsify the natural clause.");
		return;
	}
	if(!upModelCheck()) {
		Core::reject("SICK natural model is not an UP-model of the accumulated formula at the SICK instruction");
		return;
	}
	m_array.reset();
}

void SickChecker::resolventCheck() {
	if(m_sick.pivot() != ZeroLiteral) {
		if(m_sick.resolventClause() == 0UL) {
			Core::reject("SICK resolution clause does not occur in the accumulated formula at the SICK instruction");
			return;
		}

		ClauseIterator natclause(m_clausedb.getClause(m_sick.naturalClause()));
		ClauseIterator resclause(m_clausedb.getClause(m_sick.resolventClause()));
		ClauseIterator model(m_clausedb.getClause(m_sick.resolventModel()));
		Short pivot = m_sick.pivot();
		if(!m_clausedb.active(m_sick.resolventClause())) {
			Core::reject("SICK resolution clause does not occur in the accumulated formula at the SICK instruction");
			return;
		}
		if(!natclause.contains(pivot) || !resclause.contains(Shortie::complement(pivot))) {
			Core::reject("SICK natural and resolution clauses are not resolvable upon SICK pivot");
			return;
		}
		natclause.rewind();
		resclause.rewind();
		m_array.insertResolvent(natclause, resclause, pivot);
		if(m_array.tautology()) {
			Core::reject("Resolvent of SICK natural and resolution clauses is a tautology");
		}
		natclause.rewind();
		resclause.rewind();
		m_array.reset();
		m_array.insertClause(model);
		if(m_array.tautology()) {
			Core::reject("SICK resolvent model is inconsistent");
			return;
		}
		if(!m_array.containsReversedResolvent(natclause, resclause, pivot)) {
			Core::reject("SICK resolvent model does not falsify the resolvent of SICK natural and resolvent clauses.");
			return;
		}
		if(!upModelCheck()) {
			Core::reject("SICK resolvent model is not an UP-model of the accumulated formula at the SICK instruction");
			return;
		}
		m_array.reset();
	}
}


//------------------------------------------------------------
// Internal methods

bool SickChecker::upModelCheck() {
	FormulaIterator it(m_clausedb.getIterator());
	while(!it.end()) {
		ClauseIterator clause(m_clausedb.getClause(it.get()));
		if(!m_array.upModels(clause)) {
			return false;
		}
		it.next();
	}
	return true;
}


//************************************************************
// Main procedure
//************************************************************

int main(int argc, char* argv[]) {
	if(argc <= 3) {
		Core::fail("c Incorrect syntax; run as \"lratcheck [CNF file] [DRAT file] [SICK file]\"");
	}
	ClauseDatabase clausedb(100000UL, 10000U);
	DratProof proof(10000U);
	SickInstruction sick;
	SickChecker checker(clausedb, proof, sick);
	do {
		HashTable hashtable(5U);
		std::uint32_t counter;
		do {
			SickParser sickps(argv[3], clausedb, sick);
			sickps.parse();
		} while(false);
 		do {
			CnfParser cnfps(argv[1], clausedb);
			cnfps.setDratProof(proof, hashtable);
			counter = cnfps.parse();
		} while(false);
		do {
			DratParser dratps(argv[2], clausedb, proof, hashtable, counter);
			if(sick.pivot() != Shortie::PositiveZero) {
				dratps.setIdentification(sick.instruction(), sick.resolventClause());
			}
			dratps.parse();
			if(sick.pivot() != Shortie::PositiveZero) {
				sick.resolventClause() = dratps.getIdentification();
			}
		} while(false);
	} while(false);
	proof.accumulateFormula(sick.instruction(), clausedb);
	if(Core::undecided()) {
		checker.instructionCheck();
	}
	if(Core::undecided()) {
		checker.naturalCheck();
	}
	if(Core::undecided()) {
		checker.resolventCheck();
	}
	if(Core::undecided()) {
		Core::accept("SICK incorrectness certificate verified");
	}
	Core::print();
	return 0;
}