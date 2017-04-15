#pragma once
class GameTimer
{
public:
	GameTimer();
	~GameTimer();

	float GetGameTime() const;
	float GetDeltaTime() const;

	void Reset(); // 메시지 루프 이전에 호출해줘야함
	void Start(); // 타이머 시작
	void Stop(); // 일시 정지시 호출
	void Tick(); // 매 프레임마다 호출

private:
	double SecondsPerCount = 0;
	double DeltaTime = -1.0;

	__int64 BaseTime = 0;
	__int64 PausedTime = 0;
	__int64 StopTime = 0; 
	__int64 PrevTime = 0;
	__int64 CurrTime = 0;

	bool bStopped = false;
};

extern GameTimer gGameTimer;

