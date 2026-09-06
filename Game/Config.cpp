#include "stdafx.h"

ConfigManager::~ConfigManager()
{
}

void ConfigManager::Init()
{
	// 스크린설정 초기화
	configStr[ConfigName::Config_Screen] = "ScreenMode";
	screenSettingVector.clear();
	screenSettingVector.push_back(make_pair(Window::ScreenSettings_FULLSCREEN, u8"켜기"));
	screenSettingVector.push_back(make_pair(Window::ScreenSettings_WINDOW, u8"끄기"));
	assert(screenSettingVector.size() >= Window::ScreenSettings_ALL_COUNTS);

	// 해상도 초기화 및 앱 사이즈 초기화
	configStr[ConfigName::Config_Resolution] = "Resolution";
	resolutionSettingVector.clear();
	resolutionSettingVector.push_back(make_pair(Window::ResolutionSettings_DEFAULT, Int2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN))));
	resolutionSettingVector.push_back(make_pair(Window::ResolutionSettings_800x600, Int2(800, 600)));
	resolutionSettingVector.push_back(make_pair(Window::ResolutionSettings_1024x768, Int2(1024, 768)));
	resolutionSettingVector.push_back(make_pair(Window::ResolutionSettings_1152x864, Int2(1152, 864)));
	resolutionSettingVector.push_back(make_pair(Window::ResolutionSettings_1280x720, Int2(1280, 720)));
	resolutionSettingVector.push_back(make_pair(Window::ResolutionSettings_1360x768, Int2(1360, 768)));
	resolutionSettingVector.push_back(make_pair(Window::ResolutionSettings_1366x768, Int2(1366, 768)));
	resolutionSettingVector.push_back(make_pair(Window::ResolutionSettings_1600x900, Int2(1600, 900)));
	resolutionSettingVector.push_back(make_pair(Window::ResolutionSettings_1920x1080, Int2(1920, 1080)));
	assert(resolutionSettingVector.size() >= Window::ResolutionSettings_ALL_COUNTS);
	for (int i = 0; i < resolutionSettingVector.size(); i++)
	{
		while (resolutionSettingVector[i].first != Window::ResolutionSettings_DEFAULT && (
			(resolutionSettingVector[i].second.x == resolutionSettingVector[0].second.x && resolutionSettingVector[i].second.y == resolutionSettingVector[0].second.y) ||
			(resolutionSettingVector[i].second.x > resolutionSettingVector[0].second.x || resolutionSettingVector[i].second.y > resolutionSettingVector[0].second.y)))
		{
			resolutionSettingVector.erase(resolutionSettingVector.begin() + i);
			i--;
		}
	}

	// 수직동기화 Config명 정의
	settings.b_vSync = &App.vSync;
	configStr[ConfigName::Config_vSync] = "vSync";

	// 그림자품질 초기화
	configStr[ConfigName::Config_Shadow] = "ShadowLevel";
	shadowSettingVector.clear();
	shadowSettingVector.push_back(make_pair(u8"없음", ShadowSettingStruct(0.0f, 0.0f, false)));
	shadowSettingVector.push_back(make_pair(u8"낮음", ShadowSettingStruct(5000.0f, 500.0f, false)));
	shadowSettingVector.push_back(make_pair(u8"중간", ShadowSettingStruct(5000.0f, 500.0f, true)));
	shadowSettingVector.push_back(make_pair(u8"높음", ShadowSettingStruct(10000.0f, 500.0f, true)));

	// 사운드
	configStr[ConfigName::Config_VolumeMaster] = "Volume_Master";
	configStr[ConfigName::Config_VolumeBGM] = "Volume_BGM";
	configStr[ConfigName::Config_VolumeSE] = "Volume_SE";
	configStr[ConfigName::Config_VolumeENV] = "Volume_ENV";

	// 록온 파츠별 조준여부
	configStr[ConfigName::Config_LockOnParts] = "LockOnToParts";

	// 록온 속도
	configStr[ConfigName::Config_LockOnSpeed] = "LockOnSpeedLevel";
	lockOnSpeedSettingVector.clear();
	lockOnSpeedSettingVector.push_back(make_pair(u8"보통", LockOnSpeedSettingStruct(3.5f, 10.0f)));
	lockOnSpeedSettingVector.push_back(make_pair(u8"약간 빠름", LockOnSpeedSettingStruct(0.0f, 3.25f)));
	lockOnSpeedSettingVector.push_back(make_pair(u8"빠름", LockOnSpeedSettingStruct(0.0f, 0.0f)));

	// 카메라 거리 (1 ~ 5)
	configStr[ConfigName::Config_CamZoom] = "CameraZoomLevel";
	zoomSettingVector.clear();
	zoomSettingVector.push_back(u8"가까움");	// 인덱스가 낮을수록 가깝다
	zoomSettingVector.push_back(u8"조금 가까움");
	zoomSettingVector.push_back(u8"표준");
	zoomSettingVector.push_back(u8"조금 멀리");
	zoomSettingVector.push_back(u8"멀리");		// 인덱스가 높을수록 멀다

	// 카메라 설정(마우스, 패드)
	cameraSettings[DeviceState::KEYBOARD] = CameraSettingsStruct(u8"마우스", "Keyboard");
	cameraSettings[DeviceState::CONTROLLER] = CameraSettingsStruct(u8"컨트롤러", "Controller");
	for (auto it = cameraSettings.begin(); it != cameraSettings.end(); it++)
	{
		// 카메라 속도
		configStr[ConfigName::Config_CamSpeed] = "CameraSpeedLevel";
		it->second.speedSettingVector.clear();
		it->second.speedSettingVector.push_back(make_pair(u8"느림", 1.0f));
		it->second.speedSettingVector.push_back(make_pair(u8"조금 느림", 1.5f));
		it->second.speedSettingVector.push_back(make_pair(u8"표준", 2.0f));
		it->second.speedSettingVector.push_back(make_pair(u8"조금 빠름", 2.5f));
		it->second.speedSettingVector.push_back(make_pair(u8"빠름", 3.0f));

		// 수직/수평 이동방향
		configStr[ConfigName::Config_CamHorizontal] = "CameraHorizontal_Reverse";
		configStr[ConfigName::Config_CamVertical] = "CameraVertical_Reverse";
	}

	// 컨트롤러진동 Config명 정의
	settings.b_enableVibration = &VAR->enableVibration;
	configStr[ConfigName::Config_ControllerVib] = "ControllerVibration";

	// 콜라이더표시 주소정의
	settings.b_showColliderComponents = &VAR->showColliderComponents;

	Reset(settings);
}

