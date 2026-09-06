#pragma once
class Camera : public Actor
{
	friend class CameraControl;
public:
	enum class CameraState
	{
		NOMRAL = 0,			// 자유 (그 어떤 사물도 비추지 않음)
		PLAYER = 1,			// 플레이어를 비춤
		OTHER_EVENT = 2,	// 플레이어 외 객체의 이벤트카메라 사용
		PLAYER_EVENT = 3,	// 플레이어의 이벤트카메라 사용
		SCENE_CAM = 4		// 씬에 배치한 카메라를 사용한다.
	};
private:
	static ID3D11Buffer* VPBuffer;
	static ID3D11Buffer* VBuffer;
	static ID3D11Buffer* PBuffer;
	static ID3D11Buffer* viewPosBuffer;
	static ID3D11Buffer* viewUpBuffer;
	static ID3D11Buffer* viewForwardBuffer;

	static Actor* followPivot;	// 현재 팔로우중인 메인카메라의 구조
	static Camera* main;	// 현재 사용중인 카메라
	static Camera* follow;	// 메인을 따라가는 카메라

	static CameraState camState;	// 현재 카메라가 나타내야할 곳을 가리키는 방향(또는 상태)
public:
	static void CreateStaticMember();
	static void DeleteStaticMember();
	static Camera* Create(string name = "Camera");

	static void ControlMainCam(float scalar = 100.0f);
	static void FollowMainCam(float scalarPos, float scalarRot);
	static void SetMainCam(Camera* target);

	static Camera* GetMainCam() { return main; }
	static Camera* GetFollowCam() { return follow; }
	static bool Intersect(BoundingFrustum Frustum, Collider* col);
private:
	
public:
	Matrix		view, proj;
	Viewport	viewport;
	bool		ortho;
	float		fov,nearZ,farZ, width, height;
protected:
	Camera();
	~Camera();
public:
	void	Update() override;
	void	Set();
	void	SetShadow();
	void	RenderDetail();
	bool    Intersect(Vector3 coord);
	bool    Intersect(Collider* col);
};

class CameraControl
{
	friend class Main;
private:
	// Main함수에서만 조작가능한 함수
	static Camera::CameraState	GetCamState() { return Camera::camState; }
	static void	InitCamState() { Camera::camState = Camera::CameraState::NOMRAL; }
public:
	// 우선순위가 높은 순으로 Set이 되는 함수. NORMAL로 시작해서 Scene내에서 여러 Set을 거친 후 최종값으로 보고자 하는 카메라를 표시한다.
	static void	SetCamState(Camera::CameraState state) { if (Camera::camState < state) Camera::camState = state; } 
};