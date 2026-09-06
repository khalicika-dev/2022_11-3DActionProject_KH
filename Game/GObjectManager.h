#pragma once
class GObjectManager : public Singleton<GObjectManager>
{
public:
	// 씬에서 추가&설정하는 오브젝트들
	enum class GOType
	{
		PLAYER,
		MONSTER_GOBLIN,
		MONSTER_DRONE,
		MONSTER_GIANT,
	};

	// 오브젝트가 생성하는 오브젝트
	enum class ParticleType
	{
		Effect_Hit,
		Bullet_Red
	};

	struct HitInfo
	{
		Collider* collider;
		GameObject* owner;
		HitInfo(Collider* col, GameObject* owner) : collider(col), owner(owner) {}
	};
	map<string, GameObject*> obj_pool;
	multimap<ParticleType, shared_ptr<GameObject>> m_particles;	// 특수오브젝트를 보관
	queue<Object_Trail*> q_trails;	// 현 프레임에서 렌더할 Trail을 보관

	GameObject* eventTarget;		// Camera의 camState가 OTHER_EVENT일때, 이벤트에 중심으로 비춰줄 플레이어 외 객체

	/*
	* 모든 오브젝트의 Hit콜라이더의 주소를 가리키는 벡터를 생성
	* 매 업데이트마다 벡터를 비우고 활성화된 Actor의 활성화된 모든 Hit콜라이더의 주소들을 받는다
	*
	* Attack콜라이더에는 데미지, 넉백유무를 표기한다.
	* Hit콜라이더에는 치명타유무, 받는 대미지 비율을 표기한다.
	* 충돌된 콜라이더에는 피격된 Hit콜라이더의 주소를 넣어 중복피격을 방지한다
	* Attack콜라이더의 enable이 false가 되면 피격대상 목록을 초기화한다.
	* 
	* 히트콜라이더
	*  : 이름은 HitCollider1_1 의 꼴로 표기
	*  : 이름에 붙은 앞의 숫자는 피격되는 콜라이더의 큰 분류이다. 피격시 앞숫자가 같은 모든 콜라이더는 해당 어택콜라이더에 피격된 것으로 처리된다.
	* 
	* 어택콜라이더
	*  : 이름은 AttackCollider1_1 의 꼴로 표기
	*  : 이름에 붙은 앞의 숫자는 공격하는 콜라이더의 큰 분류이다. 히트콜라이더 타격 시 해당 히트콜라이더의 앞숫자가 똑같은 모든 콜라이더를 자신의 앞숫자가 똑같은 모든 콜라이더의 중복타격방지 벡터에 추가한다.
	*  : enable이 false가 되면 중복타격방지 벡터를 초기화한다.
	*/
	vector<HitInfo> HitList;
private:
	string Type2Name(GOType type, UINT num) { return to_string(static_cast<int>(type)) + to_string(num); }

public:
	GObjectManager();
	~GObjectManager();
	void InitPool();
	void CalculatePool();
	void ClearHitList() { HitList.clear(); }
	void Update(GOType type, UINT num);
	void LateUpdate(GOType type, UINT num);
	void Render(GOType type, UINT num);
	void CubeMapRender(GOType type, UINT num);
	void ShadowMapRender(GOType type, UINT num);
	void Prepare(GOType type, UINT num);
	void Calculate(GOType type, UINT num);
	GameObject* Find(GOType type, UINT num);
	Monster* Find_Mob(GOType type, UINT num);
	Player* FindPlayer();	// Player를 바로 Find하는 함수

	// 투사체, 파티클 등 특수오브젝트 관련 함수
	void Particles_Update();
	void Particles_LateUpdate();
	void Particles_Render();
	void All_Particle_ActiveOff();
	void Trails_Render();
	shared_ptr<GameObject> Generate(ParticleType type, Vector3 pos, bool isStore = false);
};