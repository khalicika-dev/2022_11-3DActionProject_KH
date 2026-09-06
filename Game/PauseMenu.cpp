#include "stdafx.h"
#include "PauseMenu.h"

PauseMenu::PauseMenu()
{
	// 커서 정의
	tex_cursor = RESOURCE->textures.Load("UI/cursor.png");
	cursorSize = { static_cast<float>(tex_cursor->image.GetMetadata().width), static_cast<float>(tex_cursor->image.GetMetadata().height) };

	// 메뉴 정의
	menus[MenuKey::PAUSE] = MenuProperty(u8"일시정지##menus", { MenuKey::HTP , MenuKey::OPTION, MenuKey::EXIT_CONFIRM }, 300, 0, &PauseMenu::ShowPauseMenu, false, 0);
	menus[MenuKey::TITLE] = MenuProperty(u8"Title##menus", { MenuKey::HTP , MenuKey::OPTION, MenuKey::EXIT_CONFIRM }, 450, 0, &PauseMenu::ShowTitleMenu, false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
	menus[MenuKey::HTP] = MenuProperty(u8"게임방법##menus", {}, 500, 0.5f, &PauseMenu::ShowHtpMenu, false, 0);
	menus[MenuKey::OPTION] = MenuProperty(u8"옵션##menus", { MenuKey::OPTION_CONFIRM, MenuKey::RESET_CONFIRM }, 500, 0.5f, &PauseMenu::ShowOptionMenu, false, 0);
	menus[MenuKey::OPTION_CONFIRM] = MenuProperty("Option_confirm", { }, 0, 0, &PauseMenu::ShowOptionConfirm, true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
	menus[MenuKey::EXIT_CONFIRM] = MenuProperty("Exit_confirm", { }, 0, 0, &PauseMenu::ShowExitConfirm, true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
	menus[MenuKey::RESTART_CONFIRM] = MenuProperty("Restart_confirm", { }, 0, 0, &PauseMenu::ShowRestartConfirm, true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
	menus[MenuKey::RESET_CONFIRM] = MenuProperty("Reset_confirm", { }, 0, 0, &PauseMenu::ShowResetConfirm, true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);

	// 변수 초기화
	b_pauseMenuLoaded = false;
	closeSignal.b_received = false;
	closeSignal.b_allClose = false;
}

PauseMenu::~PauseMenu()
{
}

void PauseMenu::Update()
{
	ImGui::PushID("MenuStyle##PauseMenu");
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { 0.5f,0.5f });

	// 가장 마지막에 열린 자식 창 선정
	closeSignal.targetName = MenuKey::NONE;
	for (auto it = menus.begin(); it != menus.end(); it++)
	{
		if (!it->second.b_open) continue;

		bool check = true;
		for (auto it2 = it->second.childs.begin(); it2 != it->second.childs.end(); it2++)
		{
			if (menus[*it2].b_open)
			{
				check = false;
				break;
			}
		}
		if (check)
		{
			// ESC(창닫음)신호 수신, 열려있는 마지막 자식부터 닫힌다
			closeSignal.targetName = it->first;

			// 만약 모든 창을 닫는 신호이고 마지막 자식이 확인창이 아니라면 해당 시도를 하는 변수가 켜진다.
			if (closeSignal.b_received && closeSignal.b_allClose && !it->second.isConfirmType)
				b_menuAllClosing = true;

			// 포커싱되지 않은 창이 있으면 열려있는 마지막 자식이 포커싱된다.
			if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
				ImGui::SetWindowFocus(it->second.name.c_str());
			break;
		}
	}
	if (closeSignal.targetName == MenuKey::NONE)
	{
		ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	}
	else
	{
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	}

	// 일시정지 메뉴가 열릴 때
	if (VAR->isPause && !b_pauseMenuLoaded)
	{
		INPUT->ResetVibration();
		menus[MenuKey::PAUSE].b_open = true;
		b_pauseMenuLoaded = true;
	}

	// 타이틀 메뉴가 열릴 때
	if (PLAYER_UI->GetGameStart() < 0 && !menus[MenuKey::TITLE].b_open)
	{
		menus[MenuKey::TITLE].b_open = true;
	}
	
	/* 메뉴 표시 */
	//if (!VAR->isPause) return;

	//메뉴 디자인
	for (auto it = menus.begin(); it != menus.end(); it++)
	{
		// 신호를 받으면 마지막 창이 닫힌다.
		bool cond = (b_menuAllClosing || closeSignal.b_received) && closeSignal.targetName == it->first;

		// 단, 특정 상황에서는 신호가 무시된다.
		if (cond && it->second.isConfirmType && (
			closeSignal.b_allClose ||
			(b_menuAllClosing && !closeSignal.b_received)))
			cond = false;

		// 메뉴를 업데이트
		ShowMenu(it->second, cond);
	}
	if (closeSignal.targetName == MenuKey::TITLE || closeSignal.targetName == MenuKey::NONE)
		b_menuAllClosing = false;
	closeSignal.b_received = false;
	closeSignal.b_allClose = false;

	// 일시정지 메뉴가 닫힐 때
	if (!menus[MenuKey::PAUSE].b_open && VAR->isPause && b_pauseMenuLoaded)
	{
		for (auto it = menus.begin(); it != menus.end(); it++)
			it->second.b_open = false;
		b_pauseMenuLoaded = false;
		INPUT->TogglePuase();
	}

	ImGui::PopStyleVar(2);
	ImGui::PopID();
}

void PauseMenu::RenderCursor()
{
	// 인게임 내에서 커서를 표시할 수 있는 조건
	bool cond = (SCENE->GetCurrentScene() == SCENE->GetScene(SCENEKEY::LOADING)) ||
		(Player_Interface::isHaveInstance() && PLAYER_UI->GetGameStart() < 0) ||
		VAR->isPause;

	// 패드를 사용하지 않는 중 조건을 만족하면 커서표시 (Debug모드에서는 상시 표시)
	if ((!INPUT->usePad && cond) || VAR->isDebug)
		ImGui::GetForegroundDrawList()->AddImage((void*)tex_cursor->GetTextureSRV(0), ImGui::GetMousePos(), { ImGui::GetMousePos().x + cursorSize.x,ImGui::GetMousePos().y + cursorSize.y });

	if (VAR->isDebug)
	{
		ImGui::Text("MouseImgui: %f, %f", ImGui::GetMousePos().x, ImGui::GetMousePos().y);
	}
}

void PauseMenu::ShowMenu(MenuProperty& menu, bool closeSignal)
{
	if (!menu.b_open) 
		return;

	// 자식창이 열려있으면 비활성화
	bool b_pauseMenuDisabled = false;
	for (auto it = menu.childs.begin(); it != menu.childs.end(); it++)
	{
		if (menus[*it].b_open)
		{
			b_pauseMenuDisabled = true;
			break;
		}
	}

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoDocking;
	flags |= menu.addFlags;
	if (b_pauseMenuDisabled) 
		flags |= ImGuiWindowFlags_NoInputs;

	// 표시할 창 위치크기 설정
	if (menu.name == menus[MenuKey::TITLE].name)
	{
		float yPos = ImGui::GetMainViewport()->Pos.y + ImGui::GetMainViewport()->Size.y;
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->GetCenter().x, yPos - App.GetHeight() * 0.05f }, 0, { 0.5f,1.0f });
	}
	else
	{
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), 0, { 0.5f,0.5f });
	}
	ImGui::SetNextWindowSize({ menu.width,
		menu.heightRatio > 0 ? App.GetHeight() * menu.heightRatio : 0 });


	// Begin
	ImGui::Begin(menu.name.c_str(), NULL, flags);

	// 표시된 창 크기 조절
	const float maxHeight = App.GetHeight() - 10.0f;
	if (ImGui::GetCurrentWindow()->SizeFull.y > maxHeight)
		ImGui::SetWindowSize({ menu.width, maxHeight });

	// 표시된 창 디자인
	ImGui::BeginDisabled(b_pauseMenuDisabled);
	menu.showFunc(*this, closeSignal);
	ImGui::EndDisabled();
	ImGui::End();
}

