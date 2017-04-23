#include "stdafx.h"
#include "GameTimer.h"

GameTimer gGameTimer;

GameTimer::GameTimer()
{
	// 개당 초수 설정
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	SecondsPerCount = 1.0 / (double)countsPerSec;
}


GameTimer::~GameTimer()
{
}

float GameTimer::GetGameTime() const
{
	// 현재 타이머가 중단 상태이면, 중단된 이후로 부터 흐른시간은 계산하지 않는다.
	// 또한 이전에 일시 정지된 적이 있따면 전체시간에 포함시키지 말아야 하므로,
	// StopTime에서 일시정지시간을 뺀채로 계산한다.
	if (bStopped)
	{
		return static_cast<float>(((StopTime - PausedTime) - BaseTime) * SecondsPerCount);
	}
	
	// 시간차 CurrTime -= BaseTime에는 일시정지된시간(PauseTime)이 포함되어있으므로,
	// 그시간을 제해준다
	else
	{
		return static_cast<float>(((CurrTime - PausedTime) - BaseTime)*SecondsPerCount);
	}
}

float GameTimer::GetDeltaTime() const
{
	return static_cast<float>(DeltaTime);
}

void GameTimer::Reset() // 메시지 루프 이전에 호출해줘야함
{
	__int64 localCurrTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&localCurrTime);

	BaseTime = localCurrTime;
	PrevTime = localCurrTime;
	StopTime = 0;
	bStopped = false;
}

void GameTimer::Start() // 타이머 시작
{
	__int64 localStartTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&localStartTime);
	if (bStopped)
	{
		// 정지되어있었던 시간을 누적해준다.
		PausedTime += (localStartTime - StopTime);
		
		// 타이머를 다시시작하므로 원래의 PrevTime은 Valid하지 않으므로 갱신해준다.
		PrevTime = localStartTime;

		StopTime = 0;
		bStopped = false;
	}
}

void GameTimer::Stop() // 일시 정지시 호출
{
	if (!bStopped)
	{
		__int64 localCurrTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&localCurrTime);

		// 타이머가 정지되었다면 정지된 시점의 시간을 저장하고
		// 타이머가 중단되었음을 뜻하는 플래그 설정
		StopTime = localCurrTime;
		bStopped = true;
	}
}

void GameTimer::Tick() // 매 프레임마다 호출
{
	if (bStopped)
	{
		DeltaTime = 0.0;
		return;
	}

	// 이번 프레임의 시간을 가져온다.
	__int64 localcurrTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&localcurrTime);
	CurrTime = localcurrTime;

	// 이 시간과 이전 프레임의 시간의 차이를 구한다.
	DeltaTime = (CurrTime - PrevTime)*SecondsPerCount;

	// 다음 프레임을 준비한다.
	PrevTime = CurrTime;

	//음수가 되지 않도록 한다. 
	//프로세서가 절전 모드로 들어가거나 다른 프로세스와 엉키는 경우, 음수가 될수 있다고 한다.

	if (DeltaTime < 0.0)
	{
		DeltaTime = 0.0;
	}
}
