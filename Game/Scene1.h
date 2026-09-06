#pragma once

class Scene1 : public Scene
{
private:
	Camera* Cam;
	bool camdebug = false;

	Sky* sky;
	Actor* Grid;

	//Terrain* map[9];

	enum SoundCondition
	{
		SPACE0,
		SPACE1,
		ALL_COUNTS	// 사운드의 갯수를 구할때 사용하는 상수
	};
	vector<bool> soundBoard;	// 사운드의 출력여부를 결정하는 변수
	
	Environment* envWater;
	Environment* envWaterFall;
	//Environment* envMirror;
	Environment* envWall;

	bool bCapture_envWaterFall;
	bool bCapture_envWall;
public:
	Scene1();
	~Scene1();
	static void Calculate_Init();	// 이 씬을 Init하는데에 로딩될 용량 계산
	virtual void Init() override;
	virtual void Release() override; //해제
	virtual void Update() override;
	virtual void LateUpdate() override;//갱신
	virtual void DebugRender() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void ResizeScreen() override;

	void AllObjectRender(string type = "Normal");
	void SetCheckPoint(int num);
};

