#include <iostream>

#include "core.hpp"
#include "database.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "lratchecker.hpp"


//************************************************************
// Class LratChecker
//************************************************************
// Builders

LratChecker::LratChecker(ClauseDatabase& clausedb, ChainDatabase& chaindb,
				ResolventDatabase& resolventdb, ChainDatabase& deletiondb,
				LratProof& proof) :
	m_clausedb(clausedb),
	m_chaindb(chaindb),
	m_resolventdb(resolventdb),
	m_deletiondb(deletiondb),
	m_proof(proof),
	m_array(10000U)
{}

//------------------------------------------------------------
// Interface methods

void LratChecker::check() {
	m_empty = false;
	while(!m_proof.end() && Core::undecided()) {
		LratInference inference(m_proof.get());
		if(inference.isRup()) {
			checkRupInference(inference.getClause(), inference.getChain());
		} else if(inference.isRat()) {
			std::cout << "[RAT check performed]" << std::endl;
			checkRatInference(inference.getClause(), inference.getChain(), inference.getResolvents());
		}
		m_clausedb.setActivity(inference.getClause(), true);
		processDeletions(inference.getDeletion());
		m_empty |= m_clausedb.empty(inference.getClause());
		m_proof.next();
	}
	if(Core::undecided()) {
		if(m_empty) {
			Core::accept("LRAT proof verified");
		} else {
			Core::reject("Empty clause was not derived");
		}
	}
}

//------------------------------------------------------------
// Internal methods

void LratChecker::propagateChain(ChainIterator& it) {
	while(!it.end()) {
		Long propcl = it.get();
		if(m_clausedb.active(propcl)) {
			ClauseIterator propit(m_clausedb.getClause(propcl));
			m_array.propagateClause(propit);
		}
		it.next();
	}
}

void LratChecker::checkRupInference(Long clause, Long chain) {
	ClauseIterator clit(m_clausedb.getClause(clause));
	ChainIterator chit(m_chaindb.getChain(chain));
	m_array.reverseClause(clit);
	propagateChain(chit);
	if(!m_array.tautology()) {
		Core::reject("Hints do not lead to contradiction by unit propagation in RUP inference");
	}
	m_array.reset();
}

void LratChecker::checkRatInference(Long clause, Long chain, Long resolvents) {
	Short pivot = m_clausedb.pivot(clause);
	if(pivot == ShortDatabase::Separator) {
		Core::reject("Checking empty clause as a RAT inference");
		return;
	}
	ClauseIterator clit1(m_clausedb.getClause(clause));
	ChainIterator chit(m_chaindb.getChain(chain));
	ResolventIterator rsit(m_resolventdb.getResolvent(resolvents));
	FormulaIterator cnfit(m_clausedb.getIterator(Shortie::complement(pivot)));
	while(!cnfit.end()) {
		Long resolvable = cnfit.get();
		ClauseIterator clit2(m_clausedb.getClause(resolvable));
		m_array.reverseResolvent(clit1, clit2, pivot);
		if(!m_array.tautology()) {
			if(!rsit.find(resolvable)) {
				Core::reject("Could not find hint sequence for resolvable clause in RAT inference");
				return;
			}
			ChainIterator chrsit(m_chaindb.getChain(rsit.get()));
			propagateChain(chit);
			propagateChain(chrsit);
			if(!m_array.tautology()) {
				Core::reject("Hints do not lead to contradiction by unit propagation in RUP check within a RAT inference");
				return;
			}
		}
		clit1.rewind();
		m_array.reset();
		cnfit.nextLiteral();
	}
}

void LratChecker::processDeletions(Long chain) {
	ChainIterator it(m_deletiondb.getChain(chain));
	while(!it.end()) {
		m_clausedb.setActivity(it.get(), false);
		it.next();
	}
}

//************************************************************
// Main procedure
//************************************************************

int main(int argc, char* argv[]) {
	if(argc <= 2) {
		Core::fail("c Incorrect syntax; run as \"lratcheck [CNF file] [LRAT file]\"");
	}
	ClauseDatabase clausedb(100000UL, 10000U);
	ChainDatabase chaindb(100000UL);
	ChainDatabase deletiondb(100000UL);
	ResolventDatabase resolventdb(1000UL);
	LratProof proof(10000U);
	do {
		ClauseDictionary dictionary(10000U);
		do {
			CnfParser cnfps(argv[1], clausedb);
			cnfps.setLratProof(proof, dictionary);
			cnfps.parse();
		} while(false);
		do {
			LratParser lratps(argv[2], clausedb, chaindb, resolventdb, deletiondb, dictionary, proof);
			lratps.parse();
		} while(false);
	} while(false);
	do {
		LratChecker check(clausedb, chaindb, resolventdb, deletiondb, proof);
		check.check();
	} while(false);
	Core::print();
	return 0;
}