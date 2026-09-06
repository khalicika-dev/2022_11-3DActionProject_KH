#pragma once
class Monster_Goblin : public Monster
{
private:
	enum PatternType
	{
		PT_NONE,
		PT_RECOGNIZE,
		PT_ATTACK,
		PT_RUSH
	};

	// 한 모션에서 송출할 사운드칸
	enum SoundCondition
	{
		SE_SPACE0,
		SE_SPACE1,
		SE_SPACE2,
		SE_ALL_COUNTS	// 사운드의 갯수를 구할때 사용하는 상수
	};
	inline Sound::SoundKey SDKEY_FootstepA();
	inline Sound::SoundKey SDKEY_FootstepB();

	enum AniAction
	{
		APPEAR = 1,
		IDLE = 2,
		WALK = 3,
		RECOGNIZE = 4,
		BATTLE_IDLE = 5,
		RUN = 6,
		LTURN = 7,
		RTURN = 8,
		ATTACK = 9,
		RUSH = 10,
		HIT1 = 11,
		HIT2 = 12,
		JUMP = 13,
		FALL = 14,
		FALL_LAND = 15,
		DEATH = 16
	};
	bool Check_Hit() { return actor->anim->PlayingIdx() == AniAction::HIT1 || actor->anim->PlayingIdx() == AniAction::HIT2; }
	virtual bool Check_Death() override { return actor->anim->PlayingIdx() == AniAction::DEATH; }
	virtual bool Check_Unbreakable() override { return Check_Death(); }
public:

	Monster_Goblin();
	virtual ~Monster_Goblin();
	static void Calculate();	// 이 객체를 추가하기 위해 로딩될 용량 계산

	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;
	virtual void CubeMapRender() override;
	virtual void ShadowMapRender() override;
	virtual void Init() override;
	virtual void GeneratePatterns() override;
	bool Move(Vector3& pos, Vector3 dir, Vector3 toPlayerDir);

	void Appear(bool instant = false, bool activate_beware = false) override;
private:
	int breathing;	// 숨소리 재생까지 남은 BATTLE_IDLE 루프횟수
	const int max_breathing = 6;	// breathing이 가질 수 있는 최대값
};

