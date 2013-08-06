/*
 * Node.cpp
 *
 *  Created on: Jul 31, 2013
 *      Author: xiangyu
 */

#include "Node.h"

//used by root node object
void Node::createIndexFile(char* inputFile, char* outputFile,bool ifCreateFile){
     //read record from input file
     //get key value and offset
	 //insert key into tree from root node
     ifstream infile(inputFile);
     if(!infile.good()){
     	 cerr<<"Error opening input file\n";
     	 exit(-1);
     }
     long long offset = 0;
     while(1){
    	 //get a new record
         string record;
         getline(infile,record);
         if(!infile.good()) break;
         //set key value
         const char* rcd = record.c_str();
         Key* tmpKey = new Key();
         tmpKey->setKeyValue(rcd);
         //set offset
         tmpKey->setOffset(offset);
         offset += record.length() + 1;

         insertKey(tmpKey);
     }
     infile.close();

//     this->printKeys(0);
     //tree->index file
     if(ifCreateFile==true){
         createIndexFromTree(inputFile,outputFile);
         cout<<"Succeed to create index file!"<<endl;
     }
}

//used by all nodes
//insert key into keys or pass it to child node
int Node::insertKey(Key* newKey){
	 bool ifHR = ifHaveRoom();
	 //internal node
	 if(getIsLeaf()==false){
	      int re = insertKeyToChild(newKey);
	      if(re==-1){
//	      	   cout<<"fail to insert!"<<endl;
	      	   return -1;
	      }
	 }
	 //insert directly
	 if(getIsLeaf()==true && ifHR==true ){
		 int re = insertKeyDirectly(newKey);
		 if(re==-1){
//			 cout<<"fail to insert!"<<endl;
			 return -1;
		 }
	 }
     //split leaf node
	 if(getIsLeaf()==true && ifHR==false){
    	 int re = insertKeyDirectly(newKey);
    	 if(re==-1){
//    		 cout<<"fail to insert!"<<endl;
    		 return -1;
    	 }
         //1.get middle key
         //mid: 4->1, 5->2  size->midIndex
    	 int midIndex;
    	 if(keys.size() % 2 == 0) midIndex = keys.size()/2 -1;
    	 else midIndex = keys.size()/2;
    	 Key* midKey = keys[midIndex];
    	 //2.create left node
    	 //keys
    	 vector<Key*> lks = vector<Key*>(0);
    	 for(int i=0;i<=midIndex;i++){
    	      lks.insert(lks.end(),keys[i]);
    	 }
    	 //nodes
    	 vector<Node*> lns = vector<Node*>(0);
    	 Node* lNode = new Node(lks,lns,true);
    	 //3.create right node
    	 //keys
    	 vector<Key*> rks = vector<Key*>(0);
    	 for(int i=midIndex+1;i<(int)keys.size();i++){
    	      rks.insert(rks.end(),keys[i]);
    	 }
    	 //nodes
    	 vector<Node*> rns = vector<Node*>(0);
    	 Node* rNode = new Node(rks,rns,true);

    	 lNode->siblingPointer(rNode);
    	 if(this->lNode != NULL){
  		     lNode->setLNode(this->lNode);
  	         this->lNode->setRNode(lNode);
    	 }
    	 if(this->rNode != NULL){
              rNode->setRNode(this->rNode);
  	          this->rNode->setLNode(rNode);
    	 }

         if(getIsRoot()==true){
             this->setLeaf(false);
             this->splitRootNode(lNode,rNode,midKey);
         }
         else{
             lNode->setFather(this->father);
             rNode->setFather(this->father);
             this->father->splitNode(lNode,rNode,midKey);

         }
     }
	 return 0;
}

//called by root node
void Node::splitRootNode(Node* lNode,Node* rNode,Key* mKey){
    keys.clear();
    nodes.clear();
    keys.insert(keys.end(),mKey);
    lNode->setFather(this);
    rNode->setFather(this);
    lNode->setChildrenFater();
    rNode->setChildrenFater();
    nodes.insert(nodes.end(),lNode);
    nodes.insert(nodes.end(),rNode);
}

