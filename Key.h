/*
 * key.h
 *
 *  Created on: Jul 31, 2013
 *      Author: xiangyu
 */

#include "Util.h"

class Key {
public:
	Key();
	virtual ~Key();
	void setKeyValue(const char* record);
	inline void setOffset(long long offset){_offset = offset; }
	inline long long getKeyValue(){return _longKeyValue;}
	inline char* getKeyValueChar(){return _keyValue;}
	inline long long getOffSet(){return _offset;}
	void printKeyInfo();
	void printKeyInfo(ofstream& outfile);
    int compare(Key* key);
private:
	char* _keyValue;
	long long _longKeyValue;
    long long _offset;
};

