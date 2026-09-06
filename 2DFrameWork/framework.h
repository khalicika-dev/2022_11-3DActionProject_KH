#pragma once
//Popup Console : 콘솔 팝업시키기
//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 

//C
#include <Windows.h>
#include <assert.h>

//C++
#include <string>
#include <vector>
#include <deque>
#include <chrono>
#include <random>
#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <functional>
using namespace std;

//DirectX d3d 11
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")

//D2D
#include <d2d1_1.h>
#pragma comment(lib, "d2d1.lib")

//Direct Write
#include <dwrite.h>
#pragma comment(lib, "dwrite.lib")

//DirectXTk 
#include "../Libs/DirectXTK/SimpleMath.h"
#include "../Libs/DirectXTK/GamePad.h"
#include "../Libs/DirectXTK/Mouse.h"
#pragma comment(lib, "../Libs/DirectXTK/DirectXTK.lib")
using namespace DirectX;
using namespace SimpleMath;

//DirectXTex
#include "../Libs/DirectXTex/DirectXTex.h"
#pragma comment(lib, "../Libs/DirectXTex/DirectXTex.lib")

//Imgui
#include "../Libs/ImGui/imgui.h"
#include "../Libs/ImGui/imgui_internal.h"
#include "../Libs/ImGui/imgui_impl_dx11.h"
#include "../Libs/ImGui/imgui_impl_win32.h"
#pragma comment(lib, "../Libs/ImGui/example_win32_directx11.lib")

//Dialog
#include "../Libs/ImGui/dirent.h"
#include "../Libs/ImGui/ImGuiFileDialog.h"
#include "../Libs/ImGui/ImGuiFileDialogConfig.h"


//Fmod
#include "../Libs/Fmod/fmod.hpp"
#pragma comment(lib, "../Libs/Fmod/fmodL_vc.lib")

//File
#include "BinaryFile.h"
#include "Xml.h"


//Macro(Pascal)
#define Check(hr)			{ assert(SUCCEEDED(hr)); }
#define SafeRelease(p)		{ if(p){ (p)->Release(); (p) = nullptr; } }
#define SafeDelete(p)		{ if(p){ delete (p); (p) = nullptr; } }
#define SafeDeleteArray(p)	{ if(p){ delete[] (p); (p) = nullptr; } }
#define SafeReset(p)		{ if(p){ (p).reset(); (p) = nullptr; } }
//Macro(WINAPI Style)
#define DEFAULT_RESOLUTION	Vector2(1920.0f,1080.0f)
#define	TORADIAN			0.0174532f
#define	PI_2				6.2831853f  //360
#define	PI					3.1415926f	//180
#define	PI_DIV2				1.5707963f	//90
#define	PI_DIV4				0.7853981f	//45
#define	PI_DIV8				0.3926991f	//22.5
#define	DIR_RIGHT			Vector3(1.0f,0.0f,0.0f)
#define	DIR_UP				Vector3(0.0f,1.0f,0.0f)
#define	DIR_FORWARD			Vector3(0.0f,0.0f,1.0f)

//2DFramework Header
#include "Types.h"
#include "Window.h"
extern Application App;//Extern Global
#include "Direct3D11.h"
#include "Gui.h"
#include "Input.h"
#include "Timer.h"
#include "Random.h"
#include "Sound.h"


#include "VertexType.h"
#include "Shader.h"
#include "Mesh.h"
#include "Transform.h"
#include "Texture.h"
#include "Material.h"
#include "Collider.h"
#include "Skeleton.h"
#include "Animation.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Dwrite.h"

#include "Object.h"
#include "Camera.h"
#include "Dijkstra.h"
#include "Terrain.h"
#include "Light.h"
#include "UI.h"
#include "Sky.h"
#include "Billboard.h"
#include "Rain.h"
#include "Pop.h"
#include "DepthState.h"
#include "BlendState.h"
#include "RasterState.h"
#include "RenderTarget.h"
#include "CubeRenderTarget.h"
#include "PostEffect.h"
#include "Environment.h"
#include "Shadow.h"

#include "Util.h"
#include "Variables.h"

//Singleton Macro
#define	D3D			Direct3D11::GetInstance()
#define	WIN			Window::GetInstance()
#define	GUI			Gui::GetInstance()
#define INPUT		Input::GetInstance()
#define TIMER		Timer::GetInstance()
#define DELTA		Timer::GetInstance()->GetDeltaScaleTime()
#define DELTA_NS	Timer::GetInstance()->GetDeltaTime()
#define DELTA_NPNS	Timer::GetInstance()->GetDeltaTimeNoScaleAndPause()
#define RANDOM		Random::GetInstance()
#define RESOURCE	ResourceManager::GetInstance()
#define DWRITE		Dwrite::GetInstance()
#define SCENE		SceneManager::GetInstance()
#define SCENEKEY	SceneManager::SceneKey
#define LOADINGTYPE	LoadingManager::LoadingType
#define DEPTH		DepthState::GetInstance()
#define BLEND		BlendState::GetInstance()
#define RASTER		RasterState::GetInstance()
#define SOUND		Sound::GetInstance()
#define LIGHT		LightManager::GetInstance()
#define SDKEY		Sound::SoundKey
#define SDTYPE		Sound::SoundType
#define VAR			Variables::GetInstance()