void Node::splitNode(Node* lNode,Node* rNode,Key* mKey){
     if(ifHaveRoom()==true){
    	 // 2,3,4 -> 2,5,3,4  :index= 1
         int index = this->insertKeyDirectly(mKey);
         if(index == -1) {
//        	 cout<< "fail to insert!!!"<<endl;
        	 return;
         }
         nodes.insert(nodes.begin()+index,lNode);
         nodes[index+1] = rNode;
         //TODO release useless node
     }
     else{
    	 // 2,3,4 -> 2,5,3,4  :index= 1
         int index = this->insertKeyDirectly(mKey);
         if(index == -1) {
//        	 cout<< "fail to insert!!!"<<endl;
        	 return;
         }
         nodes.insert(nodes.begin()+index,lNode);
         nodes[index+1] = rNode;
         //TODO release useless node

         //1.get middle key
         //mid: 4->1, 5->2  size->midIndex
    	 int midIndex;
    	 if(keys.size() % 2 == 0) midIndex = keys.size()/2 -1;
    	 else midIndex = keys.size()/2;
    	 Key* midKey = keys[midIndex];

    	 //2.create left node
    	 //keys
    	 vector<Key*> lks = vector<Key*>(0);
    	 //no middle node
    	 for(int i=0;i<midIndex;i++){
    	      lks.insert(lks.end(),keys[i]);
    	 }
    	 //nodes
    	 vector<Node*> lns = vector<Node*>(0);
    	 for(int i=0;i<= midIndex;i++){
    		 lns.insert(lns.end(),nodes[i]);
    	 }
    	 Node* lNode = new Node(lks,lns,false);

    	 //3.create right node
    	 //keys
    	 vector<Key*> rks = vector<Key*>(0);
    	 for(int i=midIndex+1;i<(int)keys.size();i++){
    	      rks.insert(rks.end(),keys[i]);
    	 }
    	 //nodes
    	 vector<Node*> rns = vector<Node*>(0);
    	 for(int i=midIndex+1;i<=(int)keys.size();i++){
    		 rns.insert(rns.end(),nodes[i]);
    	 }
    	 Node* rNode = new Node(rks,rns,false);

         if(getIsRoot()==true){
             this->splitRootNode(lNode,rNode,midKey);
         }
         else{
        	 lNode->setChildrenFater();
        	 rNode->setChildrenFater();
             lNode->setFather(this->father);
             rNode->setFather(this->father);
             this->father->splitNode(lNode,rNode,midKey);
         }
     }
}

//point to right sibling node
void Node::siblingPointer(Node* rNode){
	//l->r
    this->rNode = rNode;
    rNode->setLNode(this);
}

int Node::insertKeyToChild(Key *newKey){
	int result;
	for(int i=0;i<=(int)keys.size();i++){
        //bigger than all the keys
        if(i==(int)keys.size()){
        	result = nodes[i]->insertKey(newKey);
			break;
		}
		//smaller
	    if(newKey->compare(keys[i])<=0){
	    	result = nodes[i]->insertKey(newKey);
	    	break;
		}
	}
	return result;
}

int Node::insertKeyDirectly(Key *newKey){
	for(int i=0;i<=(int)keys.size();i++){
		//bigger than all the keys
		if(i==(int)keys.size()){
			keys.insert(keys.end(),newKey);
			return i;
		}
		//smaller
	    if(newKey->compare(keys[i])<0){
	    	keys.insert(keys.begin()+i,newKey);
	    	return i;
	    }
	    if(newKey->compare(keys[i])== 0){
//	   	     cout<<"Exist!"<<endl;
	   	     return -1;
	   	}
	}
	return -1;
}

//used by root node object
void Node::createIndexFromTree(char* textFileName, char* indexFileName){
	 string indexFN = indexFileName;
     ofstream outfile(indexFN.c_str());

     //name  256B
     string textFNInIndex(256,' ');
     string textFN = textFileName;
     textFNInIndex.replace(0,textFN.size(),textFN);
     outfile<<textFNInIndex;
     //depth 4B
     int depth = this->getDepth(0);
     outfile.write((char*)&depth,sizeof(int));
     //others 1024B-260B
     string others(BLOCK_SIZE-260,' ');
     outfile <<others;

     /*
      * write b+ tree to index file
      */
     //mark all nodes
     int numMarkedNodes = 0;
     for(int level=0;level<=depth;level++){
    	 numMarkedNodes = this->markNodesInALevel(numMarkedNodes,level);
     }
     //output nodes to index file
     for(int level=0;level<=depth;level++){
         this->outputNodesInALevel(level,outfile);
     }

	 outfile.close();
}

