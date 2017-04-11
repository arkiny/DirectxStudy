#include "BinarySearchTree.h"

BinarySearchTree::BinarySearchTree()
{
}


BinarySearchTree::~BinarySearchTree()
{
}

bool BinarySearchTree::isEmpty()
{
	if(count > 0)
		return false;

	return true;
}

bool BinarySearchTree::Add(int key)
{
	if (Search(key) == true)
	{
		return false;
	}
	else if(root == nullptr)
	{
		root = Insert(root, key);
	}
	else
	{
		Node* output = Insert(root, key);
	}
	return true;
}


Node * BinarySearchTree::Insert(Node* pos, int key)
{
	if (pos == nullptr)
	{
		pos = new Node(key);
		return pos;
	}
	else if (pos->key > key)
	{
		pos->left = Insert(pos->left, key);
		return pos;
	}
	else if (pos->key < key)
	{
		pos->right = Insert(pos->right, key);
		return pos;
	}
}

bool BinarySearchTree::Search(int key)
{
	Node* destination = Traversal(root, key);
	if (destination != nullptr)
		return true;

	return false;

}

Node* BinarySearchTree::Traversal(Node* pos, int key)
{
	if (pos == nullptr)
		return nullptr;
	else if (pos->key == key)
		return pos;
	else if (pos->key < key)
		return Traversal(pos->right, key);
	else if (pos->key > key)
		return Traversal(pos->left, key);

	return nullptr;
}

void BinarySearchTree::allPrint()
{
	print(root);
}

Node* BinarySearchTree::print(Node* pos)
{
	cout << pos->key << endl;
	if (pos == nullptr)
		return pos;
	else
	{
		if (pos->right != nullptr)
			print(pos->right);
		if (pos->left != nullptr)
			print(pos->left);
	}
}
