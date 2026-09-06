#pragma once
class Monster : public GameObject
{
	friend class Player_Interface;
protected:
	virtual bool Check_Death() override { return false; }
	virtual bool Check_Unbreakable() override;
protected:
	// 패턴 구조
	// 1. (패턴준비)특정패턴을 사용하기 위해 필요한 거리까지 이동한다. 이동 시 타겟의 위치를 주기적으로 갱신한다.
	// 2. 이동 중 방향불문하고 필요한 거리만큼 접근했으면(실시간 측정) 준비한 패턴을 시행한다. 일정시간내로 접근하지 못하면 다른 패턴을 준비한다. (중복가능)
	// 3. 패턴을 시행할 때 목표의 방향이 틀어져있으면 회전모션을 사용한다. 차가 적을수록 모션의 속도가 빨라진다.
	// 4. 타겟위치 갱신 시 직선거리(ray)로 벽이 없으면 직선경로를 사용한다. 벽이 있으면 다익스트라 길찾기를 통해 얻은 경로들을 사용한다.
	// 5. 다익스트라 길찾기 시 경로리스트에 (본인과 가장 가까운)첫 노드와 (타겟과 가장 가까운)마지막노드를 제거하여 빙 도는 일이 없도록 한다. [2번째노드]->[3]->[4]->...->[n-2]->[n-1]->타겟위치 순으로 이동
	// 6. 5번을 위해서는 다익스트라의 노드배치를 코너마다 배치할 필요가 있다. 무리하지 않을만큼 간격을 좁혀두는 것이 좋음.
	struct PatternData
	{
		int   playing;					// 실행상태 (0=이동중, 1=실행확정, 2=실행중, 3=종료)
		list<int> actionNums;			// 패턴과 연관된 모션들(첫번째 칸이 실행시 먼저 실행될 첫 모션, 마지막 칸이 마지막으로 실행될 모션)
		float aniScale;					// 첫 모션의 속도
		float duration;					// 패턴의 지속시간(최대치) (실행중에 time이 감소함)
		float cooldown;					// 재사용 대기시간(최대치) (사용대기중에 time이 감소함)
		float distance;					// 패턴 사용을 위한 최대거리
		function<bool()> condition;		// 랜덤한 패턴으로 선정되기 위한 조건 (cooldown과는 무관)

		bool  mustUse = false;			// 이 패턴은 추적해서라도 무조건 사용해야할 때 true
		float first_cooldown = 0.0f;	// 초기 재사용 대기시간 (전투시작 시 적용됨)
		float time = 0.0f;				// 남은시간
		//PatternData() 
		//	: playing(false), actionNum(-1), duration(0.0f), cooldown(0.0f), distance(0.0f), time(0.0f), condition([]() {return false; }) {}
		PatternData() {}
		//PatternData(int actionNum, float duration, float cooldown, float distance, function<bool()> condition)
		//	: playing(0), actionNum(actionNum), aniScale(1.0f), duration(duration), cooldown(cooldown), distance(distance), condition(condition) {}
		PatternData(list<int> actionNums, float duration, float cooldown, float distance, function<bool()> condition)
			: playing(0), actionNums(actionNums), aniScale(1.0f), duration(duration), cooldown(cooldown), distance(distance), condition(condition) {}
	};
	int currentPattern = -1;	// 현재 사용할 패턴
	int lastPattern = -1;		// 마지막으로 사용한 패턴
	map<int, PatternData> pattern;
	
	bool beware = false;
	Vector3 destPos = { 0,0,0 };
	bool destJump = false;	// 자신과 타겟 사이에 점프장애물이 있음.
	deque<Vector3> Way;

	float targetTime = 0.0f;	// 타겟의 위치를 갱신하는 주기
	float startRot;		// 회전할 때의 시작각도
	float destRot;

	float st;
	float st_max;

	float corpseTime;
	float maxCorpseTime = 10.0f;
public:
	Monster() {}
	virtual ~Monster() {}

	virtual void Update() abstract;
	virtual void LateUpdate() abstract;
	virtual void Render() abstract;
	virtual void CubeMapRender() abstract;
	virtual void Init() abstract;
	virtual void GeneratePatterns() abstract;
	void PatternInit();
	void AllPatternUpdate();
	void ChangePattern(int patternNum);
	bool RandomPattern();
	void destUpdate(Vector3 pos, Vector3 playerPos, Ray toPlayerRay);
	bool IsKilled() { return hp <= 0.0f; }

	virtual void Appear(bool instant = false, bool beware = false) {};
	void ResetHP(int value) { hp_max = hp = value; }	// 특정 상황에서 소환 시 해당 몬스터의 체력 재조정
	void ResetST(int value) { st_max = st = value; }	// 특정 상황에서 소환 시 해당 몬스터의 스태미나 재조정
};