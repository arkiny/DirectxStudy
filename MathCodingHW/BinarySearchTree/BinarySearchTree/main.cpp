// BinarySearchTree.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BinarySearchTree.h"

int main()
{
	BinarySearchTree Tree;
	Tree.Insert(5);
	Tree.Insert(7);
	Tree.Insert(1);
	Tree.Insert(3);
	Tree.PrintAll();

	Tree.Destroy();
	system("pause");
}

