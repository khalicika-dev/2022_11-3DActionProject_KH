#pragma once
class Variables : public Singleton<Variables>
{
public:
	// 플레이용 변수
	bool		isPause = false;
	bool		enableVibration = true;
	bool		showColliderComponents = false;

	// 디버그 변수
	float		debugAniBlend = 0.2f;
	bool		isDebug = true;
	bool		playLock = false;
};