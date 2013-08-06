/*
 * Key.cpp
 *
 *  Created on: Jul 31, 2013
 *      Author: xiangyu
 */

#include "Key.h"

Key::Key() {
	// TODO Auto-generated constructor stub
    _keyValue = new char[KEY_SIZE];
    _longKeyValue = 0;
    _offset = 0;
}

Key::~Key() {
	// TODO Auto-generated destructor stub
}

int Key::compare(Key* key){
	if( getKeyValue() < key->getKeyValue() )          return -1;
	else if( getKeyValue() == key->getKeyValue() )    return 0;
	else                                              return 1;
}


void Key::setKeyValue(const char* record){
	for(int i=0;i<KEY_SIZE;i++){
		_keyValue[i] = record[i];
	}
	for(int i=KEY_SIZE-2;i>=0;i--){
		_longKeyValue += (_keyValue[i]-'0')*pow(10,(KEY_SIZE-2-i));
	}
}

void Key::printKeyInfo(){
	for(int i=0;i<KEY_SIZE;i++){
		cout<<_keyValue[i];
	}
	cout<<endl;
//    cout<<"long long value: "<< _longKeyValue<<endl;
//	cout<<"offset: "<<_offset<<endl;;
}

void Key::printKeyInfo(ofstream& outfile){
	for(int i=0;i<KEY_SIZE;i++){
		outfile<<_keyValue[i];
	}
	outfile<<endl;
}