void PauseMenu::ShowPauseMenu(bool closeSignal)
{
	// 메뉴 구성요소 업데이트
	ImGui::Text("FPS: %d", TIMER->GetFramePerSecond());
	if (ImGui::Button(u8"재개##pause", { ImGui::GetContentRegionAvail().x,0 }) || closeSignal)
		menus[MenuKey::PAUSE].b_open = false;
	if (ImGui::Button(u8"게임방법##pause", { ImGui::GetContentRegionAvail().x,0 }))
		menus[MenuKey::HTP].b_open = true;
	if (ImGui::Button(u8"옵션##pause", { ImGui::GetContentRegionAvail().x,0 }))
	{
		CopySettings();
		menus[MenuKey::OPTION].b_open = true;
	}
	ImGui::Separator();
	if (ImGui::Button(u8"다시 시작##pause", { ImGui::GetContentRegionAvail().x,0 }))
		menus[MenuKey::RESTART_CONFIRM].b_open = true;
	if (ImGui::Button(u8"게임 종료##pause", { ImGui::GetContentRegionAvail().x,0 }))
		menus[MenuKey::EXIT_CONFIRM].b_open = true;
}

void PauseMenu::ShowTitleMenu(bool closeSignal)
{
	ImGui::SetCurrentFont(GUI->bigFont);
	ImGui::PushID(50);
	ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(33, 34, 45));
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(148, 156, 178));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(187, 197, 225));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(130, 146, 190));
	ImGui::PushStyleColor(ImGuiCol_Border, (ImVec4)ImColor(212, 223, 255));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

	if (ImGui::Button(u8"시작##title", { ImGui::GetContentRegionAvail().x,40 }))
	{
		PLAYER_UI->GameStart();
		menus[MenuKey::TITLE].b_open = false;
	}

	if (ImGui::Button(u8"게임방법##title", { ImGui::GetContentRegionAvail().x,40 }))
		menus[MenuKey::HTP].b_open = true;

	if (ImGui::Button(u8"옵션##title", { ImGui::GetContentRegionAvail().x,40 }))
	{
		CopySettings();
		menus[MenuKey::OPTION].b_open = true;
	}
	if (ImGui::Button(u8"게임 종료##title", { ImGui::GetContentRegionAvail().x,40 }))
		menus[MenuKey::EXIT_CONFIRM].b_open = true;
	ImGui::PopStyleVar(1);
	ImGui::PopStyleColor(5);
	ImGui::PopID();
	ImGui::SetCurrentFont(GUI->smallFont);
}

