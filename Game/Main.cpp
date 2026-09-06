#include "stdafx.h"
#include "Main.h"

Main::Main()
{
}
Main::~Main()
{
}

void Main::Init()
{
	pMenu = new PauseMenu();

	// 프로그램을 구성하는 필수요소부터 정의
	Transform::CreateStaticMember();
	Camera::CreateStaticMember();
	Material::CreateStaticMember();
	PostEffect::CreateStaticMember();
	INPUT->SetMouseMode(Mouse::Mode::MODE_RELATIVE);

	// 로딩Scene 전환
	SCENE->AddScene(SCENEKEY::LOADING, new LoadingScene());
	SCENE->SetLoadingScene(LOADINGTYPE::COMPONENTS,
		[]()
		{
			// 이후 로드할 요소의 용량 계산
			Object::CalculateStaticMember();
			Terrain::CalculateStaticMember();
			ItemManager::CalculateStaticMember();
			DmgNumber::CalculateStaticMember();
			InfoMsg::CalculateStaticMember();
			SOUND->Init(Sound::SoundInitMode::CALCULATE);
			GAMEOBJ->CalculatePool();

			GAMEOBJ->Calculate(GObjectManager::GOType::PLAYER, 0);
			PLAYER_UI->Calculate();
		},
		[]()
		{
			// 컴포넌트 로드
			Object::CreateStaticMember();
			Skeleton::CreateStaticMember();
			Terrain::CreateStaticMember();
			Environment::CreateStaticMember();
			Rain::CreateStaticMember();
			Pop::CreateStaticMember();
			Shadow::CreateStaticMember();
			ItemManager::CreateStaticMember();
			DmgNumber::CreateStaticMember();
			InfoMsg::CreateStaticMember();
			SCENE->shadow->SetProperty(
				CONFIG->shadowSettingVector[CONFIG->settings.idx_shadowSetting].second.textureSize,
				CONFIG->shadowSettingVector[CONFIG->settings.idx_shadowSetting].second.range,
				CONFIG->shadowSettingVector[CONFIG->settings.idx_shadowSetting].second.quality);
			SOUND->Init(Sound::SoundInitMode::ADD);
			GAMEOBJ->InitPool();

			// 플레이어 로드
			GAMEOBJ->Prepare(GObjectManager::GOType::PLAYER, 0);
			PLAYER_UI->Init();

			if(GET_PLAYER)
				Camera::SetMainCam(GET_PLAYER->GetCamera());
		});
}

void Main::Release()
{
	SafeDelete(pMenu);
	if(ConfigManager::isHaveInstance()) CONFIG->DeleteSingleton();
	if(Player_Interface::isHaveInstance()) PLAYER_UI->DeleteSingleton();
	if(GObjectManager::isHaveInstance()) GAMEOBJ->DeleteSingleton();
	if(ResourceManager::isHaveInstance()) RESOURCE->ReleaseAll();
	ItemManager::DeleteStaticMember();
	DmgNumber::DeleteStaticMember();
	InfoMsg::DeleteStaticMember();
	
	Transform::DeleteStaticMember();
	Object::DeleteStaticMember();
	Camera::DeleteStaticMember();
	Material::DeleteStaticMember();
	Skeleton::DeleteStaticMember();
	Terrain::DeleteStaticMember();
	Environment::DeleteStaticMember();
	Rain::DeleteStaticMember();
	Pop::DeleteStaticMember();
	Shadow::DeleteStaticMember();
	PostEffect::DeleteStaticMember();

	TIMER->DeleteSingleton();
	INPUT->DeleteSingleton();
	GUI->DeleteSingleton();
	RANDOM->DeleteSingleton();
	DEPTH->DeleteSingleton();
	DWRITE->DeleteSingleton();
	SOUND->DeleteSingleton();
	SCENE->DeleteSingleton();
	RESOURCE->DeleteSingleton();
}


void Main::Update()
{
	if (isLoading() || SCENE->GetIsChaning())
	{
		VAR->isPause = false;
		SCENE->Update();
		return;
	}

	if (!VAR->isPause)
	{
		CameraControl::InitCamState();
		GAMEOBJ->ClearHitList();
		SCENE->Update();
		GAMEOBJ->Particles_Update();
	}
}

