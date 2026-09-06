#pragma once
class Object_Trail
{
	friend class GObjectManager;
private:
	Actor* actor;
	Object* trailStart;
	Object* trailEnd;

	const UINT trailMaxCount = 10;
	UINT vertexCount;
	deque<Vector3> startPos;
	deque<Vector3> endPos;
	const float trailAddTimeMax = 0.8f / 60.0f;
	float trailAddTime;
	bool isAdd;
	void RemoveTrail();
public:
	Object_Trail(Object* trailStart, Object* trailEnd);
	~Object_Trail();
	static void Calculate();	// 이 객체를 추가하기 위해 로딩될 용량 계산

	void Update();
	void Render();
	void Init();

	void UpdateTrail();
	void AddTrail();
};