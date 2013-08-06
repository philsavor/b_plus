/*
 * Node.h
 *
 *  Created on: Jul 31, 2013
 *      Author: xiangyu
 */

#ifndef NODE_H_
#define NODE_H_

#include "Util.h"
#include "Key.h"

class Node {
public:
	Node();
	Node(vector<Key*> ks,vector<Node*>ns,bool isL);
	virtual ~Node();
	void createIndexFile(char* inputFile, char* outputFile,bool ifCreateFile);
    void splitNode(Node* lNode,Node* rNode,Key* mKey);
    void splitRootNode(Node* lNode,Node* rNode,Key* mKey);
    void siblingPointer(Node* rNode);
    int getDepth(int level);
    int markNodesInALevel(int numMarkedNodes, int level);
    void outputNodesInALevel(int level,ofstream& outfile);
    void outputNode(ofstream& outfile);
	inline void setLeaf(bool is_or_not){isLeaf = is_or_not;}
	inline void setRoot(bool is_or_not){isRoot = is_or_not;}
	inline void setFather(Node* f){father = f;}
	inline void setLNode(Node* l){lNode = l;}
	inline void setRNode(Node* r){rNode = r;}
	inline Node* getRNode(){return rNode;}
	inline Node* getLNode(){return lNode;}
    inline void setChildrenFater(){
    	for(int i=0;i<(int)nodes.size();i++){
    		nodes[i]->setFather(this);
    	}
    }
	inline bool getIsLeaf(){return isLeaf;}
	inline bool getIsRoot(){return isRoot;}
	inline int  getNodeNum(){return nodeNum;}
	inline bool ifHaveRoom(){
		if(keys.size()< MAX_KEYS) return true;
		else                      return false;
	}
	void printKeys(int level);
//	void printLeave();
	inline Node* getFirstNode(){return nodes[0];}
    int insertKey(Key* newKey);
    void createIndexFromTree(char* inputFile,char* indexFileName);
private:
	vector<Key*> keys;
    vector<Node*> nodes;
    Node* father;
    Node* lNode;
    Node* rNode;
    bool isLeaf;
    bool isRoot;
    int nodeNum;

    int insertKeyDirectly(Key *newKey);
    int insertKeyToChild(Key *newKey);
};

#endif /* NODE_H_ */
