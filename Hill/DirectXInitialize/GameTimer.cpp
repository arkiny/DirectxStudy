#include "stdafx.h"
#include "GameTimer.h"

GameTimer gGameTimer;

GameTimer::GameTimer()
{
	// ���� �ʼ� ����
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	SecondsPerCount = 1.0 / (double)countsPerSec;
}


GameTimer::~GameTimer()
{
}

float GameTimer::GetGameTime() const
{
	// ���� Ÿ�̸Ӱ� �ߴ� �����̸�, �ߴܵ� ���ķ� ���� �帥�ð��� ������� �ʴ´�.
	// ���� ������ �Ͻ� ������ ���� �ֵ��� ��ü�ð��� ���Խ�Ű�� ���ƾ� �ϹǷ�,
	// StopTime���� �Ͻ������ð��� ��ä�� ����Ѵ�.
	if (bStopped)
	{
		return static_cast<float>(((StopTime - PausedTime) - BaseTime) * SecondsPerCount);
	}
	
	// �ð��� CurrTime -= BaseTime���� �Ͻ������Ƚð�(PauseTime)�� ���ԵǾ������Ƿ�,
	// �׽ð��� �����ش�
	else
	{
		return static_cast<float>(((CurrTime - PausedTime) - BaseTime)*SecondsPerCount);
	}
}

float GameTimer::GetDeltaTime() const
{
	return static_cast<float>(DeltaTime);
}

void GameTimer::Reset() // �޽��� ���� ������ ȣ���������
{
	__int64 localCurrTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&localCurrTime);

	BaseTime = localCurrTime;
	PrevTime = localCurrTime;
	StopTime = 0;
	bStopped = false;
}

void GameTimer::Start() // Ÿ�̸� ����
{
	__int64 localStartTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&localStartTime);
	if (bStopped)
	{
		// �����Ǿ��־��� �ð��� �������ش�.
		PausedTime += (localStartTime - StopTime);
		
		// Ÿ�̸Ӹ� �ٽý����ϹǷ� ������ PrevTime�� Valid���� �����Ƿ� �������ش�.
		PrevTime = localStartTime;

		StopTime = 0;
		bStopped = false;
	}
}

void GameTimer::Stop() // �Ͻ� ������ ȣ��
{
	if (!bStopped)
	{
		__int64 localCurrTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&localCurrTime);

		// Ÿ�̸Ӱ� �����Ǿ��ٸ� ������ ������ �ð��� �����ϰ�
		// Ÿ�̸Ӱ� �ߴܵǾ����� ���ϴ� �÷��� ����
		StopTime = localCurrTime;
		bStopped = true;
	}
}

void GameTimer::Tick() // �� �����Ӹ��� ȣ��
{
	if (bStopped)
	{
		DeltaTime = 0.0;
		return;
	}

	// �̹� �������� �ð��� �����´�.
	__int64 localcurrTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&localcurrTime);
	CurrTime = localcurrTime;

	// �� �ð��� ���� �������� �ð��� ���̸� ���Ѵ�.
	DeltaTime = (CurrTime - PrevTime)*SecondsPerCount;

	// ���� �������� �غ��Ѵ�.
	PrevTime = CurrTime;

	//������ ���� �ʵ��� �Ѵ�. 
	//���μ����� ���� ���� ���ų� �ٸ� ���μ����� ��Ű�� ���, ������ �ɼ� �ִٰ� �Ѵ�.

	if (DeltaTime < 0.0)
	{
		DeltaTime = 0.0;
	}
}
