#pragma once

class Player : public GameObject
{
	friend class Player_Interface;
	friend class ItemManager;
private:
	Object_Trail* trail = nullptr;

	// 한 모션에서 송출할 사운드칸
	enum SoundCondition
	{
		SE_SPACE0,
		SE_SPACE1,
		SE_SPACE2,
		SE_SPACE3,
		SE_SPACE4,
		SE_SPACE5,
		SE_SPACE6,
		SE_ALL_COUNTS	// 사운드의 갯수를 구할때 사용하는 상수
	};
	inline Sound::SoundKey SDKEY_FootstepA();

	enum PlayerAction
	{
		TITLE_POSE = 1,
		IDLE = 3,
		IDLE_CAPEK = 4,
		IDLE_BREATH = 5,
		MOVE = 6,
		DASH = 7,
		STEP = 8,
		STEP_AIR = 9,
		JUMP = 10,
		FALL = 11,
		FALL_LAND = 12,
		HIT = 13,
		DOWN = 14,
		DOWN_LAND = 15,
		GUARD = 16,
		GUARD_AIR = 17,
		GUARD_LAND = 18,
		GUARDHIT = 19,
		GUARDHIT_AIR = 20,
		USE_ITEM = 21,
		ATTACK_A1 = 22,
		ATTACK_A2 = 23,
		ATTACK_A3 = 24,
		ATTACK_A4 = 25,
		ATTACK_B1 = 26,
		ATTACK_B2_PrevA1 = 27,
		ATTACK_B2_PrevB1 = 28,
		ATTACK_B3 = 29,
		ATTACK_B4 = 30,
		ATTACK_StepA1 = 31,
		ATTACK_StepB1 = 32,
		ATTACK_A1_PrevStepB1 = 33,
		ATTACK_B1_PrevStepB1 = 34,
		ATTACK_AirA1 = 35,
		ATTACK_AirA2 = 36,
		ATTACK_AirA3 = 37,
		ATTACK_AirB1 = 38,
		ATTACK_AirB1_FALL = 39,
		ATTACK_AirB1_LAND = 40,
		DIE = 41,
		DIE_LAND = 42,
		CINEMATIC_001_01 = 43,
		CINEMATIC_001_02 = 44,
		CINEMATIC_001_03 = 45,
		CINEMATIC_001_04 = 46,
		CINEMATIC_001_05 = 47,
		CINEMATIC_001_06 = 48,
		CINEMATIC_001_07 = 49,
		CINEMATIC_001_09 = 50,
		CINEMATIC_001_11 = 51,
		CINEMATIC_001_12 = 52,
		CINEMATIC_001_13 = 53,
		CUTSCENE_001_01 = 54,
		CUTSCENE_001_02 = 55,
		CUTSCENE_002_01 = 56
	};
	
	struct AttackNode
	{
		float scale = 1.0f;
		UINT nextA_animIdx = 0;		// 약공격 시 이어지는 액션Key								// 0: null
		UINT nextB_animIdx = 0;		// 강공격 시 이어지는 액션Key
		float actionTime = 0.0f;	// 후속행동까지 필요한 시간
		float comboTime = 0.0f;		// 콤보를 이을 수 있는 시간
		float stopTime = 0.05f;		// 타격 시 프레임 정지시간
		float blend = 0.05f;		// Blend Time
		AttackNode() : scale(1.0f), nextA_animIdx(0), nextB_animIdx(0), actionTime(0.0f), comboTime(0.0f), stopTime(0.05f), blend(0.05f) {}
		AttackNode(float aniScale, UINT nextA, UINT nextB, float aTime, float cTime, float stop = 0.05f, float blendTime = 0.05f) : scale(aniScale), nextA_animIdx(nextA), nextB_animIdx(nextB), actionTime(aTime), comboTime(cTime), stopTime(stop), blend(blendTime) {}
	};
	bool Check_Idle() { return actor->anim->PlayingIdx() == IDLE || actor->anim->PlayingIdx() == IDLE_CAPEK || actor->anim->PlayingIdx() == IDLE_BREATH; }
	bool Check_Step() { return actor->anim->PlayingIdx() == STEP || actor->anim->PlayingIdx() == STEP_AIR; }
	bool Check_Hit() { return actor->anim->PlayingIdx() == HIT || actor->anim->PlayingIdx() == DOWN || actor->anim->PlayingIdx() == DOWN_LAND; }
	bool Check_Guard() { return actor->anim->PlayingIdx() == GUARD || actor->anim->PlayingIdx() == GUARDHIT ||
		actor->anim->PlayingIdx() == GUARD_AIR || actor->anim->PlayingIdx() == GUARDHIT_AIR || actor->anim->PlayingIdx() == GUARD_LAND;}
	bool Check_Attack();
	bool Check_MoveControl() { return INPUT->KeyPress('W') || INPUT->KeyPress('A') || INPUT->KeyPress('S') || INPUT->KeyPress('D') ||
		fabs(INPUT->tracker.GetLastState().thumbSticks.leftX) > 0.0f || fabs(INPUT->tracker.GetLastState().thumbSticks.leftY) > 0.0f;}
	bool Check_CutScene(bool isOption = false, bool isGravity = true);
	virtual bool Check_Death() override { return actor->anim->PlayingIdx() == DIE || actor->anim->PlayingIdx() == DIE_LAND; }
	virtual bool Check_Unbreakable() override;

