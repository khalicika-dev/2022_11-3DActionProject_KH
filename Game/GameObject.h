#pragma once
class GameObject
{
	friend class Player_Interface;
protected:
	Actor* actor	= nullptr;		// 객체를 표현하는 Actor
	Actor* camPivot	= nullptr;		// 객체를 따라다니는 카메라 스프링 암
	Camera* camera	= nullptr;		// 스프링 암에 장착된 카메라
	Camera* eventCamera = nullptr;	// 특정 이벤트에서 사용하는 객체를 비추는 카메라

	bool MoveOnTerrain(Vector3 pos);
	void CameraOnTerrain(Vector3 pos, float z);
	bool ChangeAnim(AnimationState state, UINT idx, float blendtime = 0.2f, bool force = false);

	int hp;
	int hp_max;
	bool movable = false;
	bool isAir = false;
	float YSpeed = 0.0f;
	float YminSpeed = -(SCENE->gravity / 2.0f);

	bool ghost = false;	// true일때 오브젝트끼리 충돌을 안하는 상태
	virtual void DmgCor(int& damage) {}	// 대미지에 추가적으로 가하는 보정(상속용)
	virtual bool Check_Unbreakable() { return false; }	// 피격효과를 받지만 실질적 피해를 받지 않는 상태가 되는 조건인가를 판정(상속용)
	virtual bool Check_Death() { return false; }			// 대상이 리타이어한 상태인가를 판정(상속용)
	vector<bool> soundBoard;	// 사운드의 출력여부를 결정하는 변수
public:
	GameObject() {}
	virtual ~GameObject() {}

	virtual void Update() abstract;
	virtual void LateUpdate() abstract;
	virtual void Render() abstract;
	virtual void CubeMapRender() abstract;
	virtual void ShadowMapRender() abstract;
	virtual void Init() abstract;
	Actor*	GetActor() { return actor; }
	Camera* GetCamera() { return camera; }
	Object* GetCamPivot() { return camPivot->Find("CameraPivot"); }
	Camera* GetEventCamera() { return eventCamera; }
	float	GetHpRatio() { return (float)hp / (float)hp_max; }
	void	SetWorldPos(Vector3 pos) { actor->SetWorldPos(pos); }
	float	GetJump(float height, float t);
	void	Rotate(Vector3 targetDir, float speedPerSec, float angleOffset = 0.0f);
	bool	Attack(Collider* atkCol, GameType targetType);	// 공격으로 사용할 콜라이더와 피격대상이 되는 콜라이더의 타입
	void	Invincible(bool active);
	virtual void	Spawn(Object* point);

	struct DamageInfo
	{
		string hitName;			// 피격된 부위(HitCollider)의 이름
		Vector3 atkPos;			// 공격의 진원지 (WorldPos)
		Vector3 hitPos;			// 피격의 진원지 (WorldPos)
		DmgNumber::DmgType dmgType;	// 크리티컬에 따른 데미지수준
		int damage;				// 데미지(crit포함)
		int knockdown;			// [몬스터]넉다운게이지(knock_crit포함)
		Vector2 kb_velocity;	// [플레이어]넉백속도 (0이면 제자리 경직), value는 [수평, 수직]
		Sound::SoundKey hitSound;		// 피격 시 효과음
		Sound::SoundKey guardSound;		// 가드 시 효과음
		DamageInfo() {}
		DamageInfo(string hitName, Vector3 atkPos, Vector3 hitPos, DmgNumber::DmgType dmgType, int damage, int knockdown, Vector2 kb_velocity = { 0,0 }, Sound::SoundKey hitSound = Sound::SoundKey::NONE, Sound::SoundKey guardSound = Sound::SoundKey::NONE)
			: hitName(hitName), atkPos(atkPos), hitPos(hitPos), dmgType(dmgType), damage(damage), knockdown(knockdown), kb_velocity(kb_velocity), hitSound(hitSound), guardSound(guardSound) {}
	};
	vector<DamageInfo> damageInfo;	// 누적된 피해내역
	

	bool Enable = true;
	int weight;	// 해당 오브젝트의 체급. 자기보다 낮은 체급의 오브젝트의 블록콜라이더를 무시할 수 있다.
};