#include "stdafx.h"
#include "BinarySearchTree.h"

inline Node::~Node() 
{
	std::cout << "Delete : " << GetValue() << std::endl;
}

void Node::Insert(int InValue)
{
	
	if (InValue <= value)// 자기의 값보다 작거나 같으면 왼쪽으로 이동하거나 추가한다.
	{
		if (Node* LeftNode = GetLeft()) // 이미 왼쪽이 있으면 왼쪽에 넣어주는 시도를 한다.
		{
			LeftNode->Insert(InValue);
		}
		else // 왼쪽에 없으면 새로 노드를 만들어주고 추가해준다.
		{			
			LeftNode = new Node(InValue);
			SetLeft(LeftNode);
		}
	}
	else // 자기 값보다 크면 오른쪽으로 이동하거나 추가한다.
	{
		if (Node* RightNode = GetRight()) // 오른쪽 노드가 있으면 오른쪽 노드로 이동한다.
		{
			RightNode->Insert(InValue);
		}
		else // 없으면 노드를 만들어주고 추가해준다.
		{
			RightNode = new Node(InValue);
			SetRight(RightNode);
		}
	}
}

void Node::Print()
{
	if (Node* LeftNode = GetLeft()) // 왼쪽 노드가 있으면 왼쪽으로 이동한다.
	{
		LeftNode->Print();
	}
	
	std::cout << value << ","; // 왼쪽 처리가 끝나면 자기를 처리해준다.

	if (Node* RightNode = GetRight()) // 오른쪽 노드가 있다면 이제 오른쪽으로 이동한다.
	{
		RightNode->Print();
	}
}

void Node::Destroy()
{
	if (Node* LeftNode = GetLeft()) // 왼쪽 노드가 있다면 들어가서 지워준다.
	{
		LeftNode->Destroy();
		SetLeft(nullptr);
	}
	if (Node* RightNode = GetRight()) // 오른쪽 노드가 있으면 들어가서 지워준다.
	{
		RightNode->Destroy();
		SetRight(nullptr);
	}

	delete this; // 양쪽다 지웠으니 나도 지워도 된다.
}

void BinarySearchTree::Insert(int Value)
{
	if (pRoot == nullptr) // 루트가 없다면 루트부터 새로 만든다.
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