	void Decrease_HP(int value);
public:
	
	Player();
	virtual ~Player();
	static void Calculate();	// 이 객체를 추가하기 위해 로딩될 용량 계산

	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;
	virtual void CubeMapRender() override;
	virtual void ShadowMapRender() override;
	virtual void Init() override;
	virtual void Spawn(Object* point) override;
	void RenderDetail();

	Actor* GetPlayer() { return actor; }
	float GetStRatio() { return st / st_max; }
	void Reset();
	void Start(Object* point);
private:
	float st;
	float st_max;
	float guardTime;
	float attackTime;	// 공격할 때마다 선입력이 가능한 시간
	float attackStopTime;	// 모션이 멈추는 시간
	bool attackStop;		// 모션이 멈춘 상태
	UINT Swing;		// 한 모션동안 실행한 공격횟수 (또는 이벤트 횟수) (1)
	UINT Swing2;	// 한 모션동안 실행한 이벤트횟수(2)
	float invincibleTime;
	int jumpCount;
	int stepCount;	// 공중에서 스텝가능한 횟수(공중공격시 초기화됨)
	bool airGuard;	// 공중가드를 했었는지의 여부(지상착지시에만 초기화, true면 행동불가)
	int airAttack;	// 공중공격을 했었는지의 여부(지상착지와 def행동 시 초기화, true면 이동불가)
	float kb_speed;	// 히트넉백 시 수평방향으로 날아가는 속도
	bool actionable_atk;	// 공격가능
	bool actionable_def;	// 수비가능
	bool can_input;	// 선입력가능
	int key_buffer;	// 선입력 키코드 버퍼
	Vector3 mov_buffer;	// 선입력 방향버퍼
	map<int, AttackNode> atkNode;	// 공격노드
	list<pair<ItemManager::ItemType,int>> itemList;
	int current_itemIdx;

	Collider* lockOnTarget;
	float lockOnAnyAngleTime;	// 록온을 해제한 직후 일정시간 동안은 시야 밖에서도 록온할 수 있는 시간
	const float lockOnAnyAngleMaxTime = 2.0f;	// 위 변수의 최대 지속시간

	Vector3 realDir;	// 회전이 포함된 조작 시 바라보고자 하는 방향
	Vector3 GetMov();	// 조작을 통해 움직이려는 벡터를 구하는 함수 (단위벡터x)
	float	effectTime;	// 일부 공격에 이펙트를 뿌리는 시간 (예:AirB1)
	float GetRealRot() { return atan2(-realDir.x, -realDir.z); }
	void AttackToMonster(Collider* atkCol, int dmg, int knock, Sound::SoundKey hitSound, Sound::SoundKey guardSound);
	void InitKeyBuffer() { key_buffer = 0; }

	void Resurrection();
	float resurrectionTime = 0.0f;

public:
	bool IsResurrection() { return resurrectionTime > 0.0f; }
};

