#include <exception>
#include <iostream>

#include "core.hpp"
#include "database.hpp"
#include "lexer.hpp"
#include "parser.hpp"

//************************************************************
// Class ClauseDictionaryEntry
//************************************************************
// Builders

ClauseDictionaryEntry::ClauseDictionaryEntry(Long clause, std::uint32_t instruction) :
	m_clause(clause),
	m_instruction(instruction)
{}

//------------------------------------------------------------
// Interface methods

Long ClauseDictionaryEntry::clause() {
	return m_clause;
}

std::uint32_t ClauseDictionaryEntry::instruction() {
	return m_instruction;
}


//************************************************************
// Class ClauseDictionary
//************************************************************
// Builders

ClauseDictionary::ClauseDictionary(std::uint32_t capacity) :
	m_array(nullptr),
	m_length(0U),
	m_capacity(capacity)
{
	m_array = static_cast<ClauseDictionaryEntry*>(std::malloc(m_capacity * sizeof(ClauseDictionaryEntry)));
	if(m_array == nullptr) {
		Core::fail("Allocation error");
	}
	for(std::uint32_t i = 0U; i < m_capacity; ++i) {
		m_array[i] = ClauseDictionaryEntry(NoRun, NoInstruction);
	}
}

ClauseDictionary::~ClauseDictionary() {
	if(m_array != nullptr) {
		std::free(m_array);
	}
}

//------------------------------------------------------------
// Interface methods

void ClauseDictionary::set(std::uint32_t id, Long clause, std::uint32_t instruction) {
	if(!fits(id)) {
		reallocate(id);
	}
	if(id > m_length) {
		m_length = id;
	}
	m_array[id] = ClauseDictionaryEntry(clause, instruction);
}

Long ClauseDictionary::getClause(std::uint32_t id) {
	Long x = m_array[id].clause();
	if(x == NoRun) {
		Core::fail("Undefined clause referenced");
	}
	return x;
}

std::uint32_t ClauseDictionary::getInstruction(std::uint32_t id) {
	std::uint32_t x = m_array[id].instruction();
	if(x == NoInstruction) {
		Core::invalid("Undefined clause referenced");
	}
	return x;
}

//------------------------------------------------------------
// Internal methods

void ClauseDictionary::reallocate(std::uint32_t id) {
	std::uint32_t oldcapacity = m_capacity;
	m_capacity = id * ReallocationFactor;
	m_array = static_cast<ClauseDictionaryEntry*>(std::realloc(m_array, m_capacity * sizeof(ClauseDictionaryEntry)));
	if(m_array == nullptr) {
		Core::fail("Reallocation error");
	}
	for(std::uint32_t i = oldcapacity; i < m_capacity; ++i) {
		m_array[i] = ClauseDictionaryEntry(NoRun, NoInstruction);
	}
}

bool ClauseDictionary::fits(std::uint32_t id) {
	return id < m_capacity;
}


//************************************************************
// Class CnfParser
//************************************************************
// Builders

CnfParser::CnfParser(const std::string& path, ClauseDatabase& database) :
	m_lexer(),
	m_database(database),
	m_dictionary(nullptr),
	m_lratproof(nullptr),
	m_dratproof(nullptr),
	m_hashtable(nullptr),
	m_counter(0U),
	m_state(Global)
{
	m_lexer.open(path);
}

CnfParser::~CnfParser() {
	m_lexer.close();
}

//------------------------------------------------------------
// Interface methods

void CnfParser::setDratProof(DratProof& dratproof, HashTable& hashtable) {
	m_dratproof = &dratproof;
	m_hashtable = &hashtable;
}

void CnfParser::setLratProof(LratProof& lratproof, ClauseDictionary& dict) {
	m_lratproof = &lratproof;
	m_dictionary = &dict;
}

std::uint32_t CnfParser::parse() {
	Short literal;
	Long run;
	Long instruction;
	while(!m_lexer.end() && Core::undecided()) {
		try {
			literal = Shortie::toShort(std::stoi(m_lexer.read()));
		} catch(std::invalid_argument& e) {
			Core::invalid("Unable to parse CNF file");
			return 0U;
		} catch(std::out_of_range& e) {
			Core::fail("Parsed integer outside of <int> range");
			return 0U;
		}
		if(literal != Shortie::PositiveZero) {
			if(m_state == Global) {
				m_state = InClause;
			}
			m_database.insert(literal);
		} else {
			m_state = Global;
			++m_counter;
			if(m_database.tautology()) {
				Core::invalid("CNF file contains a tautology");
			} else {
				if(m_dratproof != nullptr) {
					m_database.sort();
				}
				run = m_database.close();
				m_database.setActivity(run, false);
				if(m_lratproof != nullptr) {
					instruction = m_lratproof->insertPremise(run);
					m_dictionary->set(m_counter, run, instruction);
				}
				if(m_dratproof != nullptr) {
					m_dratproof->insertPremise(run);
					ClauseIterator it = m_database.getClause(run);
					m_hashtable->force(it);
				}
			}
		}
		m_lexer.next();
	}
	if(m_state != Global) {
		Core::invalid("Unable to parse CNF file");
	}
	m_state = Done;
	return m_counter;
}

