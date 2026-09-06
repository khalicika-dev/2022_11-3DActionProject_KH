#pragma once
class Monster_Drone : public Monster
{
private:
	enum PatternType
	{
		PT_NONE,
		PT_RECOGNIZE,
		PT_ATTACK,
		PT_BURST
	};

	// 한 모션에서 송출할 사운드칸
	enum SoundCondition
	{
		SE_SPACE0,
		SE_SPACE1,
		SE_SPACE2,
		SE_ALL_COUNTS	// 사운드의 갯수를 구할때 사용하는 상수
	};

	enum AniAction
	{
		IDLE = 0,
		APPEAR = 1,
		RECOGNIZE = 2,
		MOVE = 3,
		ATTACK = 4,
		BURST = 5,
		HIT = 6,
		FALL = 7,
		DEATH = 8,
	};
	bool Check_Hit() { return actor->anim->PlayingIdx() == AniAction::HIT; }
	virtual bool Check_Death() override { return actor->anim->PlayingIdx() == AniAction::DEATH || (hp <= 0.0f && actor->anim->PlayingIdx() == AniAction::FALL); }
	virtual bool Check_Unbreakable() override { return Check_Death(); }

	float dist_needFloating;
	float dist_lastFloating;
	float fly_climbY;	// 점프벽을 오르기 위해 필요한 Y좌표
	bool jumpingFly;
	Vector3 targetRot;
	int Swing = 0;

	Sound::ChannelNode* motorChannel;	// 루프되는 motor사운드를 내는 채널
public:

	Monster_Drone();
	virtual ~Monster_Drone();
	static void Calculate();	// 이 객체를 추가하기 위해 로딩될 용량 계산

	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;
	virtual void CubeMapRender() override;
	virtual void ShadowMapRender() override;
	virtual void Init() override;
	virtual void GeneratePatterns() override;
	bool Move(Vector3& pos, Vector3 dir, Vector3 toPlayerDir);

	void Spawn(Object* point) override;
	void Appear(bool instant = false, bool activate_beware = false) override;
};

