#pragma once
class GameTimer
{
public:
	GameTimer();
	~GameTimer();

	float GetGameTime() const;
	float GetDeltaTime() const;

	void Reset(); // �޽��� ���� ������ ȣ���������
	void Start(); // Ÿ�̸� ����
	void Stop(); // �Ͻ� ������ ȣ��
	void Tick(); // �� �����Ӹ��� ȣ��

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

