#ifndef __EXTRA_H_INCLUDED__
#define __EXTRA_H_INCLUDED__

#include <string>

#include "parser.hpp"

namespace Constants {
	const int ExtraCellsDatabase = 1;
	const int FlagsCellDatabase = -1;
	const int ActivityBit = 0;
	const int VerificationBit = 1;
	const int OriginalityBit = 2;
	const int PersistencyBit = 3;
    const int PseudounitBit = 4;
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

    const bool FilePremise = OriginalFlag;
    const bool FileProof = DerivedFlag;
    const bool InstructionDeletion = InactiveFlag;
    const bool InstructionIntroduction = ActiveFlag;

	const int ArgumentsFilePremise = 1;
	const int ArgumentsFileProof = 2;
	const int ArgumentsRest = 3;
	const int ArgumentsHelp = 4;
	const int ArgumentsError = 5;

	const int ClauseSizeConflict = 0;
	const int ClauseSizeUnit = 1;
	const int ClauseSizeLong = 2;
}

namespace Parameters {
	extern std::string pathPremise;
	extern std::string pathProof;
	extern int bufferSize;
	extern int databaseSize;
	extern bool verbosity;
	extern int hashDepth;
	extern int noVariables;

	void setPremise(std::string path);
	void setProof(std::string path);
	void setVerbosity(bool value);
	void setBufferSize(int value);
	void setDatabaseSize(int value);
	void setHashDepth(int value);
	void importNoVariables(parser& p);
}

namespace Blablabla {
	extern int level;

	void increase();
	void decrease();
	void comment(std::string str);
	void log(std::string str);
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