//************************************************************
// Class DratParser
//************************************************************
// Builders

DratParser::DratParser(const std::string& path, ClauseDatabase& clausedb,
		DratProof& proof, HashTable& hashtable, std::uint32_t counter) :
	m_clausedb(clausedb),
	m_proof(proof),
	m_hashtable(hashtable),
	m_counter(counter),
	m_identification(0U),
	m_sickresolvent(0UL),
	m_state(Global)
{
	m_lexer.open(path);
}

DratParser::~DratParser() {
	m_lexer.close();
}

//------------------------------------------------------------
// Interface methods

void DratParser::parse() {
	while(!m_lexer.end() && Core::undecided()) {
		m_reading = m_lexer.read();
		if(m_state == Introduction) {
			parseIntroduction();
		} else if(m_state == Deletion) {
			parseDeletion();
		} else if(m_state == Global) {
			parseGlobal();
		}
		m_lexer.next();
	}
	if(Core::undecided() && (m_state != Global)) {
		Core::invalid("Unable to parse DRAT file");
	}
	m_state = Done;
}

void DratParser::setIdentification(std::uint32_t counter, Long clause) {
	m_identification = counter;
	m_sickresolvent = clause;
}

Long DratParser::getIdentification() {
	return m_sickresolvent;
}

//------------------------------------------------------------
// Internal methods

void DratParser::parseGlobal() {
	if(m_reading == DeletionId) {
		m_state = Deletion;
	} else {
		m_state = Introduction;
		m_pivot = parseLiteral(m_reading);
		parseIntroduction();
	}
}

void DratParser::parseIntroduction() {
	Short literal = parseLiteral(m_reading);
	if(literal == ZeroLiteral) {
		if(++m_counter == m_identification) {
			identify();
		}
		if(m_clausedb.tautology()) {
			Core::invalid("DRAT proof contains a tautology");
			return;
		}
		m_clausedb.sort();
		Long clause = m_clausedb.close();
		ClauseIterator it = m_clausedb.getClause(clause);
		m_hashtable.force(it);
		m_clausedb.setActivity(clause, false);
		m_proof.insertIntroduction(clause, m_pivot);
		m_state = Global;
	} else {
		m_clausedb.insert(literal);
	}
}

void DratParser::parseDeletion() {
	Short literal = parseLiteral(m_reading);
	if(literal == ZeroLiteral) {
		Long found;
		if(++m_counter == m_identification) {
			identify();
		}
		m_clausedb.sort();
		Long clause = m_clausedb.close();
		ClauseIterator it = m_clausedb.getClause(clause);
		if(m_hashtable.remove(it, found)) {
			m_proof.insertDeletion(found);
		} else {
			m_proof.insertDeletion(0UL);
		}
		m_clausedb.rewind(clause);
		m_state = Global;
	} else {
		m_clausedb.insert(literal);
	}
}

Short DratParser::parseLiteral(std::string& str) {
	return Shortie::toShort(parseInteger(str));
}

std::int32_t DratParser::parseInteger(std::string& str) {
	try {
		return std::stoi(str);
	} catch(std::invalid_argument& e) {
		Core::invalid("Unable to parse LRAT file");
	} catch(std::out_of_range& e) {
		Core::fail("Parsed integer outside of <int> range");
	}
	return 0;
}

void DratParser::identify() {
	ClauseIterator it(m_clausedb.getClause(m_sickresolvent));
	if(!m_hashtable.find(it, m_sickresolvent)) {
		m_sickresolvent = 0UL;
	}
}

const std::string DratParser::DeletionId = "d";


//************************************************************
// Class LratParser
//************************************************************
// Builders

