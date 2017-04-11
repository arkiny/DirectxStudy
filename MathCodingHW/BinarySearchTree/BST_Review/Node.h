#pragma once

class Node
{
public:
	Node(int key);
	~Node();

	int getKey() { return key; };
	Node* getLeft() { return pLeft;  };
	Node* getRight() { return pRight; };
	void setLeft(Node* _pLeft) { pLeft = _pLeft; };
	void setRight(Node* _pRight) { pRight = _pRight; };

private:
	int key;
	Node* pLeft;
	Node* pRight;
};

