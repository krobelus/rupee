#ifndef __EXTRA_H_INCLUDED__
#define __EXTRA_H_INCLUDED__

#include <string>

namespace Constants {
	const int ExtraCellsDatabase = 3;
	const int FlagsCellDatabase = -2;
	const int PivotCellDatabase = -1;
	const int WatchOneCellDatabase = 0;
	const int WatchTwoCellDatabase = 1;
	const int ActivityFlagDatabase = 0;
	const int VerificationFlagDatabase = 1;
	const int OriginalityFlagDatabase = 2;
	const int PersistencyFlagDatabase = 3;

	const int ExtraBitsProof = 1;
	const int DeletionFlagProof = ActivityFlagDatabase;

	const int PremiseContradiction = 1;
	const int ProofContradiction = 2;
	const int NoContradiction = 3;

	const int KindDeletion = 0 << ActivityFlagDatabase;
	const int KindIntroduction = 1 << ActivityFlagDatabase;
	const int KindOriginal = 0 << OriginalityFlagDatabase;
	const int KindDerived = 1 << OriginalityFlagDatabase;
	const int KindInactive = 0 << ActivityFlagDatabase;
	const int KindActive = 1 << ActivityFlagDatabase;
	const int KindVerify = 1 << VerificationFlagDatabase;
	const int KindSkip = 0 << VerificationFlagDatabase;
	const int KindPermanent = 1 << PersistencyFlagDatabase;
	const int KindTemporal = 0 << PersistencyFlagDatabase;

	const int FilePremise = KindOriginal;
	const int FileProof = KindDerived;

	const int ParsingPhasePreprocessPremises = 1;
	const int ParsingPhasePreprocessProof = 2;
	const int ParsingPhaseInitialize = 3;
	const int ParsingPhaseReadPremises = 4;
	const int ParsingPhaseReadProof = 5;
	const int ParsingDeallocate = 6;

	const int ArgumentsFilePremise = 1;
	const int ArgumentsFileProof = 2;
	const int ArgumentsRest = 3;
	const int ArgumentsHelp = 4;
	const int ArgumentsError = 5;
}

namespace Parameters {
	extern std::string pathPremise;
	extern std::string pathProof;
	extern int bufferSize;
	extern int databaseSize;
	extern bool verbosity;
	extern int hashDepth;

	void setPremise(std::string path);
	void setProof(std::string path);
	void setVerbosity(bool value);
}

namespace Blablabla {
	extern int level;

	void increase();
	void decrease();
	void comment();
	void log();
}


namespace Tools {
	extern int commentLevel;

	int compare (const void *a, const void *b);
	void sortClause(int* cla, int length);
	int leap(int x);

	void comment(std::string str);
	void increaseCommentLevel();
	void decreaseCommentLevel();
	std::string offsetToString(long offset);
	std::string pointerToString(int* ptr);
	std::string processingStackToString();
	void resolutionChain();
	void reprocessingStack();
	void watchList(int literal);
}

#endif