LratParser::LratParser(const std::string& path, ClauseDatabase& clausedb,
			ChainDatabase& chaindb, ResolventDatabase& resolventdb,
			ChainDatabase& deletiondb, ClauseDictionary& dictionary,
			LratProof& lratproof) :
	m_lexer(),
	m_clausedb(clausedb),
	m_chaindb(chaindb),
	m_deletiondb(deletiondb),
	m_resolventdb(resolventdb),
	m_dictionary(dictionary),
	m_lratproof(lratproof),
	m_state(Identifier)
{
	m_lexer.open(path);
}

LratParser::~LratParser() {
	m_lexer.close();
}

//------------------------------------------------------------
// Interface methods

void LratParser::parse() {
	while(!m_lexer.end() && Core::undecided()) {
		m_reading = m_lexer.read();
		if(m_state <= InClause) {
			if(m_state == InClause) {
				parseClause();
			} else if(m_state == Identifier) {
				parseIdentifier();
			} else if(m_state == Kind) {
				parseKind();
			}
		} else {
			if(m_state == InDeletion) {
				parseDeletion();
			} else if(m_state == InChain) {
				parseChain();
			} else if(m_state == InResolvents) {
				parseResolvents();
			}
		}
		m_lexer.next();
	}
	if(Core::undecided() && m_state != Identifier) {
		Core::invalid("Unable to parse LRAT file");
	}
	m_state = Done;
}

//------------------------------------------------------------
// Internal methods

void LratParser::parseIdentifier() {
	if(m_reading == RatId) {
		m_lexer.next();
		m_reading = m_lexer.read();
	}
	m_identifier = parseNumber(m_reading);
	m_state = Kind;
}

void LratParser::parseKind() {
	if(m_reading == DeletionId) {
		m_instruction = m_dictionary.getInstruction(m_identifier);
		m_state = InDeletion;
	} else {
		m_rat = true;
		m_state = InClause;
		parseClause();
	}
}

void LratParser::parseClause() {
	Short literal = parseLiteral(m_reading);
	if(literal == ZeroLiteral) {
		if(m_clausedb.tautology()) {
			Core::invalid("LRAT file contains a tautology");
		}
		m_clause = m_clausedb.close();
		m_clausedb.setActivity(m_clause, false);
		m_state = InChain;
	} else {
		m_clausedb.insert(literal);
	}
}

void LratParser::parseChain() {
	std::int32_t number = parseInteger(m_reading);
	if(number > 0) {
		m_rat = false;
		m_chaindb.insert(m_dictionary.getClause(static_cast<Long>(number)));
	} else {
		m_prefix = m_chaindb.close();
		if(number < 0) {
			m_resolvent = m_dictionary.getClause(static_cast<Long>(-number));
			m_instruction = m_lratproof.insertRat(m_clause, m_prefix);
			m_state = InResolvents;
		} else {
			m_state = Identifier;
			if(m_rat) {
				m_instruction = m_lratproof.insertRat(m_clause, m_prefix);
				m_lratproof.addResolventChain(m_instruction, m_resolventdb.close());
			} else {
				m_instruction = m_lratproof.insertRup(m_clause, m_prefix);
			}
		}
		m_dictionary.set(m_identifier, m_clause, m_instruction);
	}
}

void LratParser::parseResolvents() {
	std::int32_t number = parseInteger(m_reading);
	if(number > 0) {
		m_chaindb.insert(m_dictionary.getClause(static_cast<Long>(number)));
	} else {
		m_prefix = m_chaindb.close();
		m_resolventdb.insert(m_resolvent, m_prefix);
		if(number == 0) {
			m_lratproof.addResolventChain(m_instruction, m_resolventdb.close());
			m_state = Identifier;
		} else {
			m_resolvent = m_dictionary.getClause(static_cast<Long>(-number));
		}
	}
}

void LratParser::parseDeletion() {
	std::uint32_t id = parseNumber(m_reading);
	if(id != 0L) {
		m_deletiondb.insert(m_dictionary.getClause(id));
	} else {
		m_lratproof.addDeletionChain(m_instruction, m_deletiondb.close());
		m_state = Identifier;
	}
}

std::uint32_t LratParser::parseNumber(std::string& str) {
	std::int32_t x = parseInteger(str);
	if(x < 0) {
		Core::invalid("Unable to parse LRAT file");
	}
	return static_cast<std::uint32_t>(x);
}

std::int32_t LratParser::parseInteger(std::string& str) {
	try {
		return std::stoi(str);
	} catch(std::invalid_argument& e) {
		Core::invalid("Unable to parse LRAT file");
	} catch(std::out_of_range& e) {
		Core::fail("Parsed integer outside of <int> range");
	}
	return 0;
}

Short LratParser::parseLiteral(std::string& str) {
	return Shortie::toShort(parseInteger(str));
}

