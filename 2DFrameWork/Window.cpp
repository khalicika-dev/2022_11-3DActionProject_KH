#include "framework.h"

Application	 App;
Scene* Window::main = nullptr;

WPARAM Window::Run(Scene* main)
{
	Window::main = main;
	windowSignal = make_pair(false, EnumScreenSettings::ScreenSettings_WINDOW);
	resolutionSignal = make_pair(false, Int2(App.width, App.height));
	BLEND->Set(false);
	MSG msg = { 0 };
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			TIMER->Chronometry(App.fixFrame);
			INPUT->Update();
			GUI->Update();
			GUI->DebugRender();

			main->Update();
			main->LateUpdate();

			SOUND->Update();
			SOUND->DebugRender();

			main->DebugRender();
			main->PreRender();

			D3D->SetRenderTarget();
			DWRITE->GetDC()->BeginDraw();
			D3D->Clear(App.background);
			{
				
				main->Render();
				GUI->Render();
			}
			DWRITE->GetDC()->EndDraw();

			if (windowSignal.first)
			{
				switch (windowSignal.second)
				{
				case EnumScreenSettings::ScreenSettings_WINDOW:
					App.fullScreen = false;
					break;
				case EnumScreenSettings::ScreenSettings_FULLSCREEN:
					App.x = App.y = 0;
					App.width = GetSystemMetrics(SM_CXSCREEN);
					App.height = GetSystemMetrics(SM_CYSCREEN);
					Resize(App.x, App.y, App.width, App.height);
					App.fullScreen = true;
					break;
				default:
					assert(false);
				}
				windowSignal.first = false;
			}
			if (D3D->GetSwapChain() && App.activeWindow)
			{
				BOOL temp;
				D3D->GetSwapChain()->GetFullscreenState(&temp, NULL);

				if (temp != (int)App.fullScreen)
				{
					D3D->GetSwapChain()->SetFullscreenState(App.fullScreen, NULL);
					//if (App.fullScreen == true)
					//{
					//	DEVMODE devMode = { 0 };
					//	devMode.dmSize = sizeof(DEVMODE);
					//	devMode.dmPelsWidth = (DWORD)App.width;
					//	devMode.dmPelsHeight = (DWORD)App.height;
					//	devMode.dmBitsPerPel = 32;
					//	devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
					//
					//	ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
					//}
					//else
					//	ChangeDisplaySettings(NULL, 0);
				}
			}
			if (resolutionSignal.first)
			{
				if (!App.fullScreen)
				{
					App.width = resolutionSignal.second.x;
					App.height = resolutionSignal.second.y;
					App.x = (GetSystemMetrics(SM_CXSCREEN) - (UINT)App.GetWidth()) / 2;
					App.y = (GetSystemMetrics(SM_CYSCREEN) - (UINT)App.GetHeight()) / 2;
					Resize(App.x, App.y, App.width, App.height);
				}
				resolutionSignal.first = false;
			}
			D3D->Present();
		}
	}
	//WIN->Save();
	Destroy();

	return msg.wParam;
}

void Window::Load()
{
	ifstream fin;
	string file = "window.ini";
	fin.open(file.c_str(), ios::in);
	string temp;
	if (fin.is_open())
	{
		string temp;
		fin >> temp >> App.width >> App.height;
		fin >> temp >> App.x >> App.y;
		fin.close();
	}

	RECT rect = { 0, 0, (LONG)App.GetWidth(), (LONG)App.GetHeight() };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	MoveWindow
	(
		App.handle
		, (LONG)App.x, (LONG)App.y
		, rect.right - rect.left, rect.bottom - rect.top
		, TRUE
	);
}

void Window::Save()
{
	RECT rc;
	GetWindowRect(App.handle, &rc);
	App.x = (float)rc.left;
	App.y = (float)rc.top;
	ofstream fout;
	string file = "window.ini";
	fout.open(file.c_str(), ios::out);
	if (fout.is_open())
	{
		fout << "Size " << App.width << " " << App.height << endl;
		fout << "Pos " << App.x << " " << App.y << endl;
		fout.close();
	}
}

