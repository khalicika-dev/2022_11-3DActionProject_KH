#pragma once
class Monster_Giant : public Monster
{
	friend class Player_Interface;
	/*
	* 그로기 초당 30 회복
	* 발 하나 파괴할 때마다 그로기 회복속도 15씩 감소
	* 파괴 된 부위 당 그로기보정이 +0.2배, 그로기 시 데미지보정이 +0.2배
	* 
	* 머리: dam(x2.0, 파괴 시 x2.5), knock(x2.0, 파괴 시 x2.5)
	* 왼쪽 어깨: dam(x1.1, 파괴 시 x1.3), knock(x0.8, 파괴 시 x0.85)
	* 오른쪽 어깨: dam(x1.1, 파괴 시 x1.3), knock(x0.8, 파괴 시 x0.85)
	* 왼쪽 발: dam(x0.25, 파괴 시 x0.8), knock(x1.0, 파괴 시 x1.2), 피격 시 그로기수치 1.2배정도 보정
	* 오른쪽 발: dam(x0.25, 파괴 시 x0.8), knock(x1.0, 파괴 시 x1.2), 피격 시 그로기수치 1.2배정도 보정
	*/
private:
	Object_Trail* trail = nullptr;
	enum PatternType
	{
		PT_NONE,
		PT_RECOGNIZE,
		PT_ATTACK1,
		PT_ATTACK2,
		PT_ATTACK3,
		PT_ATTACK_STOMP,
		PT_ATTACK_JUMP,
		PT_BACKSTEP,
		PT_TAUNT
	};

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
		SE_SPACE7,
		SE_SPACE8,
		SE_SPACE9,
		SE_SPACE10,
		SE_SPACE11,
		SE_SPACE12,
		SE_SPACE13,
		SE_SPACE14,
		SE_SPACE15,
		SE_ALL_COUNTS	// 사운드의 갯수를 구할때 사용하는 상수
	};

	enum AniAction
	{
		IDLE = 0,
		RECOGNIZE = 1,
		WALK = 2,
		LTURN = 3,
		RTURN = 4,
		ATTACK1 = 5,
		ATTACK2 = 6,
		ATTACK3 = 7,
		ATTACK_STOMP = 8,
		TAUNT = 9,
		ATTACK_JUMP_BEGIN = 10,
		ATTACK_JUMP_END = 11,
		BACKSTEP = 12,
		STUN_L = 13,
		STUN_R = 14,
		GROGGY_BEGIN = 15,
		GROGGY_ING = 16,
		GROGGY_END = 17,
		DEATH = 18,
		CINEMATIC_001_08 = 19,
		CINEMATIC_001_10 = 20,
		CINEMATIC_001_12 = 21,
		CINEMATIC_001_13 = 22
	};
	bool Check_Hit() { return actor->anim->PlayingIdx() == STUN_L || actor->anim->PlayingIdx() == STUN_R; }
	virtual bool Check_Death() override { return actor->anim->PlayingIdx() == AniAction::DEATH; }
	bool Check_Groggy(bool bExceptEnd = false) { 
		return actor->anim->PlayingIdx() == GROGGY_BEGIN ||
			actor->anim->PlayingIdx() == GROGGY_ING ||
			(actor->anim->PlayingIdx() == GROGGY_END && !bExceptEnd);
	}
	bool Check_CutScene();

	void Decrease_ST(int value, OUT float* feet_knockDmg) { if (!Check_Hit() && !Check_Groggy()) st -= value; if (feet_knockDmg != nullptr) *feet_knockDmg += value; }
	void Decrease_Groggy(int value) { if (!Check_Groggy()) groggy -= value; }
	void SummonMonster(int GOTypeID, UINT num);
	void AttackPlayerAndMonster(Collider* atkCol);
	virtual void DmgCor(int& damage) override;
private:
	enum class PartsName
	{
		HEAD, 
		LSHOULDER, RSHOULDER,
		LFOOT, RFOOT
	};
	struct PartsStatus
	{
		InfoMsg::Subject name;	// 파츠의 이름(파괴 시 로그를 통해 출력)
		int hp_Parts;			// 파츠의 체력
		int hpMax_Parts;		// 파츠의 최대체력
		int destroyDamage;		// 파츠 파괴시 스태미나/그로기 수치 감소량
		Object* PopLocation;	// 파츠 파괴시 이펙트 발생위치
		vector<int>	vertices;	// 해당 파츠를 이루는 버텍스 모음
		bool bDestroy;			// 파괴여부
		PartsStatus() {}
		PartsStatus(InfoMsg::Subject name, int hp, int destroyDamage, Object* location)
			:name(name), hp_Parts(hp), hpMax_Parts(hp), destroyDamage(destroyDamage), PopLocation(location), vertices(vector<int>(0)), bDestroy(false) {}
	};
	map<PartsName, PartsStatus> parts;
	void SetPartsTexWeights(PartsName partsName, const vector<int>& vertices, float value);
	Vector3 GetPos_AnkleL();
	Object* GetObj_AnkleL();
	Vector3 GetPos_FootL();
	Object* GetObj_FootL();
	Vector3 GetPos_FootR();
	Object* GetObj_FootR();
	Vector3 GetPos_WeaponBlade();
	Object* GetObj_WeaponBlade();
	Vector3 GetPos_WeaponHandle();

	int Swing = 0;
	int taunt_level = 0;	// 잡몹 소환페이즈 구분
	float groggy;
	float groggy_max;
	float groggy_bonus;

	float jump_lastYOffset;	// 점프공격 중 1프레임동안 낙하하는 거리
	float jump_minDistance = 50.0f;	// 도약공격이 가능한 최소거리
	float jump_time;		// 도약중인 시간
	Vector3	jump_startPos;	// 점프공격 시작지점
	bool b_deathSceneSkip;	// 처치씬 스킵유무

	Sound::ChannelNode* motorChannel;	// 루프되는 motor사운드를 내는 채널
	Sound::ChannelNode* tempChannel;	// 특수처리를 위해 임시로 지정된 채널
	float soundTime;					// 특정 효과음을 주기적으로 재생하기위해 사용하는 변수
public:

	Monster_Giant();
	virtual ~Monster_Giant();
	static void Calculate();	// 이 객체를 추가하기 위해 로딩될 용량 계산

	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;
	virtual void CubeMapRender() override;
	virtual void ShadowMapRender() override;
	virtual void Init() override;
	virtual void GeneratePatterns() override;
	void RenderDetail();
	bool Move(Vector3& pos, Vector3 dir, Vector3 toPlayerDir);

	void Appear(bool instant = false, bool beware = false) override;
	float GetGroggy() { return groggy; }
	float GetGroggyMax() { return groggy_max; }
};

