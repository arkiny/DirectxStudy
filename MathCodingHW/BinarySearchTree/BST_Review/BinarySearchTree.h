#pragma once
//#include "BinarySearchTree.h"
//#include "Node.h"
#include <iostream>
using namespace std;

class Node
{ 
public:
	int key;
	Node* left;
	Node* right;

	Node(int _key) { key = _key; };
};

class BinarySearchTree
{
public:
	BinarySearchTree();
	~BinarySearchTree();

	bool isEmpty();

	bool Add(int key);
	//Delete();
	class Node* Insert(Node* pos, int key);
	bool Search(int key);
	Node* Traversal(Node* pos, int key);

	void allPrint();
	Node* print(Node* pos);


private:
	int count;
	Node* root;
};