const std::string LratParser::DeletionId = "d";
const std::string LratParser::RatId = "r";


//************************************************************
// Class SickParser
//************************************************************
// Builders

SickParser::SickParser(const std::string& path, ClauseDatabase& database, SickInstruction& sick) :
	m_database(database),
	m_sick(sick),
	m_state(Verification)
{
	m_lexer.open(path);
}

SickParser::~SickParser() {
	m_lexer.close();
}

//------------------------------------------------------------
// Interface methods

void SickParser::parse() {
	while(!m_lexer.end() && Core::undecided()) {
		m_reading = m_lexer.read();
		if(m_state <= NaturalClause) {
			if(m_state == NaturalClause) {
				parseNaturalClause();
			} else if(m_state == Verification) {
				parseVerification();
			} else if(m_state == Pivot) {
				parsePivot();
			} else if(m_state == Instruction) {
				parseInstruction();
			} else if(m_state == Natural) {
				parseNatural();
			}
		} else {
			if(m_state == NaturalModel) {
				parseNaturalModel();
			} else if(m_state == ResolventClause) {
				parseResolventClause();
			} else if(m_state == ResolventModel) {
				parseResolventModel();
			} else if(m_state == Resolvent) {
				parseResolvent();
			}
		}
		m_lexer.next();
	}
	if(Core::undecided() && m_state != Done) {
		Core::invalid("Unable to parse SICK file");
	}
}

//------------------------------------------------------------
// Internal methods

void SickParser::parseVerification() {
	if(m_reading == VerificationId) {
		m_state = Pivot;
	} else {
		Core::invalid("Unable to parse SICK file");
	}
}

void SickParser::parsePivot() {
	m_sick.pivot() = parseLiteral(m_reading);
	m_state = Instruction;
}

void SickParser::parseInstruction() {
	m_sick.instruction() = parseNumber(m_reading);
	m_state = Natural;
}

void SickParser::parseNatural() {
	if(m_reading == NaturalId) {
		m_state = NaturalClause;
	} else {
		Core::invalid("Unable to parse SICK file");
	}
}

void SickParser::parseNaturalClause() {
	Short literal = parseLiteral(m_reading);
	if(literal == ZeroLiteral) {
		m_database.sort();
		m_sick.naturalClause() = m_database.close();
		m_database.setActivity(m_sick.naturalClause(), false);
		m_state = NaturalModel;
	} else {
		m_database.insert(literal);
	}
}

void SickParser::parseNaturalModel() {
	Short literal = parseLiteral(m_reading);
	if(literal == ZeroLiteral) {
		m_database.sort();
		m_sick.naturalModel() = m_database.close();
		m_database.setActivity(m_sick.naturalModel(), false);
		if(m_sick.pivot() != ZeroLiteral) {
			m_state = Resolvent;
		} else {
			m_state = Done;
		}
	} else {
		m_database.insert(literal);
	}
}

void SickParser::parseResolvent() {
	if(m_reading == ResolventId) {
		m_state = ResolventClause;
	} else {
		Core::invalid("Unable to parse SICK file");
	}
}

void SickParser::parseResolventClause() {
	Short literal = parseLiteral(m_reading);
	if(literal == ZeroLiteral) {
		m_database.sort();
		m_sick.resolventClause() = m_database.close();
		m_database.setActivity(m_sick.resolventClause(), false);
		m_state = ResolventModel;
	} else {
		m_database.insert(literal);
	}
}

void SickParser::parseResolventModel() {
	Short literal = parseLiteral(m_reading);
	if(literal == ZeroLiteral) {
		m_sick.resolventModel() = m_database.close();
		m_database.setActivity(m_sick.resolventModel(), false);
		m_state = Done;
	} else {
		m_database.insert(literal);
	}
}

std::uint32_t SickParser::parseNumber(std::string& str) {
	std::int32_t x = parseInteger(str);
	if(x < 0) {
		Core::invalid("Unable to parse SICK file");
	}
	return static_cast<std::uint32_t>(x);
}

std::int32_t SickParser::parseInteger(std::string& str) {
	try {
		return std::stoi(str);
	} catch(std::invalid_argument& e) {
		Core::invalid("Unable to parse SICK file");
	} catch(std::out_of_range& e) {
		Core::fail("Parsed integer outside of <int> range");
	}
	return 0;
}

Short SickParser::parseLiteral(std::string& str) {
	return Shortie::toShort(parseInteger(str));
}

const std::string SickParser::VerificationId = "v";
const std::string SickParser::NaturalId = "n";
const std::string SickParser::ResolventId = "r";