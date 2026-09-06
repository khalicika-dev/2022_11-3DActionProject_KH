#pragma once


class Application
{
	friend class Direct3D11;
	friend class Window;
	friend class Gui;
	friend class Input;
	friend class ConfigManager;
	friend class PauseMenu;
	friend class ResourceManager;
private:
	float		x, y;
	float		width;			// 앱의 크기(해상도와는 관련없음)
	float		height;
	HINSTANCE	instance;
	HWND		handle;
	wstring		appName;
	bool		fullScreen;
	uint64_t	currentLoadingCapacity = 0;	// 현재 로딩된 용량 (디스플레이 표시용)
	uint64_t	totalLoadingCapacity = 0;	// 불러와야하는 최대용량
	bool		is_AppQuit;	// 애플리케이션 종료알림 (스레드 종료용 변수)
public:
	bool		vSync;
	Color		background;
	int			fixFrame;
	float		deltaScale;
	bool		activeWindow;
	bool		is_game;	// 인게임 애플리케이션 확인 변수 (false: 어씸프컨버터, 트레인에디터 등, true: 게임)
public:
	Application() :instance(nullptr), handle(nullptr), vSync(true), fullScreen(false), activeWindow(true), is_AppQuit(false), is_game(false)
		, appName(L" "), width(1280.0f), height(720.0f), background(Color(0.0f, 0.0f, 0.0f, 1.0f))
		, fixFrame(60), deltaScale(1.0f), x(0.0f), y(0.0f) {}
	float	GetWidth() { return width; }
	float	GetHeight() { return height; }
	float	GetHalfWidth() { return width / 2.0f; }
	float	GetHalfHeight() { return height / 2.0f; }
	bool	GetFullScreen() { return fullScreen; }
	bool	GetAppQuit() { return is_AppQuit; }
	void	SetInstance(HINSTANCE instance)
	{ 
		if(!this->instance)
		this->instance = instance;
	}
	void	SetAppName(wstring appName) 
	{ 
		if (this->appName == L" ")
		this->appName = appName; 
	}
	//해상도변경은 D3D의 리사이즈 함수 이용!
};

template <typename T>
class Singleton
{
	//static
private:
	static T* instance;
public:
	static bool isHaveInstance()
	{
		return instance;
	}
	static T* GetInstance()
	{
		if (!instance)
			instance = new T;

		return instance;
	};
	//member
public:
	virtual ~Singleton() {};
	void DeleteSingleton()
	{
		delete instance;
		instance = nullptr;
	};
};
template <typename T>
T* Singleton<T>::instance = nullptr;

enum class DeviceState
{
	KEYBOARD,
	CONTROLLER
};

//씬타입으로는 객체를만들수 없다.

enum class SceneState
{
	NONE,
	FADEIN,
	FADEOUT
};
class Scene
{
public:
	vector<class Terrain*> terrainMap;
	SceneState	sceneState = SceneState::NONE;
	float		sceneTime;
	virtual ~Scene() {};
	//초기화
	virtual void Init() = 0;
	//해제
	virtual void Release() = 0;
	//Loop
	virtual void Update() = 0;
	virtual void LateUpdate() = 0;
	virtual void DebugRender() = 0;
	virtual void PreRender() = 0;
	virtual void Render() = 0;
	//Window Resize
	virtual void ResizeScreen() = 0;
};


struct Int2
{
	int x;
	int y;
	Int2()
	{
		x = 0;
		y = 0;
	}
	Int2(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
	bool operator ==(Int2 dest)
	{
		return (x == dest.x && y == dest.y);
	}
	bool operator !=(Int2 dest)
	{
		return (x != dest.x || y != dest.y);
		//return !(*this == dest);
	}
};