void Node::outputNode(ofstream& outfile){
	/*
	 * one block: 1024 Byte
	 * pointer key pointer key ... pointer key pointer
	 * pointer: 8Byte
	 * key:     15 Byte
	 */

	if(this->getIsLeaf()==true){
		for(int i=0;i<(int)keys.size();i++){
			long long offsetInText = keys[i]->getOffSet();
			outfile.write((char*)&offsetInText,sizeof(offsetInText));    //8byte
            outfile<<keys[i]->getKeyValueChar();  //15byte
		}
		string blank((BLOCK_SIZE-(POINTER_SIZE+KEY_SIZE)*(int)keys.size())-POINTER_SIZE,' ');
		outfile<<blank;
        //sibling pointer
		if(this->rNode != NULL){
			int num = this->rNode->getNodeNum();
	//		cout<<"num:"<<num<<endl;
	//		this->printKeys(0);
			long long offsetInIndex = BLOCK_SIZE * (num + 1);
			outfile.write((char*)&offsetInIndex,sizeof(offsetInIndex));      //8byte
		}
		else{
			string blank(POINTER_SIZE,' ');
			outfile<<blank;
		}

	}
	else{
		for(int i=0;i<=(int)keys.size();i++){
			//childNum:offset  --->   0:1024   1:2048
			int childNum = nodes[i]->getNodeNum();
			long long offsetInIndex = BLOCK_SIZE * (childNum + 1);
			outfile.write((char*)&offsetInIndex,sizeof(offsetInIndex));      //8byte
			if(i<(int)keys.size()){
				outfile<<keys[i]->getKeyValueChar();  //15byte
            }

		}
		string blank((BLOCK_SIZE-(POINTER_SIZE+KEY_SIZE)*(int)keys.size()-POINTER_SIZE),' ');
		outfile<<blank;
	}

}

//return the number of marked nodes
void Node::outputNodesInALevel(int level,ofstream& outfile){
     if(level==0){
    	 this->outputNode(outfile);
     }
     else{
    	 for(int i=0;i<(int)nodes.size();i++){
    		 nodes[i]->outputNodesInALevel(level-1,outfile);
    	 }
     }
}

//return the number of marked nodes
int Node::markNodesInALevel(int numMarkedNodes, int level){
     if(level==0){
    	 this->nodeNum = numMarkedNodes;
    	 return numMarkedNodes+1;
     }
     else{
    	 for(int i=0;i<(int)nodes.size();i++){
    		 numMarkedNodes = nodes[i]->markNodesInALevel(numMarkedNodes,level-1);
    	 }
    	 return numMarkedNodes;
     }
}

//only root node->depth is 0; two level-> depth is 1 ...
int Node::getDepth(int level){
	if(this->isLeaf == true) return level;
	else{
		int depth = this->nodes[0]->getDepth(level+1);
		return depth;
	}
}

void Node::printKeys(int level){
	for(int i=0;i<(int)keys.size();i++){
		if(isLeaf == false)
		    nodes[i]->printKeys(level+1);

		for(int j=0;j<level;j++)
		      cout<<"  ";
		keys[i]->printKeyInfo();
//		//test
//		if(this->rNode!=NULL){
//            cout<<"                           ";
//		    this->rNode->keys[0]->printKeyInfo();
//		}
//		//test
	}
	if(isLeaf == false)
		nodes[keys.size()]->printKeys(level+1);
	if(isLeaf == true) cout<<endl;
}

//void Node::printLeave(){
//	Node* n = this;
//	if(n->getIsLeaf() == true){
//		while(1){
//			if(n==NULL) break;
//			for(int i=0;i<(int)n->keys.size();i++){
//			    keys[i]->printKeyInfo();
//		    }
//            n = n->getRNode();
//		}
//	}
//	else{
//		n = n->getFirstNode();
//		n->printLeave();
//	}
//}


Node::Node(vector<Key*> ks,vector<Node*>ns,bool isL){
	keys = ks;
	nodes = ns;
	isLeaf = isL;
	father = NULL;
	lNode = NULL;
	rNode = NULL;
	isRoot = false;
	nodeNum = 0;
}

Node::Node() {
    isLeaf = true;
    isRoot = false;
    father = NULL;
	lNode = NULL;
	rNode = NULL;
    keys = vector<Key*>(0);
    nodes = vector<Node*>(0);
    nodeNum = 0;
}

Node::~Node() {
	// TODO Auto-generated destructor stub
}