void ConfigManager::Reset(ConfigSettingsStruct& settings)
{
	// Config 초기값으로 리셋
	settings.idx_screenSetting = 0; // 풀스크린
	settings.idx_resolutionSetting = 0;
	*settings.b_vSync = true;
	settings.idx_shadowSetting = shadowSettingVector.size() - 1;
	settings.b_lockOnParts = true;
	settings.idx_lockOnSpeedSetting = 1;
	settings.Idx_zoomSetting = zoomSettingVector.size() - 1;
	for (auto it = cameraSettings.begin(); it != cameraSettings.end(); it++)
	{
		settings.Idx_speedSetting[it->first] = 2;
		settings.b_reverseDirHorizontal[it->first] = false;
		settings.b_reverseDirVertical[it->first] = false;
	}
	settings.volume_master = settings.volume_bgm = settings.volume_se = settings.volume_env = 100;
	*settings.b_enableVibration = true;
	*settings.b_showColliderComponents = false;
	settings.b_HUDVisible = true;
}

void ConfigManager::Load()
{
	ifstream fin;
	string file = "config.ini";
	fin.open(file.c_str(), ios::in);
	string temp;
	if (fin.is_open())
	{
		// 읽어들인 첫 줄마다 해당되는 문장을 찾아서 문장에 대응하는 값을 적용한다.
		string temp;
		while (!fin.eof())
		{
			fin >> temp;

			size_t strOffset = temp.find('=');
			if (strOffset == string::npos)
				continue;

			size_t strOffset2 = temp.find('[');
			size_t strOffset2_len = temp.find(']') - strOffset2 - 1;
			string cf_name = temp.substr(0, (strOffset2 == string::npos) ? strOffset : strOffset2);	// 읽은 Config옵션의 이름
			string cf_deviceName = (strOffset2 == string::npos) ? "None" : temp.substr(strOffset2 + 1, strOffset2_len);	// 읽은 Config옵션의 기기타입(string)
			DeviceState cf_device;	// cf_deviceName에 해당하는 Enum
			if (cf_deviceName == cameraSettings[DeviceState::KEYBOARD].iniText) cf_device = DeviceState::KEYBOARD;
			else if (cf_deviceName == cameraSettings[DeviceState::CONTROLLER].iniText) cf_device = DeviceState::CONTROLLER;

			string cf_value = temp.substr(strOffset + 1);	// 읽은 Config옵션의 값
			if (cf_name == configStr[ConfigName::Config_Screen])
			{
				if (cf_value == "Fullscreen") settings.idx_screenSetting = 0;
				else if (cf_value == "Window") settings.idx_screenSetting = 1;
			}
			else if (cf_name == configStr[ConfigName::Config_Resolution])
			{
				int w = stoi(cf_value.substr(0,cf_value.find('x')));
				int h = stoi(cf_value.substr(cf_value.find('x') + 1));
				for (int i = 0; i < resolutionSettingVector.size(); i++)
				{
					if (w == resolutionSettingVector[i].second.x && h == resolutionSettingVector[i].second.y)
					{
						settings.idx_resolutionSetting = i;
						break;
					}
				}
				App.width = resolutionSettingVector[settings.idx_resolutionSetting].second.x;
				App.height = resolutionSettingVector[settings.idx_resolutionSetting].second.y;
			}
			else if (cf_name == configStr[ConfigName::Config_vSync])
				*settings.b_vSync = cf_value == "true" ? true : false;
			else if (cf_name == configStr[ConfigName::Config_Shadow])
				settings.idx_shadowSetting = stoi(cf_value);
			else if (cf_name == configStr[ConfigName::Config_VolumeMaster])
				settings.volume_master = stoi(cf_value);
			else if (cf_name == configStr[ConfigName::Config_VolumeBGM])
				settings.volume_bgm = stoi(cf_value);
			else if (cf_name == configStr[ConfigName::Config_VolumeSE])
				settings.volume_se = stoi(cf_value);
			else if (cf_name == configStr[ConfigName::Config_VolumeENV])
				settings.volume_env = stoi(cf_value);
			else if (cf_name == configStr[ConfigName::Config_LockOnParts])
				settings.b_lockOnParts = cf_value == "true" ? true : false;
			else if (cf_name == configStr[ConfigName::Config_LockOnSpeed])
				settings.idx_lockOnSpeedSetting = stoi(cf_value);
			else if (cf_name == configStr[ConfigName::Config_CamZoom])
				settings.Idx_zoomSetting = stoi(cf_value);
			else if (cf_name == configStr[ConfigName::Config_CamSpeed] && strOffset2 != string::npos)
				settings.Idx_speedSetting[cf_device] = stoi(cf_value);
			else if (cf_name == configStr[ConfigName::Config_CamHorizontal] && strOffset2 != string::npos)
				settings.b_reverseDirHorizontal[cf_device] = cf_value == "true" ? true : false;
			else if (cf_name == configStr[ConfigName::Config_CamVertical] && strOffset2 != string::npos)
				settings.b_reverseDirVertical[cf_device] = cf_value == "true" ? true : false;
			else if (cf_name == configStr[ConfigName::Config_ControllerVib])
				*settings.b_enableVibration = cf_value == "true" ? true : false;
		}
		//fin >> temp >> App.width >> App.height;
		//fin >> temp >> App.x >> App.y;
		fin.close();
		//configOptions[ConfigName::Config_Screen].address = &b_lockOnParts;
	}

	App.fullScreen = (screenSettingVector[settings.idx_screenSetting].first == Window::ScreenSettings_FULLSCREEN);
	App.width = resolutionSettingVector[settings.idx_resolutionSetting].second.x;
	App.height = resolutionSettingVector[settings.idx_resolutionSetting].second.y;
	App.x = (GetSystemMetrics(SM_CXSCREEN) - (UINT)App.GetWidth()) / 2;
	App.y = (GetSystemMetrics(SM_CYSCREEN) - (UINT)App.GetHeight()) / 2;
	//RECT rect = { 0, 0, (LONG)App.GetWidth(), (LONG)App.GetHeight() };
	//AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	//MoveWindow
	//(
	//	App.handle
	//	, (LONG)App.x, (LONG)App.y
	//	, rect.right - rect.left, rect.bottom - rect.top
	//	, TRUE
	//);
}