void PauseMenu::ShowHtpMenu(bool closeSignal)
{
	const char* label_backButton = u8"돌아가기##htp";
	ImVec2 size_backButton = ImGui::CalcTextSize(label_backButton, NULL, true);
	ImGui::Text("%s", INPUT->usePad ? u8"패드 사용 시 XBOX 기준으로 설명합니다." : u8" ");
	if (ImGui::BeginTabBar("Tab_HTP", ImGuiTabBarFlags_None))
	{
		list<pair<const char*, function<void()>>> details;
		details.push_back(make_pair(u8"기본 조작법##htp", []()
			{
				ImGui::TextWrapped(u8"[일시정지]");
				ImGui::TextWrapped(u8"일시정지 메뉴열기: %s", INPUT->usePad ? u8"Menu버튼" : u8"ESC키");
				ImGui::NewLine();
				ImGui::TextWrapped(u8"[플레이어 캐릭터 조작]");
				ImGui::TextWrapped(u8"이동: %s", INPUT->usePad ? u8"L스틱 이동" : u8"WASD키");
				ImGui::TextWrapped(u8"달리기: %s 누르면서 이동", INPUT->usePad ? u8"RB버튼을" : u8"Shift키를");
				ImGui::TextWrapped(u8"약공격: %s", INPUT->usePad ? u8"X버튼" : u8"마우스 좌클릭");
				ImGui::TextWrapped(u8"강공격: %s", INPUT->usePad ? u8"Y버튼" : u8"마우스 우클릭");
				ImGui::TextWrapped(u8"회피: %s", INPUT->usePad ? u8"B버튼" : u8"E키");
				ImGui::TextWrapped(u8"방어: %s (누르는동안 지속가능)", INPUT->usePad ? u8"RB+B버튼" : u8"R키");
				ImGui::TextWrapped(u8"점프: %s (공중에서는 1회 사용가능)", INPUT->usePad ? u8"A버튼" : u8"Space키");
				ImGui::NewLine();
				ImGui::TextWrapped(u8"[소지품창 사용]");
				ImGui::TextWrapped(u8"소지품창 열기: %s", INPUT->usePad ? u8"View버튼" : u8"Tab키");
				ImGui::TextWrapped(u8"소지품창 닫기: 아이템창을 연 상태에서 %s", INPUT->usePad ? u8"View버튼" : u8"Tab키");
				ImGui::TextWrapped(u8"소지품 사용: 아이템창을 연 상태에서 %s", INPUT->usePad ? u8"X버튼" : u8"Space키");
				ImGui::TextWrapped(u8"소지품 전환: %s", INPUT->usePad ? u8"LB버튼, RB버튼" : u8"Q키, E키");
				ImGui::NewLine();
				ImGui::TextWrapped(u8"[카메라 조작]");
				ImGui::TextWrapped(u8"카메라 회전: %s", INPUT->usePad ? u8"R스틱 이동" : u8"마우스 이동");
				ImGui::TextWrapped(u8"록온: %s", INPUT->usePad ? u8"R스틱 누름" : u8"마우스 휠 버튼");
				ImGui::TextWrapped(u8"카메라 시점초기화: %s", INPUT->usePad ? u8"LB버튼" : u8"Q키");
				ImGui::TextWrapped(u8"카메라 시점확대: %s", INPUT->usePad ? u8"↑버튼" : u8"PgUp키");
				ImGui::TextWrapped(u8"카메라 시점축소: %s", INPUT->usePad ? u8"↓버튼" : u8"PgDn키");
			}
		));
		details.push_back(make_pair(u8"HP와 ST##htp", []()
			{
				ImGui::TextWrapped(u8"HP: 플레이어의 체력을 나타내는 붉은색 게이지입니다.");
				ImGui::Indent();
				ImGui::TextWrapped(u8"적에게 공격을 받으면 소모되며 소진 시 행동불능이 됩니다. 소지품창의 체력회복 포션, 전체회복 포션으로 회복할 수 있습니다.");
				ImGui::Unindent();
				ImGui::NewLine();
				ImGui::TextWrapped(u8"ST: 플레이어의 기력을 나타내는 노란색 게이지입니다.");
				ImGui::Indent();
				ImGui::TextWrapped(u8"일부 행동을 할 때 소모되며 소진 시 탈진으로 일정 시간동안 움직일 수 없습니다. 지상에서 자연회복이 가능하며 전체회복 포션으로도 회복할 수 있습니다.");
				ImGui::Unindent();
			}
		));
		details.push_back(make_pair(u8"공격##htp", []()
			{
				ImGui::TextWrapped(u8"플레이어는 약공격과 강공격을 조합해서 사용할 수 있습니다. 또한 회피나 점프중에 공격을 연계할 수 있습니다.");
				ImGui::NewLine();
				ImGui::TextWrapped(u8"약공격(%s)은 신속한 공격으로 적의 기력을 깎는 데에 특화되어 있습니다.", 
					INPUT->usePad ? u8"X버튼" : u8"마우스 좌클릭");
				ImGui::Spacing();
				ImGui::TextWrapped(u8"강공격(%s)은 ST를 사용하는 강렬한 공격으로 적에게 큰 피해를 줄 수 있습니다.", 
					INPUT->usePad ? u8"Y버튼" : u8"마우스 우클릭");
			}
		));
		details.push_back(make_pair(u8"방어##htp", []()
			{
				ImGui::TextWrapped(u8"플레이어는 방어(%s)를 사용하는 동안에는 적에게 받는 HP피해를 경감할 수 있습니다.",
					INPUT->usePad ? u8"RB+B버튼" : u8"R키");
				ImGui::NewLine();
				ImGui::TextWrapped(u8"적의 공격을 방어할 때마다 ST가 소모됩니다. 공중에서도 방어할 수 있습니다.");
				ImGui::Spacing();
				ImGui::TextWrapped(u8"적에게 공격받는 타이밍에 맞춰 방어를 전개하는 데 성공하면 HP피해와 ST소모없이 막아낼 수 있습니다.");
			}
		));
		details.push_back(make_pair(u8"소지품##htp", []()
			{
				ImGui::TextWrapped(u8"플레이어의 전투를 지속하기 위한 수단입니다. %s 눌러 소지품창을 열 수 있습니다.",
					INPUT->usePad ? u8"View버튼을" : u8"Tab키를");
				ImGui::NewLine();
				ImGui::TextWrapped(u8"[아이템 목록]");
				ImGui::TextWrapped(u8"체력회복 포션: 플레이어의 체력을 50%% 회복.");
				ImGui::TextWrapped(u8"전체회복 포션: 플레이어의 체력과 기력을 모두 회복.");
			}
		));
		details.push_back(make_pair(u8"보스전투##htp", []()
			{
				ImGui::TextWrapped(u8"보스전투가 시작되면 화면 하단에 보스의 붉은색 체력게이지와 노란색 기력게이지가 표시됩니다.");
				ImGui::NewLine();
				ImGui::TextWrapped(u8"보스는 부위별로 가할 수 있는 피해량이 다르며, 특정부위에 피해를 누적시키면 부위파괴가 발생할 수 있습니다.");
				ImGui::Spacing();
				ImGui::TextWrapped(u8"부위파괴 시 보스의 능력저하가 발생하며 파괴된 부위의 받는 피해량이 증가하므로 이를 적극적으로 공략하여 보스에게 승리해봅시다.");
			}
		));

		for (auto it = details.begin(); it != details.end(); it++)
		{
			if (ImGui::BeginTabItem(it->first))
			{
				ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
				float marginY = size_backButton.y + ImGui::GetStyle().FramePadding.y * 2.0f + 10.0f;
				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
				ImGui::BeginChild("detail##htp", ImVec2(0, ImGui::GetContentRegionAvail().y - marginY), true, window_flags);
				it->second();
				ImGui::EndChild();
				ImGui::PopStyleVar();
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
	if (ImGui::Button(label_backButton, { ImGui::GetContentRegionAvail().x,0 }) || closeSignal)
	{
		menus[MenuKey::HTP].b_open = false;
	}
}

void PauseMenu::ShowOptionMenu(bool closeSignal)
{
	if (ImGui::CollapsingHeader(u8"그래픽##option"))
	{
		// 윈도우 설정
		{
			ImGui::AlignTextToFramePadding();
			GUI->TextWithPadding(u8"전체화면", 80.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton("ScreenSetting_left##option", ImGuiDir_Left))
			{
				if (CONFIG->settings.idx_screenSetting <= 0) CONFIG->settings.idx_screenSetting = CONFIG->screenSettingVector.size();
				CONFIG->settings.idx_screenSetting--;
			}
			ImGui::SameLine();
			GUI->TextWithPadding(CONFIG->screenSettingVector[CONFIG->settings.idx_screenSetting].second, 200.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton("ScreenSetting_right##option", ImGuiDir_Right))
			{
				CONFIG->settings.idx_screenSetting++;
				if (CONFIG->settings.idx_screenSetting >= CONFIG->screenSettingVector.size()) CONFIG->settings.idx_screenSetting = 0;
			}
		}

		// 해상도 설정
		{
			ImGui::BeginDisabled(CONFIG->screenSettingVector[CONFIG->settings.idx_screenSetting].first == Window::ScreenSettings_FULLSCREEN);
			ImGui::AlignTextToFramePadding();
			GUI->TextWithPadding(u8"해상도 크기", 80.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton("ResolutionSetting_left##option", ImGuiDir_Left))
			{
				if (CONFIG->settings.idx_resolutionSetting <= 0) CONFIG->settings.idx_resolutionSetting = CONFIG->resolutionSettingVector.size();
				CONFIG->settings.idx_resolutionSetting--;
			}
			ImGui::SameLine();
			string str = to_string(CONFIG->resolutionSettingVector[CONFIG->settings.idx_resolutionSetting].second.x) + "x" + to_string(CONFIG->resolutionSettingVector[CONFIG->settings.idx_resolutionSetting].second.y);
			if (CONFIG->resolutionSettingVector[CONFIG->settings.idx_resolutionSetting].first == Window::ResolutionSettings_DEFAULT)
				str += u8" (권장)";
			GUI->TextWithPadding(str.c_str(), 200.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton("ResolutionSetting_right##option", ImGuiDir_Right))
			{
				CONFIG->settings.idx_resolutionSetting++;
				if (CONFIG->settings.idx_resolutionSetting >= CONFIG->resolutionSettingVector.size()) CONFIG->settings.idx_resolutionSetting = 0;
			}
			ImGui::EndDisabled();
		}

		// 수직동기화
		{
			ImGui::AlignTextToFramePadding();
			GUI->TextWithPadding(u8"수직 동기화", 80.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton("vSyncSetting_left##option", ImGuiDir_Left))
				*CONFIG->settings.b_vSync = !(*CONFIG->settings.b_vSync);
			ImGui::SameLine();
			GUI->TextWithPadding(App.vSync ? u8"켜기" : u8"끄기", 200, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton("vSyncSetting_right##option", ImGuiDir_Right))
				*CONFIG->settings.b_vSync = !(*CONFIG->settings.b_vSync);
		}

		// 그림자 설정
		{
			bool b_changed = false;
			ImGui::AlignTextToFramePadding();
			GUI->TextWithPadding(u8"그림자 품질", 80.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton("ShadowSetting_left##option", ImGuiDir_Left))
			{
				if (CONFIG->settings.idx_shadowSetting <= 0) CONFIG->settings.idx_shadowSetting = CONFIG->shadowSettingVector.size();
				CONFIG->settings.idx_shadowSetting--;
				b_changed = true;
			}
			ImGui::SameLine();
			GUI->TextWithPadding(CONFIG->shadowSettingVector[CONFIG->settings.idx_shadowSetting].first, 200.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton("ShadowSetting_right##option", ImGuiDir_Right))
			{
				CONFIG->settings.idx_shadowSetting++;
				if (CONFIG->settings.idx_shadowSetting >= CONFIG->shadowSettingVector.size()) CONFIG->settings.idx_shadowSetting = 0;
				b_changed = true;
			}
			if (b_changed)
			{
				SCENE->shadow->SetProperty(
					CONFIG->shadowSettingVector[CONFIG->settings.idx_shadowSetting].second.textureSize,
					CONFIG->shadowSettingVector[CONFIG->settings.idx_shadowSetting].second.range,
					CONFIG->shadowSettingVector[CONFIG->settings.idx_shadowSetting].second.quality);
			}
		}
	}
	if (ImGui::CollapsingHeader(u8"사운드##option"))
	{
		ImGui::AlignTextToFramePadding(); GUI->TextWithPadding(u8"마스터", 80.0f, Gui::EnumTextAlignCenter); ImGui::SameLine(); ImGui::SliderInt("##master_option", &CONFIG->settings.volume_master, 0, 100);
		ImGui::AlignTextToFramePadding(); GUI->TextWithPadding(u8"배경음", 80.0f, Gui::EnumTextAlignCenter); ImGui::SameLine(); ImGui::SliderInt("##bgm_option", &CONFIG->settings.volume_bgm, 0, 100);
		ImGui::AlignTextToFramePadding(); GUI->TextWithPadding(u8"효과음", 80.0f, Gui::EnumTextAlignCenter); ImGui::SameLine(); ImGui::SliderInt("##se_option", &CONFIG->settings.volume_se, 0, 100);
		ImGui::AlignTextToFramePadding(); GUI->TextWithPadding(u8"환경음", 80.0f, Gui::EnumTextAlignCenter); ImGui::SameLine(); ImGui::SliderInt("##env_option", &CONFIG->settings.volume_env, 0, 100);
	}
	if (ImGui::CollapsingHeader(u8"액션##option"))
	{
		// 록온
		{
			ImGui::AlignTextToFramePadding();
			GUI->TextWithPadding(u8"록온대상", 100.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton("lockOnSetting_left##option", ImGuiDir_Left))
				CONFIG->settings.b_lockOnParts = !CONFIG->settings.b_lockOnParts;
			ImGui::SameLine();
			GUI->TextWithPadding(CONFIG->settings.b_lockOnParts ? u8"부위별" : u8"몬스터별", 70.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton("lockOnSetting_right##option", ImGuiDir_Right))
				CONFIG->settings.b_lockOnParts = !CONFIG->settings.b_lockOnParts;
		}
		
		// 록온 스피드
		{
			ImGui::AlignTextToFramePadding();
			GUI->TextWithPadding(u8"록온 시 속도보정", 100.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton("lockOnSpeedSetting_left##option", ImGuiDir_Left))
			{
				if (CONFIG->settings.idx_lockOnSpeedSetting <= 0) CONFIG->settings.idx_lockOnSpeedSetting = CONFIG->lockOnSpeedSettingVector.size();
				CONFIG->settings.idx_lockOnSpeedSetting--;
			}
			ImGui::SameLine();
			GUI->TextWithPadding(CONFIG->lockOnSpeedSettingVector[CONFIG->settings.idx_lockOnSpeedSetting].first, 70.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton("lockOnSpeedSetting_right##option", ImGuiDir_Right))
			{
				CONFIG->settings.idx_lockOnSpeedSetting++;
				if (CONFIG->settings.idx_lockOnSpeedSetting >= CONFIG->lockOnSpeedSettingVector.size()) CONFIG->settings.idx_lockOnSpeedSetting = 0;
			}
		}
	}
	if (ImGui::CollapsingHeader(u8"카메라##option"))
	{
		// 카메라거리
		{
			ImGui::AlignTextToFramePadding();
			GUI->TextWithPadding(u8"카메라 거리", 100.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton("zoomSetting_left##option", ImGuiDir_Left))
			{
				if (CONFIG->settings.Idx_zoomSetting <= 0) CONFIG->settings.Idx_zoomSetting = CONFIG->zoomSettingVector.size();
				CONFIG->settings.Idx_zoomSetting--;
			}
			ImGui::SameLine();
			GUI->TextWithPadding(CONFIG->zoomSettingVector[CONFIG->settings.Idx_zoomSetting], 70.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton("zoomSetting_right##option", ImGuiDir_Right))
			{
				CONFIG->settings.Idx_zoomSetting++;
				if (CONFIG->settings.Idx_zoomSetting >= CONFIG->zoomSettingVector.size()) CONFIG->settings.Idx_zoomSetting = 0;
			}
		}

		for (auto it = CONFIG->cameraSettings.begin(); it != CONFIG->cameraSettings.end(); it++)
		{
			ImGui::Separator();
			ImGui::Text(u8"%s - 카메라설정", it->second.ImGuiName);

			string arrowButtonID_Left = it->second.ImGuiName + (string)"cameraSpeedSetting_left##option";
			string arrowButtonID_Right = it->second.ImGuiName + (string)"cameraSpeedSetting_right##option";
			ImGui::AlignTextToFramePadding();
			GUI->TextWithPadding(u8"카메라 회전속도", 100.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton(arrowButtonID_Left.c_str(), ImGuiDir_Left))
			{
				if (CONFIG->settings.Idx_speedSetting[it->first] <= 0) CONFIG->settings.Idx_speedSetting[it->first] = it->second.speedSettingVector.size();
				CONFIG->settings.Idx_speedSetting[it->first]--;
			}
			ImGui::SameLine();
			GUI->TextWithPadding(it->second.speedSettingVector[CONFIG->settings.Idx_speedSetting[it->first]].first, 70.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton(arrowButtonID_Right.c_str(), ImGuiDir_Right))
			{
				CONFIG->settings.Idx_speedSetting[it->first]++;
				if (CONFIG->settings.Idx_speedSetting[it->first] >= it->second.speedSettingVector.size()) CONFIG->settings.Idx_speedSetting[it->first] = 0;
			}

			arrowButtonID_Left = it->second.ImGuiName + (string)"cameraHReverseSetting_left##option";
			arrowButtonID_Right = it->second.ImGuiName + (string)"cameraHReverseSetting_right##option";
			ImGui::AlignTextToFramePadding();
			GUI->TextWithPadding(u8"수평 회전방향", 100.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton(arrowButtonID_Left.c_str(), ImGuiDir_Left))
				CONFIG->settings.b_reverseDirHorizontal[it->first] = !CONFIG->settings.b_reverseDirHorizontal[it->first];
			ImGui::SameLine();
			GUI->TextWithPadding(CONFIG->settings.b_reverseDirHorizontal[it->first] ? u8"역방향" : u8"표준", 70.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton(arrowButtonID_Right.c_str(), ImGuiDir_Right))
				CONFIG->settings.b_reverseDirHorizontal[it->first] = !CONFIG->settings.b_reverseDirHorizontal[it->first];

			arrowButtonID_Left = it->second.ImGuiName + (string)"cameraVReverseSetting_left##option";
			arrowButtonID_Right = it->second.ImGuiName + (string)"cameraVReverseSetting_right##option";
			ImGui::AlignTextToFramePadding();
			GUI->TextWithPadding(u8"수직 회전방향", 100.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton(arrowButtonID_Left.c_str(), ImGuiDir_Left))
				CONFIG->settings.b_reverseDirVertical[it->first] = !CONFIG->settings.b_reverseDirVertical[it->first];
			ImGui::SameLine();
			GUI->TextWithPadding(CONFIG->settings.b_reverseDirVertical[it->first] ? u8"역방향" : u8"표준", 70.0f, Gui::EnumTextAlignCenter);
			ImGui::SameLine();
			if (ImGui::ArrowButton(arrowButtonID_Right.c_str(), ImGuiDir_Right))
				CONFIG->settings.b_reverseDirVertical[it->first] = !CONFIG->settings.b_reverseDirVertical[it->first];
		}
	}
	if (ImGui::CollapsingHeader(u8"기타##option"))
	{
		if (ImGui::Checkbox(u8"컨트롤러 진동##option", CONFIG->settings.b_enableVibration))
		{
			if (CONFIG->settings.b_enableVibration)
				INPUT->SetVibration(0.5f, 0.5f, 0.0f);
			else
				INPUT->ResetVibration();
		}
		ImGui::Checkbox(u8"(개발자)HUD 표시##option", &CONFIG->settings.b_HUDVisible);
		ImGui::Checkbox(u8"(개발자)콜라이더 표시##option", CONFIG->settings.b_showColliderComponents);
	}
	ImGui::NewLine();
	if (ImGui::Button(u8"초기 설정으로 복원##option", {ImGui::GetContentRegionAvail().x,0}))
		menus[MenuKey::RESET_CONFIRM].b_open = true;
	if (ImGui::Button(u8"돌아가기##option", { ImGui::GetContentRegionAvail().x,0 }) || closeSignal)
	{
		if (tempSettings.idx_screenSetting != CONFIG->settings.idx_screenSetting ||
			(CONFIG->screenSettingVector[CONFIG->settings.idx_screenSetting].first == Window::ScreenSettings_WINDOW && tempSettings.idx_resolutionSetting != CONFIG->settings.idx_resolutionSetting) ||
			*tempSettings.b_vSync != *CONFIG->settings.b_vSync ||
			tempSettings.idx_shadowSetting != CONFIG->settings.idx_shadowSetting)
			menus[MenuKey::OPTION_CONFIRM].b_open = true;
		else
			menus[MenuKey::OPTION].b_open = false;
	}
}

void PauseMenu::CopySettings()
{
	tempSettings = CONFIG->settings;
	tempSettings.b_vSync = &tempVar[0];
	*tempSettings.b_vSync = *CONFIG->settings.b_vSync;
	tempSettings.b_enableVibration = &tempVar[1];
	*tempSettings.b_enableVibration = *CONFIG->settings.b_enableVibration;
	tempSettings.b_showColliderComponents = &tempVar[2];
	*tempSettings.b_showColliderComponents = *CONFIG->settings.b_showColliderComponents;
}

void PauseMenu::ShowOptionConfirm(bool closeSignal)
{
	GUI->TextCentered(u8"변경된 그래픽 설정이 있습니다.");
	GUI->TextCentered(u8"반영하시겠습니까?");
	ImGui::Spacing();
	if (ImGui::Button(u8"변경##optconfirm", ImVec2(200, 0))) 
	{ 
		if (tempSettings.idx_screenSetting != CONFIG->settings.idx_screenSetting)
		{
			WIN->ChangeWindow(CONFIG->screenSettingVector[CONFIG->settings.idx_screenSetting].first);
		}
		if (CONFIG->screenSettingVector[CONFIG->settings.idx_screenSetting].first == Window::ScreenSettings_FULLSCREEN)
			CONFIG->settings.idx_resolutionSetting = 0;	// 풀스크린에서는 권장해상도로 변경
		if (tempSettings.idx_resolutionSetting != CONFIG->settings.idx_resolutionSetting)
		{
			WIN->ChangeResolution(CONFIG->resolutionSettingVector[CONFIG->settings.idx_resolutionSetting].second);
		}
		menus[MenuKey::OPTION_CONFIRM].b_open = false;
		menus[MenuKey::OPTION].b_open = false;
	}
	if (ImGui::Button(u8"변경하지 않고 나가기##optconfirm", ImVec2(200, 0)))
	{
		CONFIG->settings.idx_screenSetting = tempSettings.idx_screenSetting;
		CONFIG->settings.idx_resolutionSetting = tempSettings.idx_resolutionSetting;
		*CONFIG->settings.b_vSync = *tempSettings.b_vSync;
		CONFIG->settings.idx_shadowSetting = tempSettings.idx_shadowSetting;
		SCENE->shadow->SetProperty(
			CONFIG->shadowSettingVector[CONFIG->settings.idx_shadowSetting].second.textureSize,
			CONFIG->shadowSettingVector[CONFIG->settings.idx_shadowSetting].second.range,
			CONFIG->shadowSettingVector[CONFIG->settings.idx_shadowSetting].second.quality);
		menus[MenuKey::OPTION_CONFIRM].b_open = false;
		menus[MenuKey::OPTION].b_open = false;
	}
	if (ImGui::Button(u8"돌아가기##optconfirm", ImVec2(200, 0)) || closeSignal)
	{
		menus[MenuKey::OPTION_CONFIRM].b_open = false;
		b_menuAllClosing = false;
	}
}

void PauseMenu::ShowResetConfirm(bool closeSignal)
{
	GUI->TextCentered(u8"모든 설정을 초기값으로 복원하시겠습니까?");
	ImGui::Spacing();
	if (ImGui::Button(u8"예##resetconfirm", ImVec2(300, 0)))
	{
		CONFIG->Reset(CONFIG->settings);
		SCENE->shadow->SetProperty(
			CONFIG->shadowSettingVector[CONFIG->settings.idx_shadowSetting].second.textureSize,
			CONFIG->shadowSettingVector[CONFIG->settings.idx_shadowSetting].second.range,
			CONFIG->shadowSettingVector[CONFIG->settings.idx_shadowSetting].second.quality);
		menus[MenuKey::RESET_CONFIRM].b_open = false;
	}
	if (ImGui::Button(u8"아니오##resetconfirm", ImVec2(300, 0)) || closeSignal)
	{
		menus[MenuKey::RESET_CONFIRM].b_open = false;
		b_menuAllClosing = false;
	}
}


void PauseMenu::ShowExitConfirm(bool closeSignal)
{
	GUI->TextCentered(u8"게임을 종료하시겠습니까?");
	GUI->TextCentered(u8"설정한 모든 옵션들은 저장됩니다.");
	ImGui::Spacing();
	if (ImGui::Button(u8"예##exitconfirm", ImVec2(200, 0)))
		PostQuitMessage(0);
	if (ImGui::Button(u8"아니오##exitconfirm", ImVec2(200, 0)) || closeSignal)
		menus[MenuKey::EXIT_CONFIRM].b_open = false;
}

void PauseMenu::ShowRestartConfirm(bool closeSignal)
{
	GUI->TextCentered(u8"게임을 재시작하시겠습니까?");
	GUI->TextCentered(u8"모든 진행상황이 초기화됩니다.");
	ImGui::Spacing();
	if (ImGui::Button(u8"예##restartconfirm", ImVec2(200, 0)))
	{
		SOUND->StopAllSound();
		for (auto it = menus.begin(); it != menus.end(); it++)
			it->second.b_open = false;
		b_pauseMenuLoaded = false;
		SCENE->SetLoadingScene(LOADINGTYPE::LOAD_SC1,
			[]()
			{
				Scene1::Calculate_Init();
			},
			[]()
			{
				SCENE->GetScene(SCENEKEY::SC1)->Init();
			});
	}
	if (ImGui::Button(u8"아니오##restartconfirm", ImVec2(200, 0)) || closeSignal)
		menus[MenuKey::RESTART_CONFIRM].b_open = false;
}