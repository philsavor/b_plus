/*
 * Index.cpp
 *
 *  Created on: Aug 2, 2013
 *      Author: xiangyu
 */

#include "Index.h"
#include "Node.h"

Index::Index() {
	indexFileName = NULL;
	textFileName = NULL;
	depth = 0;
}

Index::~Index() {
	// TODO Auto-generated destructor stub
}

void Index::loadIndexFile(char* indexFN){
	this->indexFileName = indexFN;
	ifstream infile(this->indexFileName, ios::in | ios::binary);
    if(!infile.good()){
	   cerr<<"Error opening index file\n";
	   exit(-1);
    }
    char* buffer = new char[BLOCK_SIZE];
    //first block
    infile.seekg(infile.beg);
    infile.read(buffer,BLOCK_SIZE);
//    for(int i=0;i<BLOCK_SIZE;i++){
//    	cout<<i<<" : "<<buffer[i]<<endl;
//    }
    char* textFN = new char[FILE_NAME_LENGTH];
    for(int i=0;i<FILE_NAME_LENGTH;i++){
        if(buffer[i]==' '){
        	textFN[i] = '\0';break;
        }
        textFN[i]=buffer[i];
    }
    this->textFileName = textFN;

    //only root node->depth is 0; two level-> depth is 1 ...
    int dep;
    infile.seekg(256,infile.beg);
    infile.read((char*)(&dep), sizeof(int));
    this->depth = dep;

    infile.close();
}

void Index::findRecord(char* key){
	ifstream infile(this->indexFileName, ios::in | ios::binary);
	if(!infile.good()){
		cerr<<"Error opening index file\n";
	    exit(-1);
	}
	//for internal nodes
	long long offset=BLOCK_SIZE;  //root node
	for(int i=0;i<this->depth;i++){
		offset = findNextOffset(infile,offset,key);
	}
    //for leaf node
	offset = findOffsetInTextFile(infile,offset,key);
    if(offset == -1){
 	    cout<<"Nothing; not found."<<endl;
 	    infile.close();
 	    return;
    }

	infile.close();

    ifstream textFile(this->textFileName);
    if(!textFile.good()){
    	cerr<<"Error opening text file\n";
    	exit(-1);
    }
    textFile.seekg(offset,textFile.beg);
    string record;
    getline(textFile,record);
    cout<<"At "<<offset<<",record:"<<record<<endl;
    textFile.close();
}

long long Index::findOffsetInTextFile(ifstream& indexfile,long long offset, char* key){
	//find the same key in index file
	indexfile.seekg(offset,indexfile.beg);
    vector<Key*> keys = vector<Key*>(0);
    vector<long long>pointers = vector<long long>(0);
    for(int i=0;i<MAX_KEYS;i++){
        //pointer
    	long long p;
    	indexfile.read((char*)(&p), POINTER_SIZE);
    	pointers.insert(pointers.end(),p);
    	//key
        char k[KEY_SIZE];
        indexfile.read(k,KEY_SIZE);
        if(k[0]==' ') break;
        Key* ke = new Key();
        ke->setKeyValue(k);
        keys.insert(keys.end(),ke);
    }
//    //sibling pointer
//    long long sPointer;
//    indexfile.seekg(BLOCK_SIZE-POINTER_SIZE,indexfile.beg);
//    indexfile.read((char*)(&sPointer), POINTER_SIZE);

    Key* pKey = new Key();
    pKey->setKeyValue(key);
    long long offsetInTextFile;
    for(int i=0;i<=(int)keys.size();i++){
   	    if(i==(int)keys.size()){
   		   return -1;
   	    }
        if(pKey->compare(keys[i]) == 0){
//       	   keys[i]->printKeyInfo();
       	   offsetInTextFile = pointers[i];break;
        }
    }
    return offsetInTextFile;
}

long long Index::findNextOffset(ifstream& indexfile,long long offset,char* key){
	indexfile.seekg(offset,indexfile.beg);
    vector<Key*> keys = vector<Key*>(0);
    vector<long long>pointers = vector<long long>(0);
    for(int i=0;i<=MAX_KEYS;i++){
        //pointer
    	long long p;
    	indexfile.read((char*)(&p), POINTER_SIZE);
    	pointers.insert(pointers.end(),p);
    	//key
        char k[KEY_SIZE];
        indexfile.read(k,KEY_SIZE);
        if(k[0]==' ') break;
        Key* ke = new Key();
        ke->setKeyValue(k);
        keys.insert(keys.end(),ke);
    }

//    for(int i=0;i<keys.size();i++){
//    	keys[i]->printKeyInfo();
//    }
//    cout<<keys.size()<<endl;
//    for(int i=0;i<pointers.size();i++){
//        cout<<pointers[i]<<endl;
//    }
//    cout<<pointers.size()<<endl;

     Key* pKey = new Key();
     pKey->setKeyValue(key);
     long long nextOffset;
     for(int i=0;i<=(int)keys.size();i++){
    	 if(i==(int)keys.size()){
//    		 keys[i-1]->printKeyInfo();
    		 nextOffset = pointers[i];break;
    	 }
         if(pKey->compare(keys[i]) <= 0){
//        	 keys[i]->printKeyInfo();
        	 nextOffset = pointers[i];break;
         }
     }
     return nextOffset;
}

