#ifndef __PARSER_H_INCLUDED__
#define __PARSER_H_INCLUDED__

#include "database.hpp"
#include "proof.hpp"
#include "hashtable.hpp"
#include "lexer.hpp"

//************************************************************
// Class ClauseDictionaryEntry
//************************************************************

class ClauseDictionaryEntry {
//------------------------------------------------------------
// Data members
private:
	Long m_clause;
	std::uint32_t m_instruction;

//------------------------------------------------------------
// Builders
public:
	ClauseDictionaryEntry(Long clause, std::uint32_t instruction);

//------------------------------------------------------------
// Interface methods
public:
	Long clause();
	std::uint32_t instruction();
};

//************************************************************
// Class ClauseDictionary
//************************************************************

class ClauseDictionary {
//------------------------------------------------------------
// Data members
private:
	ClauseDictionaryEntry* m_array;
	std::uint32_t m_length;
	std::uint32_t m_capacity;

//------------------------------------------------------------
// Builders
public:
	ClauseDictionary(std::uint32_t capacity);
	~ClauseDictionary();

//------------------------------------------------------------
// Interface methods
public:
	void set(std::uint32_t id, Long clause, std::uint32_t instruction);
	Long getClause(std::uint32_t id);
	std::uint32_t getInstruction(std::uint32_t id);

//------------------------------------------------------------
// Internal methods
private:
	void reallocate(std::uint32_t id);
	bool fits(std::uint32_t id);

//------------------------------------------------------------
// Constant definitions
public:
	static constexpr Long NoRun = 0UL;
	static constexpr std::uint32_t NoInstruction = 0U;
	static constexpr std::uint32_t ReallocationFactor = 2;
};


//************************************************************
// Class CnfParser
//************************************************************

class CnfParser {
//------------------------------------------------------------
// Type definitions
public:
	using State = std::int32_t;

//------------------------------------------------------------
// Data members
private:
	Lexer m_lexer;
	ClauseDatabase& m_database;
	ClauseDictionary* m_dictionary;
	LratProof* m_lratproof;
	DratProof* m_dratproof;
	HashTable* m_hashtable;
	std::uint32_t m_counter;
	State m_state;

//------------------------------------------------------------
// Builders
public:
	CnfParser(const std::string& path, ClauseDatabase& database);
	~CnfParser();

//------------------------------------------------------------
// Interface methods
public:
	void setDratProof(DratProof& dratproof, HashTable& hashtable);
	void setLratProof(LratProof& lratproof, ClauseDictionary& dict);
	std::uint32_t parse();

//------------------------------------------------------------
// Constant definitions
public:
	static constexpr State Global = 0;
	static constexpr State InClause = 1;
	static constexpr State Done = 2;
};

//************************************************************
// Class DratParser
//************************************************************

class DratParser {
private:
	Lexer m_lexer;
	ClauseDatabase& m_clausedb;
	DratProof& m_proof;
	HashTable& m_hashtable;
	std::uint32_t m_counter;
	Short m_pivot;
	std::uint32_t m_identification;
	Long m_sickresolvent;
	std::string m_reading;
	int m_state;

//------------------------------------------------------------
// Builders
public:
	DratParser(const std::string& path, ClauseDatabase& clausedb,
				DratProof& proof, HashTable& hashtable, std::uint32_t counter);
	~DratParser();

//------------------------------------------------------------
// Interface methods
public:
	void parse();
	void setIdentification(std::uint32_t counter, Long clause);
	Long getIdentification();
	// void parseUntil(std::uint32_t limit);
	// void getSick(Short& pivot, Long& clause);

//------------------------------------------------------------
// Internal methods
private:
	void parseGlobal();
	void parseIntroduction();
	void parseDeletion();
	Short parseLiteral(std::string& str);
	std::int32_t parseInteger(std::string& str);
	void identify();


//------------------------------------------------------------
// Constant definitions
public:
	static const std::string DeletionId;
	static constexpr int Global = 0;
	static constexpr int Introduction = 1;
	static constexpr int Deletion = 2;
	static constexpr int Done = 3;
	static constexpr Short ZeroLiteral = Shortie::PositiveZero;
};


//************************************************************
// Class LratParser
//************************************************************

class LratParser {
//------------------------------------------------------------
// Type definitions
public:
	using State = std::int32_t;

//------------------------------------------------------------
// Data members
private:
	Lexer m_lexer;
	ClauseDatabase& m_clausedb;
	ChainDatabase& m_chaindb;
	ChainDatabase& m_deletiondb;
	ResolventDatabase& m_resolventdb;
	ClauseDictionary& m_dictionary;
	LratProof& m_lratproof;
	State m_state;
	std::string m_reading;
	std::uint32_t m_identifier;
	Long m_clause;
	Long m_prefix;
	Long m_resolvent;
	std::uint32_t m_instruction;
	bool m_rat;

//------------------------------------------------------------
// Builders
public:
	LratParser(const std::string& path, ClauseDatabase& clausedb, ChainDatabase& chaindb,
				ResolventDatabase& resolventdb, ChainDatabase& deletiondb,
				ClauseDictionary& dictionary, LratProof& lratproof);
	~LratParser();

//------------------------------------------------------------
// Interface methods
public:
	void parse();

//------------------------------------------------------------
// Internal methods
	void parseIdentifier();
	void parseKind();
	void parseClause();
	void parseChain();
	void parseResolvents();
	void parseDeletion();
	std::uint32_t parseNumber(std::string& str);
	std::int32_t parseInteger(std::string& str);
	Short parseLiteral(std::string& str);

	void print();

//------------------------------------------------------------
// Constant definitions
public:
	static const std::string DeletionId;
	static constexpr Short ZeroLiteral = Shortie::PositiveZero;
	static constexpr State Identifier = 0;
	static constexpr State Kind = 1;
	static constexpr State InClause = 2;
	static constexpr State InChain = 3;
	static constexpr State InResolvents = 4;
	static constexpr State InDeletion = 5;
	static constexpr State Done = 6;
};

//************************************************************
// Class SickParser
//************************************************************

class SickParser {
//------------------------------------------------------------
// Data members
private:
	Lexer m_lexer;
	ClauseDatabase& m_database;
	SickInstruction& m_sick;
	int m_state;
	std::string m_reading;

//------------------------------------------------------------
// Builders
public:
	SickParser(const std::string& path, ClauseDatabase& database, SickInstruction& sick);
	~SickParser();

//------------------------------------------------------------
// Interface methods
public:
	void parse();

//------------------------------------------------------------
// Internal methods
public:
	void parseVerification();
	void parsePivot();
	void parseInstruction();
	void parseNatural();
	void parseNaturalClause();
	void parseNaturalModel();
	void parseResolvent();
	void parseResolventClause();
	void parseResolventModel();
	std::uint32_t parseNumber(std::string& str);
	std::int32_t parseInteger(std::string& str);
	Short parseLiteral(std::string& str);

//------------------------------------------------------------
// Constant definitions
public:
	static const std::string VerificationId;
	static const std::string NaturalId;
	static const std::string ResolventId;
	static constexpr int Verification = 0;
	static constexpr int Pivot = 1;
	static constexpr int Instruction = 2;
	static constexpr int Natural = 3;
	static constexpr int NaturalClause = 4;
	static constexpr int NaturalModel = 5;
	static constexpr int Resolvent = 6;
	static constexpr int ResolventClause = 7;
	static constexpr int ResolventModel = 8;
	static constexpr int Done = 9;
	static constexpr Short ZeroLiteral = Shortie::PositiveZero;
};


#endif