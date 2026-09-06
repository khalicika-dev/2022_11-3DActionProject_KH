#pragma once


class Main : public Scene
{
private:
	Camera* Cam;
	Actor* Grid;

	Actor* Sphere;
	Actor* Sphere2;
	deque<Vector3> Way;

	float MoveValue;
	Vector3 P1, P2;
	float Dis;

	//상수버퍼에 갱신
	struct Brush
	{
		Vector3 point;
		float	range = 10.0f;

		float	shape;
		float	type;
		Vector2 padding;
	} brush;
	ID3D11Buffer* brushBuffer;

	//에디터에서만 존재하는 변수
	const UINT max_undo = 20;
	map<UINT, deque<vector<VertexTerrain>>> tempVertices;
	UINT undo_count;

	// 와이어프레임모드
	bool isWireframe = false;
	Actor* vertexTarget;
	Vector3 lastTargetPos = { 0,0,0 };
	bool modifying = false;
	Terrain* targetTerrain = nullptr;
	VertexTerrain* selectVertex = nullptr;
	//-------------------

	bool MouseBrush = true;
	float	brushMinMaxHeight[2] = { -1000.0f,1000.0f };
	float	brushAddHeightScalr = 10.0f;
	int		brushTexture = 0;
	float	TextureFillSpeed = 1.0f;
	int		nodeEdit = 0;
	int     prevPick = -1;
	bool	ObjPick = false;
	//-----------------------------
public:
	Main();
	~Main();
	virtual void Init() override;
	virtual void Release() override; //해제
	virtual void Update() override;
	virtual void LateUpdate() override;//갱신
	virtual void DebugRender() override {};
	virtual void PreRender() override {};
	virtual void Render() override;
	virtual void ResizeScreen() override;

	void EditTerrain(Vector3 Pos, Terrain* map);
	void TempTerrain(UINT num);
	void UndoTerrain(UINT num);
};