void Index::insertRecord(char* record){
	Node* rootNode = new Node();
	rootNode->setRoot(true);
	rootNode->createIndexFile(this->textFileName,NULL,false);
    //get new key
	Key* k = new Key();
	//value
	k->setKeyValue(record);
	//offset
	ifstream infile(this->textFileName);
	if(!infile.good()){
	    cerr<<"Error opening input file\n";
	    exit(-1);
	}
	infile.seekg(0,infile.end);
	long long length = infile.tellg();
//	cout<<"length:"<<length<<endl;
	k->setOffset(length);
	infile.close();
	//insert key
	if(rootNode->insertKey(k) == -1){
		cout<<"Fail to insert!"<<endl;
	}
	else{
		//add new record into text file
		ofstream outfile(this->textFileName,std::ios_base::app | std::ios_base::out);
	    if(!outfile.good()){
			 cerr<<"Error opening text file\n";
			 exit(-1);
	    }
	    outfile<<record<<"\r\n";
	    outfile.close();
		//create new index file
		rootNode->createIndexFromTree(this->textFileName,this->indexFileName);

		cout<<"Succeed to insert!"<<endl;
	}
}

void Index::findSeqInTextFile(ifstream& indexfile,
		                            long long offset,
		                            char* key,
		                            int count,
		                            vector<long long>& offsets){
	//find the same key in index file
	indexfile.seekg(offset,indexfile.beg);
    vector<Key*> keys = vector<Key*>(0);
    vector<long long>pointers = vector<long long>(0);
    for(int i=0;i<MAX_KEYS;i++){
        //pointer
    	long long p;
    	indexfile.read((char*)(&p), POINTER_SIZE);
    	pointers.insert(pointers.end(),p);
    	//key
        char k[KEY_SIZE];
        indexfile.read(k,KEY_SIZE);
        if(k[0]==' ') break;
        Key* ke = new Key();
        ke->setKeyValue(k);
//        cout<<"key_node: ";
//        ke->printKeyInfo();
        keys.insert(keys.end(),ke);
    }
    //sibling pointer
    long long sPointer;
    indexfile.seekg(offset+BLOCK_SIZE-POINTER_SIZE,indexfile.beg);
    indexfile.read((char*)(&sPointer), POINTER_SIZE);

    Key* pKey = new Key();
    pKey->setKeyValue(key);
    for(int i=0;i<=(int)keys.size();i++){
   	    if(i==(int)keys.size()){
   	    	getRightOffsets(indexfile,sPointer,count,offsets);
   	    }
        if(pKey->compare(keys[i]) <= 0){
           //save all offset
       	   for(int j=i;j<(int)keys.size();j++){
       		   offsets.insert(offsets.end(),pointers[j]);
//       		   cout<<"insert2:"<<pointers[j]<<endl;
       		   count--;
       		   if(count==0) break;
       	   }
       	   if(count>0)
       		   getRightOffsets(indexfile,sPointer,count,offsets);

       	   break;
        }
    }
}

void Index::getRightOffsets(ifstream& indexfile,
		                    long long rightNOS,
		                    int count,
		                    vector<long long>& offsets){
	//find the same key in index file
	indexfile.seekg(rightNOS,indexfile.beg);
    vector<Key*> keys = vector<Key*>(0);
    vector<long long>pointers = vector<long long>(0);
    for(int i=0;i<MAX_KEYS;i++){
        //pointer
    	long long p;
    	indexfile.read((char*)(&p), POINTER_SIZE);
    	pointers.insert(pointers.end(),p);
    	//key
        char k[KEY_SIZE];
        indexfile.read(k,KEY_SIZE);
        if(k[0]==' ') break;
        Key* ke = new Key();
        ke->setKeyValue(k);
        keys.insert(keys.end(),ke);
    }
    //TODO if sibling pointer is null,the most right node
    //sibling pointer
    long long sPointer;
    indexfile.seekg(rightNOS+BLOCK_SIZE-POINTER_SIZE,indexfile.beg);
    indexfile.read((char*)(&sPointer), POINTER_SIZE);

    for(int i=0;i<(int)keys.size();i++){
       offsets.insert(offsets.end(),pointers[i]);
//       cout<<"insert1:"<<pointers[i]<<endl;
       count--;
       if(count==0) break;
    }
    if(count>0)
    	getRightOffsets(indexfile,sPointer,count,offsets);

}

//list sequential records;
void Index::listSeqRecord(char* startKey,int count){
	ifstream infile(this->indexFileName, ios::in | ios::binary);
	if(!infile.good()){
		cerr<<"Error opening index file\n";
	    exit(-1);
	}
	//for internal nodes
	long long offset=BLOCK_SIZE;  //root node
	for(int i=0;i<this->depth;i++){
		offset = findNextOffset(infile,offset,startKey);
	}
    //for leaf node
	vector<long long> offsets= vector<long long>(0);
	findSeqInTextFile(infile,offset,startKey,count,offsets);

	infile.close();

    ifstream textFile(this->textFileName);
    if(!textFile.good()){
    	cerr<<"Error opening text file\n";
    	exit(-1);
    }
    for(int i=0;i<count;i++){
        textFile.seekg(offsets[i],textFile.beg);
        string record;
        getline(textFile,record);
        if(record=="")break;
        cout<<"Record:"<<record<<endl;
    }
    textFile.close();

}
