#include "stdafx.h"
#include "BinarySearchTree.h"

inline Node::~Node() 
{
	std::cout << "Delete : " << GetValue() << std::endl;
}

void Node::Insert(int InValue)
{
	if (InValue <= value)
	{
		if (Node* LeftNode = GetLeft())
		{
			LeftNode->Insert(InValue);
		}
		else
		{
			LeftNode = new Node(InValue);
			SetLeft(LeftNode);
		}
	}
	else
	{
		if (Node* RightNode = GetRight())
		{
			RightNode->Insert(InValue);
		}
		else
		{
			RightNode = new Node(InValue);
			SetRight(RightNode);
		}
	}
}

void Node::Print()
{
	if (Node* LeftNode = GetLeft())
	{
		LeftNode->Print();
	}
	
	std::cout << value << ",";

	if (Node* RightNode = GetRight())
	{
		RightNode->Print();
	}
}

void Node::Destroy()
{
	if (Node* LeftNode = GetLeft())
	{
		LeftNode->Destroy();
		SetLeft(nullptr);
	}
	if (Node* RightNode = GetRight())
	{
		RightNode->Destroy();
		SetRight(nullptr);
	}

	delete this;
}

void BinarySearchTree::Insert(int Value)
{
	if (pRoot == nullptr)
	{
		pRoot = new Node(Value);
	}
	else
	{
		pRoot->Insert(Value);
	}
}

void BinarySearchTree::PrintAll()
{
	if (pRoot)
	{
		std::cout << "Print out :: " ;
		pRoot->Print();
	}
	else
	{
		std::cout << "Empty Tree" << std::endl;
	}
	std::cout << std::endl;
}

void BinarySearchTree::Destroy()
{
	if (pRoot)
	{
		pRoot->Destroy();
	}
	else
	{
		std::cout << "Empty Tree" << std::endl;
	}
}

