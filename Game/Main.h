#pragma once

class Main : public Scene
{
private:
	PauseMenu* pMenu;
public:
	Main();
	~Main();
	virtual void Init() override;
	virtual void Release() override; //해제
	virtual void Update() override;
	virtual void LateUpdate() override;//갱신
	virtual void DebugRender() override; // ImGui등 디버그전용 윈도우 렌더
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void ResizeScreen() override;

	bool isLoading();
	//void Resize();
};