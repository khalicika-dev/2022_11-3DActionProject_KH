#pragma once
class Bullet_Red : public GameObject
{
public:
	GameObject* owner;
	Collider* col;

	Bullet_Red();
	virtual ~Bullet_Red();
	static void Calculate();	// 이 객체를 추가하기 위해 로딩될 용량 계산

	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;
	virtual void CubeMapRender() override;
	virtual void ShadowMapRender() override;
	virtual void Init() override;

	float range;
	Vector3 velocity;
	bool firstUpdate;
};

