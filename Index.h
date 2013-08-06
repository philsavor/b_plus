/*
 * Index.h
 *
 *  Created on: Aug 2, 2013
 *      Author: xiangyu
 */

#include "Util.h"

class Index {
public:
	Index();
	virtual ~Index();
	void loadIndexFile(char* indexFileName);
	void findRecord(char* key);
	void insertRecord(char* record);
	//list sequential records;
	void listSeqRecord(char* startKey,int count);
	void findSeqInTextFile(ifstream& indexfile,long long offset,char* key,int count,vector<long long>& offsets);
	void getRightOffsets(ifstream& indexfile,long long rightNOS,int count,vector<long long>& offsets);
	long long findOffsetInTextFile(ifstream& indexfile,long long offset,char* key);
	long long findNextOffset(ifstream& indexfile,long long offset,char* key);
private:
	char* indexFileName;
	char* textFileName;
	int   depth;
};

