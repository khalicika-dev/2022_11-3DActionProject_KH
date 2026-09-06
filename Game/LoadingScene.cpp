#include "stdafx.h"

LoadingScene::LoadingScene()
{
	Cam = Camera::Create();
	Cam->LoadFile("Cam.xml");
	loadingImage = UI::Create("loadingImage");
	loadingImage->CreateMesh("title.png");
	curLoadingCount = 0;
	maxLoadingCount = 0;
}

LoadingScene::~LoadingScene()
{
	Release();
}

void LoadingScene::Init()
{
	INPUT->SetMouseMode(Mouse::MODE_RELATIVE);
	ResizeScreen();
}

void LoadingScene::Release()
{
}

void LoadingScene::Update()
{
	Cam->Update();
	loadingImage->Update();

	// 실행되지 않은 스레드가 있다면 실행
	if (!SCENE->loadingSC.b_started && SCENE->loadingSC.type != LOADINGTYPE::NONE)
	{
		RESOURCE->ResetAllCapacity();
		SCENE->loadingSC.func_calculate();
		SCENE->loadingSC.th_loading = new thread([]()
			{
				SCENE->loadingSC.func_load();
				SCENE->loadingSC.b_finished = true;
			}
		);
		SCENE->loadingSC.thread_id = SCENE->loadingSC.th_loading->get_id();
		SCENE->loadingSC.b_started = true;

		if (curLoadingCount >= maxLoadingCount)
		{
			curLoadingCount = 0;
			switch (SCENE->loadingSC.type)
			{
			case LOADINGTYPE::COMPONENTS: maxLoadingCount = 2; break;
			default: maxLoadingCount = 1;
			}
		}
	}

	// 로딩상태 관리
	// 스레드가 없으면 반환
	if (SCENE->loadingSC.th_loading == nullptr)
		return;

	// 로딩 ImGui창 표시
	{
		ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoDocking;

		// 표시할 창 위치크기 설정
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), 0, { 0.5f,0.5f });
		ImGui::SetNextWindowSize({ 400.0f, 0.0f });

		// Begin
		ImGui::Begin("loading...##LoadingScene", NULL, flags);

		// 표시된 창 디자인
		string str;
		string str2 = "( " + to_string(RESOURCE->GetCurrentCapacity() / 1024) + " / " + to_string(RESOURCE->GetTotalCapacity() / 1024) + " KiB )";
		float progress = static_cast<float>(static_cast<double>(RESOURCE->GetCurrentCapacity()) / static_cast<double>(RESOURCE->GetTotalCapacity()));
		switch (SCENE->loadingSC.type)
		{
		case LOADINGTYPE::COMPONENTS:	str = u8"게임 구성요소를 불러오고 있습니다"; break;
		case LOADINGTYPE::LOAD_SC1:		str = u8"스테이지를 불러오고 있습니다"; break;
		default:						str = u8"로딩중";
		}
		str += "...(" + to_string(curLoadingCount) + "/" + to_string(maxLoadingCount) + ")";
		
		GUI->TextCentered(str);
		ImGui::Dummy(ImVec2(0.0f,5.0f));
		GUI->TextCentered(str2);
		ImGui::ProgressBar(progress, ImVec2(-1.0f,0.0f));
		ImGui::End();
	}

	// 로딩이 끝나지 않았으면 반환
	if (!SCENE->loadingSC.b_finished)
		return;

	// 로딩이 끝났다면 조인 후 초기화
	SCENE->loadingSC.th_loading->join();
	SafeDelete(SCENE->loadingSC.th_loading);
	LOADINGTYPE tempType = SCENE->loadingSC.type;
	SCENE->loadingSC.type = LOADINGTYPE::NONE;
	curLoadingCount++;

	// 다음 함수 실행
	switch (tempType)
	{
	case LOADINGTYPE::COMPONENTS:
		SCENE->SetLoadingScene(LOADINGTYPE::LOAD_SC1,
			[]()
			{
				Scene1::Calculate_Init();
			},
			[]()
			{
				SCENE->AddScene(SCENEKEY::SC1, new Scene1());
				SCENE->GetScene(SCENEKEY::SC1)->Init();
			});
		break;
	case LOADINGTYPE::LOAD_SC1:
		SCENE->ChangeScene(SCENEKEY::SC1);
		break;
	}
}

void LoadingScene::LateUpdate()
{
}

void LoadingScene::DebugRender()
{
	ImGui::Text("Loading: %lld%% (%lld / %lld)", (int)(((float)RESOURCE->GetCurrentCapacity() / (float)RESOURCE->GetTotalCapacity()) * 100), RESOURCE->GetCurrentCapacity(), RESOURCE->GetTotalCapacity());
	SCENE->postEffect->RenderDetail();
	ImGui::Begin("Hierarchy");
	Cam->RenderHierarchy();
	loadingImage->RenderHierarchy();
	ImGui::End();
}

void LoadingScene::PreRender()
{
	SCENE->postEffect->SetCapture();
	Cam->Set();
	loadingImage->Render();
}

void LoadingScene::Render()
{
	DEPTH->Set(false);
	//포스트이펙트 렌더
	SCENE->postEffect->Render();
	DEPTH->Set(true);
}

void LoadingScene::ResizeScreen()
{
	if (!Cam)
		return;
	Cam->width = App.GetWidth();
	Cam->height = App.GetHeight();
	Cam->viewport.width = App.GetWidth();
	Cam->viewport.height = App.GetHeight();

	loadingImage->SetWorldPos({ 0.0f,0.0f,0.0f });
	float ratio_img = (float)loadingImage->material->diffuseMap->image.GetMetadata().width / (float)loadingImage->material->diffuseMap->image.GetMetadata().height; // 이미지 화면비
	float ratio_screen = App.GetWidth() / App.GetHeight();	// 창 화면비
	loadingImage->scale = { ratio_img / ratio_screen,1.0f,1.0f };
}
