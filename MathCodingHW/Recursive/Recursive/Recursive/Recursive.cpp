#include <iostream>
#include <string>

// Recursive, ����Լ���.
// ������ �Լ� = �Լ��� ������ ������ ��ġ�� �����Ŀ�, �ش� �Լ��� ��ġ�� �����ϰ� �ȴ� 
// - �Լ��� ����Ǹ� �ڽ��� ��ġ�� ���ƿ� ���� �ڵ带 �����Ѵ�.
// ����Լ��� �Լ� �ڽ��� ������ ����, �ڱ� �ڽ��� �ٽ��ѹ� ���� �ϴ� ��

// �� �Լ��� ���� ���� ����, �μ�, ���� �������� ���� �����ӿ� ����Ǳ� ������ ������ ��

// ����Լ��� ©�� ����� ��
// 1. Ż�� ���� (���� ������ ���� ���� ��)
// 2. ���̽��� �� ���� �Ұ�

// ����Լ����� Ư¡
// 1. �ڵ��� �ܼ�ȭ
// 2. ȣ��� ���ð����� Ȱ���ϹǷ� �����ϰ� ó���ϸ� ���� �����÷ο찡 ���� ����
// 3. ���� Ƚ���� ������ ���� ������, ���������� ���� ���� �޶���
// 4. ������ϱ� ����...


// ex, 1���� n������ Sum, 1���� N���� ���丮��, x�� n power squaure

int Sum(int n)
{
	// n = 0 �ϰ�� return
	if (n == 0)
		return n;

	// �ڱ� �ڽź��� �� ���� ���� Sum�� �����ش�
	return n + Sum(n - 1);
}

int factorial(int n)
{
	// n = 0�� ��� 1�� ���� 
	// �� ������ n�� 0�� ��� ��� ���丮�󿡼��� ���� 0�� ���� ���̱� ����
	if (n == 0)
		return 1;

	return n * factorial(n-1);
}


int tailfactorial(int n, int total)
{
	if (n == 0)
		return total;
	else
	{
		return tailfactorial(n - 1, total * n);
	}
}

int factorialtail(int n)
{
	return tailfactorial(n, 1);
}

// ��������
int PowerSquare(int x, int n)
{
	return x;
}

int main()
{
	int x = 5;
	int TotalSum = Sum(5);
	std::cout << TotalSum << std::endl;
	
	int factorialresult = factorial(5);
	std::cout << factorialresult << std::endl;

	int tailfactorialresult = factorialtail(5);
	std::cout << factorialresult << std::endl;

	int PS = PowerSquare(2, 3);
	std::cout << PS << std::endl;

	system("pause");
}

