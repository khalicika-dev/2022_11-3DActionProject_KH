#pragma once
class Window :public Singleton<Window>
{
	//static
public:
	enum EnumScreenSettings
	{
		ScreenSettings_WINDOW,
		ScreenSettings_FULLSCREEN,
		ScreenSettings_ALL_COUNTS
	};
	enum EnumResolutionSettings
	{
		ResolutionSettings_DEFAULT,
		ResolutionSettings_800x600,
		ResolutionSettings_1024x768,
		ResolutionSettings_1152x864,
		ResolutionSettings_1280x720,
		ResolutionSettings_1360x768,
		ResolutionSettings_1366x768,
		ResolutionSettings_1600x900,
		ResolutionSettings_1920x1080,
		ResolutionSettings_ALL_COUNTS
	};
private:
	static void ClipToWindow(HWND handle) noexcept;
	static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

	//member
private:
	static Scene* main;
	pair<bool, EnumScreenSettings> windowSignal;
	pair<bool, Int2> resolutionSignal;
public:
	void	Create();
	void	Destroy();
	WPARAM	Run(Scene* main);
	void	Load();
	void	Save();
	void	ChangeWindow(EnumScreenSettings value) { windowSignal = make_pair(true, value); }
	void	ChangeResolution(Int2 value) { resolutionSignal = make_pair(true, value); }
	void	Resize(float x, float y, float w, float h);
};

