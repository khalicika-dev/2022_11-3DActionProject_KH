#pragma once
class DmgNumber
{
public:
	enum class DmgType
	{
		NORMAL, CRITICAL, ENDURE, HEAL
	};
	static shared_ptr<Texture> numTex[10];
	static float imgHalfWidth;
private:
	bool enable;
	Vector3 worldPos = { 0,0,0 };
	DmgType dmgType = DmgType::NORMAL;
	
	Actor* root;
	UI* number[6];
public:
	float time = 0.0f;
	static void CreateStaticMember();
	static void DeleteStaticMember();
	static void CalculateStaticMember();
	DmgNumber();
	~DmgNumber();

	void Update();
	void Render();

	void Play(Vector3 pos, int value, DmgType type);
	void Stop() { enable = false; }
};

