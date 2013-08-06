/*
 * Created on : July 31, 2013
 *     Author : Xiangyu Shen
 */

#include "Util.h"
#include "Node.h"
#include "Index.h"

int main(int argc, char* argv[]){
	//command: -create ,-find,-insert,-list
	if(argc<2){
		cerr<<"Usage: "<<argv[0]<<" ...\n";
		exit(-1);
	}
	string cmd(argv[1]);
	//create
	if(!cmd.compare("-create")){
        Node* rootNode = new Node();
        rootNode->setRoot(true);
        rootNode->createIndexFile(argv[2],argv[3],true);
	}
	else if(!cmd.compare("-find")){
		Index* fileIndex= new Index();
        fileIndex->loadIndexFile(argv[2]);
        fileIndex->findRecord(argv[3]);
	}
	else if(!cmd.compare("-insert")){
		Index* fileIndex= new Index();
		fileIndex->loadIndexFile(argv[2]);
		fileIndex->insertRecord(argv[3]);
    }
	else if(!cmd.compare("-list")){
		Index* fileIndex= new Index();
		fileIndex->loadIndexFile(argv[2]);
		fileIndex->listSeqRecord(argv[3],atoi(argv[4]));
	}

	return 0;
}
