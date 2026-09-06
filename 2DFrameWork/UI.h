#pragma once
class UI : public Actor
{
public:
	enum class AlignX
	{
		LEFT,
		CENTER,
		RIGHT
	};
	enum class AlignY
	{
		TOP,
		CENTER,
		BOTTOM
	};
	static UI* Create(string name = "UI");
	virtual void	Update() override;
	bool			Press = false;
	function<void()> mouseOver = nullptr; //마우스가 위에존재할떄
	function<void()> mouseDown = nullptr; // 위에서 눌렀을때 한번
	function<void()> mousePress = nullptr; // 누르고 있을때
	function<void()> mouseUp = nullptr; //누르고 떼었을때

	static Vector2 NDCLength(Vector2 worldLength);
	static float NDCLengthX(float worldLengthX);
	static float NDCLengthY(float worldLengthY);

	bool MouseOver();
	void CreateMesh(string texturePath, AlignX alignX = AlignX::CENTER, AlignY alignY = AlignY::CENTER);
	void CreateMesh(shared_ptr<Texture> texture, AlignX alignX = AlignX::CENTER, AlignY alignY = AlignY::CENTER);
	void UpdateMesh(shared_ptr<Texture> texture, AlignX alignX = AlignX::CENTER, AlignY alignY = AlignY::CENTER);
	void SetNDCPos(Vector2 pos);
};