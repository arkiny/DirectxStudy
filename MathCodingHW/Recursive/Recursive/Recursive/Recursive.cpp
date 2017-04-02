#include <iostream>
#include <string>

// Recursive, 재귀함수란.
// 기존의 함수 = 함수를 만나면 현재의 위치를 저장후에, 해당 함수의 위치로 점프하게 된다 
// - 함수가 종료되면 자신의 위치로 돌아와 원래 코드를 수행한다.
// 재귀함수는 함수 자신이 끝나기 전에, 자기 자신을 다시한번 실행 하는 것

// 각 함수에 대한 복귀 번지, 인수, 지역 변수들이 스택 프레임에 저장되기 때문에 가능한 것

// 재귀함수를 짤때 고려할 것
// 1. 탈출 조건 (무한 루프를 막기 위한 것)
// 2. 케이스를 잘 설정 할것

// 재귀함수를의 특징
// 1. 코드의 단순화
// 2. 호출시 스택공간을 활용하므로 무리하게 처리하면 스택 오버플로우가 날수 있음
// 3. 가능 횟수는 스택의 남은 공간과, 지역변수의 수에 따라 달라짐
// 4. 디버깅하기 힘듬...


// ex, 1부터 n까지의 Sum, 1부터 N까지 팩토리얼, x의 n power squaure

int Sum(int n)
{
	// n = 0 일경우 return
	if (n == 0)
		return n;

	// 자기 자신보다 한 숫자 작은 Sum과 더해준다
	return n + Sum(n - 1);
}

int factorial(int n)
{
	// n = 0일 경우 1을 리턴 
	// 그 이유는 n이 0일 경우 모든 팩토리얼에서의 답이 0이 나올 것이기 떄문
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

// 연습문제
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

