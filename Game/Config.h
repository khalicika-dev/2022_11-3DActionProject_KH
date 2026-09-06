#pragma once
class ConfigManager :public Singleton<ConfigManager>
{
public:
	vector<pair<Window::EnumScreenSettings, const char*>> screenSettingVector;
	vector<pair<Window::EnumResolutionSettings, Int2>> resolutionSettingVector;

	struct ShadowSettingStruct
	{
		float textureSize;
		float range;
		bool quality;
		ShadowSettingStruct() {}
		ShadowSettingStruct(float textureSize, float range, bool quality) : textureSize(textureSize), range(range), quality(quality) {}
	};
	vector<pair<const char*, ShadowSettingStruct>> shadowSettingVector;

	struct LockOnSpeedSettingStruct
	{
		float distMin;	// 속도가 붙기 시작하는 최소거리
		float distMax;	// 속도가 최고점이 되는 최대거리
		LockOnSpeedSettingStruct() {}
		LockOnSpeedSettingStruct(float  distMin, float distMax) : distMin(distMin), distMax(distMax) {}
	};
	vector<pair<const char*, LockOnSpeedSettingStruct>> lockOnSpeedSettingVector;

	vector<const char*> zoomSettingVector;

	struct CameraSettingsStruct
	{
		const char* ImGuiName;	// ImGui(일시정지 메뉴)에서 사용되는 이름, 아이디
		string iniText;			// config.ini에 저장되는 옵션명
		vector<pair<const char*, float>> speedSettingVector;
		CameraSettingsStruct() {}
		CameraSettingsStruct(const char* name, string str) :ImGuiName(name), iniText(str) {}
	};
	map<DeviceState, CameraSettingsStruct> cameraSettings;

	struct ConfigSettingsStruct
	{
		int idx_screenSetting;
		int idx_resolutionSetting;
		bool* b_vSync;	// 프레임워크 변수와 연동하기 위한 포인터
		int idx_shadowSetting;

		int volume_master;
		int volume_bgm;
		int volume_se;
		int volume_env;

		bool b_lockOnParts;		// 몬스터를 록온할때 파츠별로/중심만
		int idx_lockOnSpeedSetting;

		int Idx_zoomSetting;
		map<DeviceState, int> Idx_speedSetting;
		map<DeviceState, bool> b_reverseDirHorizontal;
		map<DeviceState, bool> b_reverseDirVertical;

		bool* b_enableVibration;
		bool* b_showColliderComponents;
		bool b_HUDVisible = true;
	}settings;
public:
	~ConfigManager();
	void Init();
	void Reset(ConfigSettingsStruct& settings);
	void Load();	// Config정보를 파일에서 가져와, 변수들에 적용합니다.
	void Save();

	// 0~1 
	// 결과값이 1에 가까울수록 카메라가 멀다
	// 결과값이 0에 가까울수록 카메라가 가깝다
	float GetZoom() { return (float)settings.Idx_zoomSetting / (float)(zoomSettingVector.size() - 1); }
private:
	enum class ConfigName
	{
		Config_Screen,
		Config_Resolution,
		Config_vSync,
		Config_Shadow,
		Config_LockOnParts,
		Config_LockOnSpeed,
		Config_CamZoom,
		Config_CamSpeed,
		Config_CamHorizontal,
		Config_CamVertical,
		Config_VolumeMaster,
		Config_VolumeBGM,
		Config_VolumeSE,
		Config_VolumeENV,
		Config_ControllerVib,
	};
	map<ConfigName, string> configStr;
};