#include "stdafx.h"
#include "BinarySearchTree.h"

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
		if (LeftNode->GetLeft() == nullptr && LeftNode->GetRight() == nullptr)
		{
			std::cout << "delete : " << LeftNode->GetValue() << std::endl;
			delete LeftNode;
			SetLeft(nullptr);
		}
		else
		{
			LeftNode->Destroy();
		}
	}
	if (Node* RightNode = GetRight())
	{
		if (RightNode->GetLeft() == nullptr && RightNode->GetRight() == nullptr)
		{
			std::cout << "delete : " << RightNode->GetValue() << std::endl;
			delete RightNode;
			SetRight(nullptr);
		}
		else
		{
			RightNode->Destroy();
		}
	}
	
	
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
		pRoot->Print();
	}
	else
	{
		std::cout << "Empty Tree" << std::endl;
	}
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