void ConfigManager::Save()
{
	RECT rc;
	GetWindowRect(App.handle, &rc);
	App.x = (float)rc.left;
	App.y = (float)rc.top;
	ofstream fout;
	string file = "config.ini";
	fout.open(file.c_str(), ios::out);
	if (fout.is_open())
	{
		#define SAVE_MACRO_STR(_CONFIGTYPE) configStr[_CONFIGTYPE] << "="
		#define SAVE_MACRO_STR_WITHDEVICE(_CONFIGTYPE, _DEVICETYPE) configStr[_CONFIGTYPE] << "[" << _DEVICETYPE << "]" << "="
		string str;

		fout << "--Graphic--" << endl;
		switch (settings.idx_screenSetting)
		{
		case 0: str = "Fullscreen"; break;
		case 1: str = "Window"; break;
		default: assert(false);
		}
		fout << SAVE_MACRO_STR(ConfigName::Config_Screen) << str << endl;
		fout << SAVE_MACRO_STR(ConfigName::Config_Resolution) << (int)App.GetWidth() << "x" << (int)App.GetHeight() << endl;
		fout << SAVE_MACRO_STR(ConfigName::Config_vSync) << (*settings.b_vSync ? "true" : "false") << endl;
		fout << SAVE_MACRO_STR(ConfigName::Config_Shadow) << settings.idx_shadowSetting << endl;
		fout << endl;

		fout << "--Sound--" << endl;
		fout << SAVE_MACRO_STR(ConfigName::Config_VolumeMaster) << settings.volume_master << endl;
		fout << SAVE_MACRO_STR(ConfigName::Config_VolumeBGM) << settings.volume_bgm << endl;
		fout << SAVE_MACRO_STR(ConfigName::Config_VolumeSE) << settings.volume_se << endl;
		fout << SAVE_MACRO_STR(ConfigName::Config_VolumeENV) << settings.volume_env << endl;
		fout << endl;

		fout << "--Action--" << endl;
		fout << SAVE_MACRO_STR(ConfigName::Config_LockOnParts) << (settings.b_lockOnParts ? "true" : "false") << endl;
		fout << SAVE_MACRO_STR(ConfigName::Config_LockOnSpeed) << settings.idx_lockOnSpeedSetting << endl;
		fout << endl;

		fout << "--Camera--" << endl;
		fout << SAVE_MACRO_STR(ConfigName::Config_CamZoom) << settings.Idx_zoomSetting << endl;
		for (auto it = cameraSettings.begin(); it != cameraSettings.end(); it++)
		{
			fout << SAVE_MACRO_STR_WITHDEVICE(ConfigName::Config_CamSpeed, it->second.iniText) << settings.Idx_speedSetting[it->first] << endl;
			fout << SAVE_MACRO_STR_WITHDEVICE(ConfigName::Config_CamHorizontal, it->second.iniText) << (settings.b_reverseDirHorizontal[it->first] ? "true" : "false") << endl;
			fout << SAVE_MACRO_STR_WITHDEVICE(ConfigName::Config_CamVertical, it->second.iniText) << (settings.b_reverseDirVertical[it->first] ? "true" : "false") << endl;
		}
		fout << endl;

		fout << "--Misc--" << endl;
		fout << SAVE_MACRO_STR(ConfigName::Config_ControllerVib) << (*settings.b_enableVibration ? "true" : "false");
		fout.close();

		#undef SAVE_MACRO_STR_WITHDEVICE
		#undef SAVE_MACRO_STR
	}
}