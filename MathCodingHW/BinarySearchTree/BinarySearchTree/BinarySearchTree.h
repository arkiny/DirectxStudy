#pragma once

class Node
{
public:
	Node(int InValue) : value(InValue) {}
	~Node();

	Node* GetLeft() { return pLeft; }
	Node* GetRight() { return pRight; }
	void Insert(int InValue);
	int GetValue() { return value; }

	void SetLeft(Node* InNode) { pLeft = InNode; }
	void SetRight(Node* InNode) { pRight = InNode; }
	void Print();
	void Destroy();

private:
	Node* pLeft = nullptr;
	Node* pRight = nullptr;
	int value = 0;
};

class BinarySearchTree
{
public:
	void Insert(int Value);
	void PrintAll();
	void Destroy();

private:
	Node* pRoot = nullptr;
};