void Window::Resize(float x, float y, float w, float h)
{
	RECT rect = { 0, 0, (LONG)App.GetWidth(), (LONG)App.GetHeight() };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	MoveWindow
	(
		App.handle
		, (LONG)App.x, (LONG)App.y
		, rect.right - rect.left, rect.bottom - rect.top
		, TRUE
	);
}

void Window::Create()
{

	WNDCLASSEXW wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hIconSm = wndClass.hIcon;
	wndClass.hInstance = App.instance;
	wndClass.lpfnWndProc = (WNDPROC)WndProc;
	wndClass.lpszClassName = App.appName.c_str();
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.cbSize = sizeof(WNDCLASSEX);

	WORD wHr = RegisterClassExW(&wndClass);
	assert(wHr != 0);

	//if (App.fullScreen == true)
	//{
	//	DEVMODE devMode = { 0 };
	//	devMode.dmSize = sizeof(DEVMODE);
	//	devMode.dmPelsWidth = (DWORD)App.GetWidth();
	//	devMode.dmPelsHeight = (DWORD)App.GetHeight();
	//	devMode.dmBitsPerPel = 32;
	//	devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	//
	//	ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
	//}
	DWORD winStyle = App.is_game ? (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX) : WS_OVERLAPPEDWINDOW;
	App.handle = CreateWindowExW
	(
		WS_EX_APPWINDOW
		, App.appName.c_str()
		, App.appName.c_str()
		, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | winStyle
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, NULL
		, (HMENU)NULL
		, App.instance
		, NULL
	);
	assert(App.handle != NULL);
	
	ShowWindow(App.handle, SW_SHOWNORMAL);
	SetForegroundWindow(App.handle);
	SetFocus(App.handle);

	ShowCursor(true);
	//WIN->Load();
	Resize(App.x, App.y, App.width, App.height);
}

void Window::Destroy()
{

	if (App.fullScreen == true)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	DestroyWindow(App.handle);

	UnregisterClass(App.appName.c_str(), App.instance);
}

void Window::ClipToWindow(HWND handle) noexcept
{
	if (!Input::isHaveInstance() || INPUT->gameMouse->GetState().positionMode == Mouse::MODE_ABSOLUTE)
		return;
	RECT rect = {};
	std::ignore = GetClientRect(handle, &rect);

	POINT ul;
	ul.x = rect.left;
	ul.y = rect.top;

	POINT lr;
	lr.x = rect.right;
	lr.y = rect.bottom;

	std::ignore = MapWindowPoints(handle, nullptr, &ul, 1);
	std::ignore = MapWindowPoints(handle, nullptr, &lr, 1);

	rect.left = ul.x;
	rect.top = ul.y;

	rect.right = lr.x;
	rect.bottom = lr.y;

	ClipCursor(&rect);
}

LRESULT Window::WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	INPUT->InputProc(handle, message, wParam, lParam);

	if (Gui::MsgProc(handle, message, wParam, lParam))
		return true;

	if (message == WM_SIZE)
	{
		if (D3D->GetCreated())
		{
			float width = (float)LOWORD(lParam);
			float height = (float)HIWORD(lParam);
			D3D->ResizeScreen(width, height);
			GUI->ResizeScreen();
			if (main)
				main->ResizeScreen();
			ClipToWindow(handle);
		}
	}
	if (message == WM_SYSCOMMAND)
	{
		if (wParam == SC_KEYMENU)
			return 0;
	}
	if (message == WM_ACTIVATE)
	{
		App.activeWindow = LOWORD(wParam);
	}
	if (message == WM_CLOSE || message == WM_DESTROY)
	{
		App.is_AppQuit = true;
		if (SceneManager::isHaveInstance() && SCENE->loadingSC.th_loading && SCENE->loadingSC.th_loading->joinable())
		{
			SCENE->loadingSC.th_loading->join();
			SafeDelete(SCENE->loadingSC.th_loading);
		}
		PostQuitMessage(0);

		return 0;
	}

	return DefWindowProc(handle, message, wParam, lParam);
}

