#pragma once
class PauseMenu
{
private:
	enum class MenuKey
	{
		NONE,
		TITLE,
		PAUSE,
		HTP,
		OPTION,
		OPTION_CONFIRM,
		RESET_CONFIRM,
		RESTART_CONFIRM,
		EXIT_CONFIRM
	};
	struct MenuProperty
	{
		string name;
		list<MenuKey> childs;
		function<void(PauseMenu&, bool)> showFunc;
		float width;
		float heightRatio;
		bool b_open = false;
		bool isConfirmType;
		ImGuiWindowFlags addFlags;
		MenuProperty() {}
		MenuProperty(string name, list<MenuKey> childs, float width, float heightRatio, function<void(PauseMenu&, bool)> showFunc, bool confirmType = false, ImGuiWindowFlags addFlags = ImGuiWindowFlags_None) 
			: name(name), childs(childs), width(width), heightRatio(heightRatio), showFunc(showFunc), isConfirmType(confirmType), addFlags(addFlags) {}
	};
	struct SignalProperty
	{
		bool b_received;	// 신호를 받음
		bool b_allClose;	// 모든 메뉴닫기 신호
		MenuKey targetName;	// 신호를 받는 메뉴
	}closeSignal;
	bool b_menuAllClosing;	// 모든 메뉴를 닫기 위한 변수. 이 변수가 켜져 있는 한 모든 메뉴를 닫는 시도를 한다.

	bool b_pauseMenuLoaded;	// 일시정지 메뉴가 불러와있는 상태.
	
	shared_ptr<Texture> tex_cursor;
	ImVec2 cursorSize;
	//int idxTemp_screenSetting;
	//int idxTemp_resolutionSetting;
	bool tempVar[10];
	ConfigManager::ConfigSettingsStruct tempSettings;
public:
	PauseMenu();
	~PauseMenu();
	void Update();
	void RenderCursor();
	void SendCloseSignal(bool b_AllClose) { closeSignal.b_received = true; closeSignal.b_allClose = b_AllClose; }
private:
	map<MenuKey, MenuProperty> menus;

	void CopySettings();

	void ShowMenu(MenuProperty& menu, bool closeSignal);
	void ShowPauseMenu(bool closeSignal);
	void ShowTitleMenu(bool closeSignal);
	void ShowHtpMenu(bool closeSignal);
	void ShowOptionMenu(bool closeSignal);
	void ShowOptionConfirm(bool closeSignal);
	void ShowResetConfirm(bool closeSignal);
	void ShowExitConfirm(bool closeSignal);
	void ShowRestartConfirm(bool closeSignal);
};