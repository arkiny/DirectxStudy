#include "stdafx.h"
#include "BinarySearchTree.h"

inline Node::~Node() 
{
	std::cout << "Delete : " << GetValue() << std::endl;
}

void Node::Insert(int InValue)
{
	
	if (InValue <= value)// �ڱ��� ������ �۰ų� ������ �������� �̵��ϰų� �߰��Ѵ�.
	{
		if (Node* LeftNode = GetLeft()) // �̹� ������ ������ ���ʿ� �־��ִ� �õ��� �Ѵ�.
		{
			LeftNode->Insert(InValue);
		}
		else // ���ʿ� ������ ���� ��带 ������ְ� �߰����ش�.
		{			
			LeftNode = new Node(InValue);
			SetLeft(LeftNode);
		}
	}
	else // �ڱ� ������ ũ�� ���������� �̵��ϰų� �߰��Ѵ�.
	{
		if (Node* RightNode = GetRight()) // ������ ��尡 ������ ������ ���� �̵��Ѵ�.
		{
			RightNode->Insert(InValue);
		}
		else // ������ ��带 ������ְ� �߰����ش�.
		{
			RightNode = new Node(InValue);
			SetRight(RightNode);
		}
	}
}

void Node::Print()
{
	if (Node* LeftNode = GetLeft()) // ���� ��尡 ������ �������� �̵��Ѵ�.
	{
		LeftNode->Print();
	}
	
	std::cout << value << ","; // ���� ó���� ������ �ڱ⸦ ó�����ش�.

	if (Node* RightNode = GetRight()) // ������ ��尡 �ִٸ� ���� ���������� �̵��Ѵ�.
	{
		RightNode->Print();
	}
}

void Node::Destroy()
{
	if (Node* LeftNode = GetLeft()) // ���� ��尡 �ִٸ� ���� �����ش�.
	{
		LeftNode->Destroy();
		SetLeft(nullptr);
	}
	if (Node* RightNode = GetRight()) // ������ ��尡 ������ ���� �����ش�.
	{
		RightNode->Destroy();
		SetRight(nullptr);
	}

	delete this; // ���ʴ� �������� ���� ������ �ȴ�.
}

void BinarySearchTree::Insert(int Value)
{
	if (pRoot == nullptr) // ��Ʈ�� ���ٸ� ��Ʈ���� ���� �����.
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