void Main::LateUpdate()
{
	if (isLoading() || SCENE->GetIsChaning())
	{
		SCENE->LateUpdate();
		pMenu->RenderCursor();
		return;
	}

	bool b_pauseAvailable = true;
	
	if (!VAR->isPause)
	{
		// 장면 업데이트
		SCENE->LateUpdate();
		GAMEOBJ->Particles_LateUpdate();

		// 카메라 시점 업데이트
		switch (CameraControl::GetCamState())
		{
		case Camera::CameraState::PLAYER:
			if (GET_PLAYER && Camera::GetMainCam() != GET_PLAYER->GetCamera())
				Camera::SetMainCam(GET_PLAYER->GetCamera());
			if (Camera::GetFollowCam() && Camera::GetMainCam())
			{
				Camera::GetFollowCam()->FollowMainCam(15.0f, PI_2);
				Camera::GetFollowCam()->root->Update();
			}
			break;
		case Camera::CameraState::OTHER_EVENT:
			b_pauseAvailable = false;
			if (GAMEOBJ->eventTarget && GAMEOBJ->eventTarget->GetEventCamera() && Camera::GetMainCam() != GAMEOBJ->eventTarget->GetEventCamera())
				Camera::SetMainCam(GAMEOBJ->eventTarget->GetEventCamera());
			break;
		case Camera::CameraState::PLAYER_EVENT:
			if (!PLAYER_UI->IsGameEnding())
				b_pauseAvailable = false;
			if (GET_PLAYER && Camera::GetMainCam() != GET_PLAYER->GetEventCamera())
				Camera::SetMainCam(GET_PLAYER->GetEventCamera());
			break;
		}
	}
	else
	{
		if (PLAYER_UI)
			PLAYER_UI->Update();
	}

	// 일시정지 조작
	if (!VAR->isPause && PLAYER_UI->GetGameStart() > 0)
	{
		if (INPUT->KeyDown(VK_ESCAPE) && b_pauseAvailable)
			INPUT->TogglePuase();
	}
	else
	{
		if (INPUT->KeyDown(VK_ESCAPE, false) || INPUT->tracker.b == GamePad::ButtonStateTracker::ButtonState::PRESSED)
			pMenu->SendCloseSignal(false);
		else if (INPUT->KeyDown(VK_CONTROL, false) || INPUT->tracker.menu == GamePad::ButtonStateTracker::ButtonState::PRESSED)
			pMenu->SendCloseSignal(true);
	}

	pMenu->Update(); // 메뉴 업데이트
	pMenu->RenderCursor();

	// 사운드 볼륨 업데이트
	SOUND->soundScale_UserMaster = static_cast<float>(CONFIG->settings.volume_master) / 100.0f;
	SOUND->soundScale_User[SDTYPE::BGM] = static_cast<float>(CONFIG->settings.volume_bgm) / 100.0f;
	SOUND->soundScale_User[SDTYPE::SE] = static_cast<float>(CONFIG->settings.volume_se) / 100.0f;
	SOUND->soundScale_User[SDTYPE::ENV] = static_cast<float>(CONFIG->settings.volume_env) / 100.0f;

	// 사운드 시점 업데이트
	if (SCENE->currentCam)
	{
		SOUND->listenerForward = SCENE->currentCam->GetForward();
		SOUND->listenerUp = SCENE->currentCam->GetUp();
		switch (CameraControl::GetCamState())
		{
		case Camera::CameraState::PLAYER:	// 플레이어 캐릭터를 기준으로 청취
			SOUND->listenerPos = (GET_PLAYER && SCENE->currentCam == Camera::GetFollowCam()) ? GET_PLAYER->GetActor()->GetWorldPos() : SOUND->listenerPos = SCENE->currentCam->GetWorldPos();
			break;
		default:	// 그 외에는 사용중인 카메라를 기준으로 청취
			SOUND->listenerPos = SCENE->currentCam->GetWorldPos();
		}
	}
	else
	{
		SOUND->listenerPos = { 0,0,0 };
		SOUND->listenerForward = { 0,0,1 };
		SOUND->listenerUp = { 0,1,0 };
	}
	SOUND->SetListenerAttr();	// 사용중인 카메라의 Forward, Up 기준으로 LookAt 설정
}

void Main::DebugRender()
{
	if (!VAR->isDebug) return;

	// 세부 디버깅
	SCENE->DebugRender();
}

void Main::Render()
{
	SCENE->Render();

	if (!isLoading() && VAR->isPause)
	{
		wstring text = L"Pause";
		float size = 35.0f;
		RECT rc{ App.GetWidth() - (5.0f + size * text.length()), 3.0f, App.GetWidth() - 5.0f, 3.0f };
		DWRITE->RenderText(text, rc, size,
			L"Verdana", Color(1, 0, 0), DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_ITALIC);
	}
	if (!isLoading() && PLAYER_UI->GetGameStart() == Player_Interface::STATE_TITLE)
	{
		wstring text = L"2024년 포트폴리오 제작자: 김환";
		float size = 30.0f;
		RECT rc{ 5.0f, App.GetHeight() - 3.0f, 5.0f + size * text.length(), App.GetHeight() - 3.0f };
		DWRITE->RenderText(text, rc, size,
			L"바탕", Color(0, 0, 0), DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_FAR, DWRITE_FONT_WEIGHT_BOLD);
	}
}

void Main::ResizeScreen()
{
	SCENE->ResizeScreen();
}

bool Main::isLoading()
{
	return SCENE->GetCurrentScene() == SCENE->GetScene(SCENEKEY::LOADING);
}

void Main::PreRender()
{
	if (isLoading() || SCENE->GetIsChaning())
	{
		SCENE->PreRender();
		return;
	}

	switch (CameraControl::GetCamState())
	{
	case Camera::CameraState::PLAYER: SCENE->currentCam = Camera::GetFollowCam(); break;
	default: SCENE->currentCam = Camera::GetMainCam();
	}
	SCENE->PreRender();
	GAMEOBJ->Particles_Render();
	GAMEOBJ->Trails_Render();
}


int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR param, int command)
{
	App.SetAppName(L"3DActionProject");
	App.SetInstance(instance);
	App.is_game = true;
	CONFIG->Init();
	CONFIG->Load();
	WIN->Create();
	D3D->Create();
	Main* main = new Main();
	main->Init();
	int wParam = (int)WIN->Run(main);
	CONFIG->Save();
	main->Release();
	SafeDelete(main);
	D3D->DeleteSingleton();
	WIN->DeleteSingleton();
	return wParam;
}
