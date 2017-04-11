#include "BinarySearchTree.h"
#include <random>

#define num 6

void main()
{
	BinarySearchTree bst;
	for (int i = num; i > 0; i--)
	{
		int irand = rand();
		bst.Add(irand);
		cout << irand << endl;
	}

	if (bst.Search(6))
		cout << "Search Success!" << endl;
	else
		cout << "Search False!" << endl;

 	bst.allPrint();

	cout << endl;

	bst.Add(5);
	bst.Add(1);

	bst.allPrint();
	cout << endl;


	if (bst.Search(1))
		cout << "Search Success!" << endl;
	else
		cout << "Search False!" << endl;
	system("pause");
}