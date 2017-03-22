#ifndef __EXTRA_H_INCLUDED__
#define __EXTRA_H_INCLUDED__

#include <string>

#include "structs.hpp"

namespace Constants {
	const int ExtraCellsDatabase = 1;
	const int FlagsCellDatabase = -1;
	const int ActivityBit = 0;
	const int VerificationBit = 1;
	const int OriginalityBit = 2;
	const int PersistencyBit = 3;
    const int PseudounitBit = 4;
	const int ConflictBit = 5;
	const int RawActivity = 6;
    const bool ActiveFlag = true;
    const bool InactiveFlag = false;
    const bool OriginalFlag = true;
    const bool DerivedFlag = false;
    const bool ScheduledFlag = true;
    const bool SkipFlag = false;
    const bool PersistentFlag = true;
    const bool TemporalFlag = false;
    const bool ReasonFlag = true;
    const bool RedundantFlag = false;
	const bool ConflictFlag = true;
	const bool SatisfiableFlag = true;

    const bool FilePremise = OriginalFlag;
    const bool FileProof = DerivedFlag;
    const bool InstructionDeletion = InactiveFlag;
    const bool InstructionIntroduction = ActiveFlag;

	const int ArgumentsFilePremise = 1;
	const int ArgumentsFileProof = 2;
	const int ArgumentsRest = 3;
	const int ArgumentsHelp = 4;
	const int ArgumentsError = 5;

	const int ReservedLiteral = 1;
	const int ConflictWatchlist = 0;
	const int AssumedLiteral = 0;
	const int EndOfClause = 0;
	const int EndOfWatchList = 0;
	const int NoTrigger = 0;
	const bool SoftPropagation = true;
	const bool HardPropagation = false;
}

namespace Parameters {
	extern std::string pathPremise;
	extern std::string pathProof;
	extern int bufferSize;
	extern int databaseSize;
	extern bool verbosity;
	extern int hashDepth;
	extern int noVariables;
	extern bool failFast;

	void setPremise(std::string path);
	void setProof(std::string path);
	void setVerbosity(bool value);
	void setBufferSize(int value);
	void setDatabaseSize(int value);
	void setHashDepth(int value);
	void setNoVariables(int value);
	void importNoVariables(parser& p);
}

namespace Blablabla {
	extern int level;

	void increase();
	void decrease();
	void comment(std::string str);
	void log(std::string str);
	void logModel(model& m);
	void logWatchList(watchlist& wl, int literal, database& d);
	void logResolutionCandidates(latency& x, database& d);
	void logReasons(model& m, database& d);
	void logRevision(revision& v);
	void logDatabase(database& d);
	void logProof(proof& r, database& d);
	std::string clauseToString(int* ptr);
	std::string instructionToString(int* pos);
}

//
// namespace Tools {
// 	extern int commentLevel;
//
// 	int compare (const void *a, const void *b);
// 	void sortClause(int* cla, int length);
// 	int leap(int x);
//
// 	void comment(std::string str);
// 	void increaseCommentLevel();
// 	void decreaseCommentLevel();
// 	std::string offsetToString(long offset);
// 	std::string pointerToString(int* ptr);
// 	std::string processingStackToString();
// 	void resolutionChain();
// 	void reprocessingStack();
// 	void watchList(int literal);
// }

#endif
