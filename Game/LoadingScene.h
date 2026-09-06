#pragma once
class LoadingScene : public Scene
{
private:
	Camera* Cam;
	UI* loadingImage;
	int curLoadingCount;
	int maxLoadingCount;
public:
	LoadingScene();
	~LoadingScene();
	virtual void Init() override;
	virtual void Release() override; //해제
	virtual void Update() override;
	virtual void LateUpdate() override;//갱신
	virtual void DebugRender() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void ResizeScreen() override;
};