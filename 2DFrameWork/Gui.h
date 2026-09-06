#pragma once
class Gui :public Singleton<Gui>
{
public:
	static LRESULT MsgProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
	class Object* target;
	ImFont* smallFont;
	ImFont* bigFont;
private:
	UINT fileID;
	void SetNavInput(DeviceState deviceState, ImGuiNavInput navInput);
public:
	Gui();
	~Gui();
	void ResizeScreen();

	void Update();
	void Render();
	void DebugRender();
	bool FileImGui(string button, string name, string fiter, string minPath);
public:
	enum EnumTextAlign
	{
		EnumTextAlignLeading,
		EnumTextAlignCenter,
		EnumTextAlignTrailing
	};
	void TextCentered(string text);
	void TextWithPadding(string text, float width, EnumTextAlign align);
};