#include "stdafx.h"

Player_Interface::Player_Interface()
{
	Reset();
}

Player_Interface::~Player_Interface()
{
	while (!log_dmg.empty())
	{
		delete log_dmg.back();
		log_dmg.pop_back();
	}
	for (auto it = textureResources.begin(); it != textureResources.end(); it++)
	{
		if (it->second) 
		{
			(it->second).reset();
			(it->second) = nullptr;
		}
	}
}

void Player_Interface::Init()
{
	textureResources["lockoff"] = RESOURCE->textures.Load("UI/mark_lockOff.png");
	textureResources["lockon"] = RESOURCE->textures.Load("UI/mark_lockOn.png");
	textureResources["bt_Space"] = RESOURCE->textures.Load("UI/Button_Space.png");
	textureResources["bt_Q"] = RESOURCE->textures.Load("UI/Button_Q.png");
	textureResources["bt_E"] = RESOURCE->textures.Load("UI/Button_E.png");
	textureResources["bt_X"] = RESOURCE->textures.Load("UI/Button_X.png");
	textureResources["bt_LB"] = RESOURCE->textures.Load("UI/Button_LB.png");
	textureResources["bt_RB"] = RESOURCE->textures.Load("UI/Button_RB.png");

	player = GET_PLAYER;

	if (App.GetAppQuit()) 
		return;

	UI* temp;

	// 데미지수치 준비
	for (int i = 0; i < 30; i++)
	{
		//log_dmg[i]= 5;
		if (App.GetAppQuit()) return;
		log_dmg.emplace_back(new DmgNumber());
	}

	// bar_hp
	{
		bar_hp = Actor::Create("bar_hp");

		temp = UI::Create("text");
		temp->CreateMesh("UI/text_HP.png");
		temp->SetNDCPos({ 70.0f,60.0f });
		bar_hp->AddChild(temp);

		MakeGuage(bar_hp, player->hp, player->hp_max, { 90.0f,50.0f }, { 6.0f,16.0f }, { 1,0,0,1 }, 2.0f, false);
		if (App.GetAppQuit()) return;
	}

	// bar_st
	{
		bar_st = Actor::Create("bar_st");

		temp = UI::Create("text");
		temp->CreateMesh("UI/text_ST.png");
		temp->SetNDCPos({ 68.0f,91.0f });
		bar_st->AddChild(temp);

		MakeGuage(bar_st, player->st, player->st_max, { 90.0f,85.0f }, { 6.0f,10.0f }, { 1,1,0,1 }, 2.0f, false);
		if (App.GetAppQuit()) return;
	}

	// bar_bossHp
	{
		bar_bossHp = Actor::Create("bar_bossHp");

		temp = UI::Create("text");
		temp->CreateMesh("UI/text_boss.png");
		temp->SetNDCPos({ 472.0f,990.0f });
		bar_bossHp->AddChild(temp);

		MakeGuage(bar_bossHp, 100.0f, 100.0f, { 388.0f,1016.0f }, { 1143.0f,20.0f }, { 1,0,0,1 }, 2.0f, true);
		if (App.GetAppQuit()) return;
	}

	// bar_bossGroggy
	{
		bar_bossGroggy = Actor::Create("bar_bossGroggy");
		MakeGuage(bar_bossGroggy, 100.0f, 100.0f, { 388.0f,1036.0f }, { 1143.0f,5.0f }, { 1,1,0,1 }, 2.0f, true);
		if (App.GetAppQuit()) return;
	}

	// bar_item
	{
		bar_item = Actor::Create("bar_item");
		MakeItemBar(bar_item);
		if (App.GetAppQuit()) return;
	}

	// lockon_mark
	{
		lock_mark = Actor::Create("lock_mark");

		temp = UI::Create("mark");
		temp->CreateMesh(textureResources["lockoff"]);
		lock_mark->AddChild(temp);
		lock_mark->visible = false;
		if (App.GetAppQuit()) return;
	}

	// log_info
	{
		log_info = Actor::Create("log_message");

		MakeInfoBar(log_info);
		log_info->visible = false;
		if (App.GetAppQuit()) return;
	}

	// log_clear
	{
		log_clear = Actor::Create("log_clear");

		MakeClearBar(log_clear);
		log_clear->visible = false;
		if (App.GetAppQuit()) return;
	}

	// text_gameStart
	{
		text_gameStart = UI::Create("text_gameStart");
		text_gameStart->CreateMesh("UI/signal_gameStart.png", UI::AlignX::CENTER, UI::AlignY::CENTER);
		text_gameStart->SetNDCPos({ 0.0f, DEFAULT_RESOLUTION.y * 0.4f });
		text_gameStart->visible = false;
		if (App.GetAppQuit()) return;
	}

	// text_gameClear
	{
		text_gameClear = UI::Create("text_gameClear");
		text_gameClear->CreateMesh("UI/signal_gameClear.png", UI::AlignX::CENTER, UI::AlignY::CENTER);
		text_gameClear->SetNDCPos({ DEFAULT_RESOLUTION.x / 2.0f, DEFAULT_RESOLUTION.y * 0.4f });
		if (App.GetAppQuit()) return;
	}

	{
		text_title = UI::Create("text_title");
		text_title->CreateMesh("UI/text_title.png", UI::AlignX::CENTER, UI::AlignY::CENTER);
		text_title->SetNDCPos({ DEFAULT_RESOLUTION.x / 2.0f, DEFAULT_RESOLUTION.y * 0.2f });
		if (App.GetAppQuit()) return;
	}
}

void Player_Interface::Calculate()
{
	// TextureResources
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/mark_lockOff.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/mark_lockOn.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Button_Space.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Button_Q.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Button_E.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Button_X.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Button_L1.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Button_R1.png"));

	// UI Guage & Pixel
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/pixel.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/guage.png"));

	// bar_item
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/text_item.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Button_Tab.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Button_View.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/itemBar.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/item_block.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/item_blockSelect.png"));

	// log_info & log_clear
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Info/bar.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Info/header.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Info/text_clearTime.png"));

	// Other UI Components
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/text_HP.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/text_ST.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/text_boss.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/signal_gameStart.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/signal_gameClear.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/text_title.png"));
}

void Player_Interface::Reset()
{
	// 타이틀에서 게임 시작 시 Reset
	soundBoard.clear();
	for (int i = 0; i < SoundCondition::ALL_COUNTS; i++)
		soundBoard.push_back(false);
	soundBoard[SoundCondition::ITEM_CLOSE] = true;

	boss = nullptr;

	menu = MenuState::NORMAL;
	stAlertTime = 0.0f;
	menuOpen = 0;
	menuOpenTime = 0.0f;
	menuMove = 0;
	menuMoveTime = 0.0f;
	menuItemUse = false;
	msgShowTime = 0.0f;

	dying = 0;
	deathTime = FLT_MAX;
	resurrectionMSG = true;

	cinematicLevel = 0;
	cinematicTime = 0.0f;
	cinematicID = 0;
	b_singalForCinematic = false;
	b_cinematicProceed = false;
	b_cinematicEnding = false;

	for (auto it = log_dmg.begin(); it != log_dmg.end(); it++)
		(*it)->Stop();
	while (!queue_msg.empty()) queue_msg.pop();
	startTextTime = FLT_MAX;

	b_missionClear = false;
	clearTime = clearMaxTime;
	clearLogOpacity = 0.0f;

	gameStartState = STATE_TITLE;
	gameStartTime = 0.0f;
}

void Player_Interface::Update()
{
	// HP,ST
	UpdateGuage(bar_hp, player->hp, player->hp_max, 6.0f, false);
	UpdateGuage(bar_st, player->st, player->st_max, 6.0f, false);
	if (boss != nullptr)
	{
		UpdateGuage(bar_bossHp, boss->hp, boss->hp_max, 1143.0f, true);
		UpdateGuage(bar_bossGroggy, dynamic_cast<Monster_Giant*>(boss)->GetGroggy(), dynamic_cast<Monster_Giant*>(boss)->GetGroggyMax(), 1143.0f, true);
		if (dynamic_cast<Monster_Giant*>(boss)->Check_Groggy(true))
		{
			TIMER->GetTick(bossGroggyAlertTime, 0.5f);
			bar_bossGroggy->Find("2_val")->material->diffuse = Util::Quadratic(Color(1, 1, 0, 1), Color(1, 0.5f, 0, 1), Color(1, 1, 0, 1), bossGroggyAlertTime / 0.5f);
		}
		else
		{
			bossGroggyAlertTime = 0.0f;
			bar_bossGroggy->Find("2_val")->material->diffuse = { 1, 1, 0, 1 };
		}
	}
	if (player->GetStRatio() <= 0.3f)
	{
		TIMER->GetTick(stAlertTime, 0.3f);
		bar_st->Find("2_val")->material->diffuse = Util::Quadratic(Color(1, 1, 0, 1), Color(1, 0, 0, 1), Color(1, 1, 0, 1), stAlertTime / 0.3f);
	}
	else
	{
		stAlertTime = 0.0f;
		bar_st->Find("2_val")->material->diffuse = { 1, 1, 0, 1 };
	}
	//소지품창
	{
		float max_t = 0.1f;
		bar_item->Find("text")->visible = (menuOpen != 2 && menuOpenTime <= 0.0f);
		bar_item->Find("bt_key")->visible = (menuOpen != 2 && menuOpenTime <= 0.0f && !INPUT->usePad);
		bar_item->Find("bt_pad")->visible = (menuOpen != 2 && menuOpenTime <= 0.0f && INPUT->usePad);
		bar_item->Find("text")->material->diffuse = (menuOpen == 0) ? Color(0.5f, 0.5f, 0.5f, 1) : Color(1, 1, 1, 1);
		bar_item->Find("bt_key")->material->diffuse = (menuOpen == 0) ? Color(0.5f, 0.5f, 0.5f, 1) : Color(1, 1, 1, 1);
		bar_item->Find("bt_pad")->material->diffuse = (menuOpen == 0) ? Color(0.5f, 0.5f, 0.5f, 1) : Color(1, 1, 1, 1);

		if (INPUT->usePad && bar_item->Find("4_bt_use")->material->diffuseMap == textureResources["bt_Space"])
		{
			dynamic_cast<UI*>(bar_item->Find("4_bt_use"))->UpdateMesh(textureResources["bt_X"]);
			dynamic_cast<UI*>(bar_item->Find("4_bt_left"))->UpdateMesh(textureResources["bt_LB"]);
			dynamic_cast<UI*>(bar_item->Find("4_bt_right"))->UpdateMesh(textureResources["bt_RB"]);
		}
		else if (!INPUT->usePad && bar_item->Find("4_bt_use")->material->diffuseMap == textureResources["bt_X"])
		{
			dynamic_cast<UI*>(bar_item->Find("4_bt_use"))->UpdateMesh(textureResources["bt_Space"]);
			dynamic_cast<UI*>(bar_item->Find("4_bt_left"))->UpdateMesh(textureResources["bt_Q"]);
			dynamic_cast<UI*>(bar_item->Find("4_bt_right"))->UpdateMesh(textureResources["bt_E"]);
		}

		if (menu == MenuState::ITEM)
		{
			SOUND->Play(SDKEY::interface_itemOpen, soundBoard, SoundCondition::ITEM_OPEN, SDTYPE::SE, 1.0f);
			soundBoard[SoundCondition::ITEM_CLOSE] = false;
		}
		else
		{
			if(menu == MenuState::NORMAL)
				SOUND->Play(menuItemUse ? SDKEY::interface_itemUse : SDKEY::interface_itemClose, soundBoard, SoundCondition::ITEM_CLOSE, SDTYPE::SE, 1.0f);
			menuItemUse = false;
			soundBoard[SoundCondition::ITEM_OPEN] = false;
		}

		if (menu == MenuState::INVISIBLE)
			menuOpenTime = 0.0f;
		else if (menu == MenuState::ITEM)
			menuOpenTime = (menuOpenTime >= max_t) ? max_t : menuOpenTime + DELTA_NS;
		else
			menuOpenTime = (menuOpenTime <= 0.0f) ? 0.0f : menuOpenTime - DELTA_NS;
		bar_item->Find("menu")->SetLocalPosY(Util::Lerp(UI::NDCLengthY(220.0f), 0.0f, menuOpenTime / max_t));

		if (menuMove != 0)
		{
			menuMoveTime += DELTA_NS;
			SOUND->Play(SDKEY::interface_itemMove, soundBoard, SoundCondition::ITEM_MOVE, SDTYPE::SE, 1.0f);
			if (menuMoveTime >= max_t)
			{
				soundBoard[SoundCondition::ITEM_MOVE] = false;
				menuMoveTime = 0.0f;
				menuMove = 0;
			}
			bar_item->Find("3_redStroke")->material->diffuse = { 0.5f,0.5f,0.5f,1 };
			bar_item->Find("4_bt_use")->material->diffuse = { 0.5f,0.5f,0.5f,1 };
			bar_item->Find("4_bt_left")->material->diffuse = { 0.5f,0.5f,0.5f,1 };
			bar_item->Find("4_bt_right")->material->diffuse = { 0.5f,0.5f,0.5f,1 };
		}
		else
		{
			bar_item->Find("3_redStroke")->material->diffuse = { 1,1,1,1 };
			bar_item->Find("4_bt_use")->material->diffuse = { 1,1,1,1 };
			bar_item->Find("4_bt_left")->material->diffuse = { 1,1,1,1 };
			bar_item->Find("4_bt_right")->material->diffuse = { 1,1,1,1 };
		}
		for (int i = 0; i <= 6; i++)
		{
			float scTemp;
			float opTemp;
			float origX;
			float fromX;
			float curX;
			bar_item->Find("item" + to_string(i))->parent->visible = false;

			origX = centerPos.x;
			if (fabs(i - 3) >= 1) origX += ((i > 3) ? 1.0f : -1.0f) * (ITSizeITV1 + (fabs(i - 3) - 1) * ITSizeITV2);

			if (menuMove != 0)
			{
				// i가 3일때 중앙을 가리킨다
				if (i == 3)
				{
					scTemp = Util::Lerp(0.7f, 1.0f, menuMoveTime / max_t);
					fromX = origX + ITSizeITV1 * (menuMove > 0 ? 1.0f : -1.0f);
				}
				else if (i == 2 && menuMove > 0)
				{
					// 리스트 우측이동 시 중앙이었던 것
					scTemp = Util::Lerp(1.0f, 0.7f, menuMoveTime / max_t);
					fromX = origX + ITSizeITV1;
				}
				else if (i == 4 && menuMove < 0)
				{
					// 리스트 좌측이동 시 중앙이었던 것
					scTemp = Util::Lerp(1.0f, 0.7f, menuMoveTime / max_t);
					fromX = origX - ITSizeITV1;
				}
				else
				{
					scTemp = 0.7f;
					fromX = origX + ITSizeITV2 * (menuMove > 0 ? 1.0f : -1.0f);
				}
				if ((i == 5 && menuMove > 0) || (i == 1 && menuMove < 0))
					opTemp = Util::Lerp(0.0f, 1.0f, menuMoveTime / max_t);
				else if ((i == 0 && menuMove > 0) || (i == 6 && menuMove < 0))
					opTemp = Util::Lerp(1.0f, 0.0f, menuMoveTime / max_t);
				else if ((i == 6 && menuMove > 0) || (i == 0 && menuMove < 0))
					opTemp = 0.0f;
				else
					opTemp = 1.0f;
				curX = Util::Lerp(fromX, origX, menuMoveTime / max_t);
			}
			else
			{
				opTemp = (fabs(i - 3) >= 3) ? 0.0f : 1.0f;
				scTemp = (i == 3) ? 1.0f : 0.7f;
				curX = origX;
			}
			UI* temp = dynamic_cast<UI*>(bar_item->Find("2_block" + to_string(i)));
			temp->SetNDCPos({ curX,centerPos.y });
			temp->scale = { scTemp, scTemp, 1.0f };

			temp->material->opacity = opTemp;
			for (auto it = temp->children.begin(); it != temp->children.end(); it++)
			{
				it->second->material->opacity = opTemp;
			}
		}

		{
			auto it = player->itemList.begin();
			bar_item->Find("1_text_item")->visible = !player->itemList.empty();
			for (int i = 0; i <= player->current_itemIdx + 3 && it != player->itemList.end(); i++)
			{
				int rel_idx = i - player->current_itemIdx;
				if (rel_idx >= -3 && i < player->itemList.size())
				{
					bar_item->Find("item" + to_string(rel_idx + 3))->material->diffuseMap = ItemManager::itemInfo[it->first].image;
					bar_item->Find("quantity" + to_string(rel_idx + 3) + "_1")->visible = (it->second / 10 != 0);
					bar_item->Find("quantity" + to_string(rel_idx + 3) + "_1")->material->diffuseMap = DmgNumber::numTex[it->second / 10];
					bar_item->Find("quantity" + to_string(rel_idx + 3) + "_2")->material->diffuseMap = DmgNumber::numTex[it->second % 10];
					bar_item->Find("item" + to_string(rel_idx + 3))->parent->visible = true;
				}

				if (rel_idx == 0 && bar_item->Find("1_text_item")->material->diffuseMap != ItemManager::itemInfo[it->first].text)
					dynamic_cast<UI*>(bar_item->Find("1_text_item"))->UpdateMesh(ItemManager::itemInfo[it->first].text);
				it++;
			}
		}

		if ((!menuOpen && menuOpenTime >= max_t) || (menuOpen && menuOpenTime < max_t))
		{
			menuOpen = (menuOpen == 2) ? 0 : 2;
			bar_item->Find("3_redStroke")->visible = menuOpen;
			bar_item->Find("4_bt_use")->visible = menuOpen;
			bar_item->Find("4_bt_left")->visible = menuOpen;
			bar_item->Find("4_bt_right")->visible = menuOpen;
		}
	}

	// 록온마크
	{
		if (player->lockOnTarget && lock_mark->Find("mark")->material->diffuseMap == textureResources["lockoff"])
			dynamic_cast<UI*>(lock_mark->Find("mark"))->UpdateMesh(textureResources["lockon"]);
		else if (!player->lockOnTarget && lock_mark->Find("mark")->material->diffuseMap == textureResources["lockon"])
			dynamic_cast<UI*>(lock_mark->Find("mark"))->UpdateMesh(textureResources["lockoff"]);
	}

	// 메세지 로그
	{
		float max_t = 0.2f;
		if (msgShowTime > 0.0f)
		{
			if (msgShowTime > msgShowTimeMax - max_t)
				log_info->SetLocalPosX(Util::Lerp(UI::NDCLengthX(-msgBarWidth), 0.0f, (msgShowTimeMax - msgShowTime) / max_t));
			else if (msgShowTime > max_t)
				log_info->SetLocalPosX(0.0f);
			else
				log_info->SetLocalPosX(Util::Lerp(UI::NDCLengthX(-msgBarWidth), 0.0f, msgShowTime / max_t));
			msgShowTime -= DELTA_NS;
			log_info->visible = true;
		}
		else
		{
			if (!queue_msg.empty())
			{
				float width_subject = InfoMsg::subjectList[queue_msg.front().subject]->image.GetMetadata().width;
				float width_msg = InfoMsg::msgList[queue_msg.front().msg]->image.GetMetadata().width;
				msgBarWidth = 124.0f + width_subject + width_msg + 15.0f;
				dynamic_cast<UI*>(log_info->Find("3_subject"))->UpdateMesh(InfoMsg::subjectList[queue_msg.front().subject], UI::AlignX::LEFT, UI::AlignY::CENTER);
				dynamic_cast<UI*>(log_info->Find("4_msg"))->UpdateMesh(InfoMsg::msgList[queue_msg.front().msg], UI::AlignX::LEFT, UI::AlignY::CENTER);
				dynamic_cast<UI*>(log_info->Find("4_msg"))->SetNDCPos({ 124.0f + width_subject,msgBarPosY });
				log_info->Find("1_bar")->scale.x = msgBarWidth / DEFAULT_RESOLUTION.x;
				queue_msg.pop();
				SOUND->Play(SDKEY::interface_message, SDTYPE::SE, 1.0f);
				msgShowTime = msgShowTimeMax;
				log_info->SetLocalPosX(UI::NDCLengthX(-msgBarWidth));
				log_info->visible = true;
			}
			else
				log_info->visible = false;
		}
	}

	// 귀환 로그
	{
		if (b_missionClear && clearTime > 0.0f)
		{
			clearLogOpacity += DELTA_NS / clearLogTime;
			clearTime -= DELTA_NS;
			log_clear->visible = true;
			if (clearLogOpacity > 1.0f)
				clearLogOpacity = 1.0f;
			
			log_clear->Find("3_num1")->material->diffuseMap = DmgNumber::numTex[static_cast<int>(floor(ceil(clearTime) / 10.0f))];
			log_clear->Find("4_num2")->material->diffuseMap = DmgNumber::numTex[static_cast<int>(ceil(clearTime)) % 10];
		}
		else
		{
			clearLogOpacity -= DELTA_NS / clearLogTime;
			if (clearLogOpacity < 0.0f)
			{
				clearLogOpacity = 0.0f;
				log_clear->visible = false;
			}
			else
				log_clear->visible = true;
		}
		log_clear->Find("1_bar")->material->opacity = clearLogOpacity;
		log_clear->Find("2_log")->material->opacity = clearLogOpacity;
		log_clear->Find("3_num1")->material->opacity = clearLogOpacity;
		log_clear->Find("4_num2")->material->opacity = clearLogOpacity;
	}

	// 게임시작 텍스트
	{
		float t1 = 0.5f;	// 텍스트를 페이드하는 시간
		float t2 = 1.5f;	// 텍스트를 정지하는 시간
		float max_t = t2 + t1 * 2.0f;	// 전체시간
		float width = text_gameStart->material->diffuseMap->image.GetMetadata().width;
		if (startTextTime < t1)
			text_gameStart->SetLocalPosX(Util::Lerp(-1.0f - UI::NDCLengthX(width), 0.0f, startTextTime / t1));
		else if (startTextTime < t1 + t2)
			text_gameStart->SetLocalPosX(0.0f);
		else if (startTextTime < max_t)
			text_gameStart->SetLocalPosX(Util::Lerp(0.0f, 1.0f + UI::NDCLengthX(width), (startTextTime - t1 - t2) / t1));

		if (startTextTime < max_t)
		{
			startTextTime += DELTA_NS;
			SOUND->Play(SDKEY::mission_start, soundBoard, SoundCondition::MISSION_START, SDTYPE::SE, 1.0f);
			if (!text_gameStart->visible) 
				text_gameStart->visible = true;
		}
		else
		{
			if (soundBoard[SoundCondition::MISSION_START])
				soundBoard[SoundCondition::MISSION_START] = false;
			if (text_gameStart->visible)
				text_gameStart->visible = false;
		}
	}

	// 타이틀 텍스트
	if (gameStartState == STATE_TITLE)
	{
		text_title->visible = true;
	}
	else
		text_title->visible = false;

	// 게임클리어 텍스트
	if (IsGameEnding())
	{
		text_gameClear->visible = true;
		SOUND->Play(SDKEY::mission_clear, soundBoard, SoundCondition::MISSION_CLEAR, SDTYPE::SE, 1.0f);
	}
	else
	{
		text_gameClear->visible = false;
		if (soundBoard[SoundCondition::MISSION_CLEAR])
			soundBoard[SoundCondition::MISSION_CLEAR] = false;
	}

	bar_hp->Update();
	bar_st->Update();
	bar_bossHp->Update();
	bar_bossGroggy->Update();
	bar_item->Update();
	log_info->Update();
	log_clear->Update();
	text_gameStart->Update();
	text_gameClear->Update();
	text_title->Update();
	
	//log_dmg.front()->root->RenderHierarchy();

	//static int testdmg = 0;
	//static float testcrit = 1.0f;
	//ImGui::InputInt("dmg",&testdmg);
	//ImGui::InputFloat("crit", &testcrit);
	//if (ImGui::Button("DmgPlay"))
	//{
	//	ShowDamage({ RANDOM->Float(-0.5f,0.5f),RANDOM->Float(-0.5f,0.5f),0 }, testdmg, testcrit);
	//}
	for (auto it = log_dmg.begin(); it != log_dmg.end(); it++)
		(*it)->Update();
}

void Player_Interface::LateUpdate()
{
	const float fov_valueDefault = 60.0f * TORADIAN;
	lock_mark->Update();

	bool b_normalBG = true;	// 지금 프레임이 페이드가 적용되지 않은 화면상태?

	switch (cinematicLevel)
	{
	case 0:	// 인게임
	{
		float t1 = 0.5f;
		if (b_cinematicEnding && cinematicTime < t1)
		{
			b_normalBG = false;
			SCENE->postEffect->blur._Color = Util::Lerp(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.5f, 0.5f, 0.5f), cinematicTime / t1);
			cinematicTime += DELTA_NS;
		}
	}
		break;
	case 1: // 시네마틱 진입 (페이드)
	{
		float t1 = 0.5f;
		float t2 = 1.0f;
		if (soundBoard[SoundCondition::CINEMATIC_FADEIN] == false)
		{
			SOUND->FadeVolumeType(SDTYPE::BGM, 0.0f, t2 + 0.5f);
			SOUND->FadeVolumeType(SDTYPE::SE, 0.0f, t2);
			SOUND->FadeVolumeType(SDTYPE::ENV, 0.5f, t2);
			soundBoard[SoundCondition::CINEMATIC_FADEIN] = true;
		}
		if (cinematicTime < t1)
		{
			b_normalBG = false;
			SCENE->postEffect->blur._Color = Util::Lerp(Vector3(0.5f, 0.5f, 0.5f), Vector3(0.0f, 0.0f, 0.0f), cinematicTime / t1);
			cinematicTime += DELTA_NS;
		}
		else if (cinematicTime < t2)
		{
			b_normalBG = false;
			SCENE->postEffect->blur._Color = { 0.0f, 0.0f, 0.0f };
			cinematicTime += DELTA_NS;
		}
		else
		{
			b_normalBG = false;
			SCENE->postEffect->blur._Filter = 0;
			SCENE->postEffect->blur._Select = 0;
			SCENE->postEffect->blur._Count = 1;
			SCENE->postEffect->blur._Color = { 0.0f, 0.0f, 0.0f };
			b_cinematicProceed = false;
			b_singalForCinematic = true;
			b_cinematicEnding = false;

			// 지정한 시네마틱ID에 따라 정해진 컷신 실행 (0은 미지정)
			bool b_ready = false;
			switch (cinematicID)
			{
			case 1:
				if (GET_PLAYER->GetActor()->anim->PlayingIdx() == Player::PlayerAction::CINEMATIC_001_01)
					b_ready = true;
				break;
			}
			if (b_ready)
				SetCinematicLevel(2);
		}
	}
		break;
	case 2:	// 시네마틱 진행
	{
		// 시네마틱 스타트
		if (!b_cinematicProceed)
		{
			b_cinematicProceed = true;
			GAMEOBJ->eventTarget = nullptr;
			SOUND->FadeVolumeType(SDTYPE::SE, 1.0f, 0.5f);
			SOUND->FadeVolumeType(SDTYPE::ENV, 1.0f, 0.5f);

			switch (cinematicID)
			{
			case 1:
				// 배역 할당
				sceneActors = { GET_PLAYER, GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GIANT, 0) };
				centerActor = 0;

				// 스타트 설정
				sceneActors[0]->GetActor()->anim->PlayCurAnimation(AnimationState::ONCE);
				sceneActors[0]->Spawn((*SCENE->Map)[0]->Find("PlayerSpawn0"));
				sceneActors[0]->isAir = false;
				sceneActors[1]->Spawn((*SCENE->Map)[0]->Find("BossSpawn0"));
				sceneActors[1]->Enable = true;
				sceneActors[1]->GetActor()->visible = false;
				sceneActors[1]->ChangeAnim(AnimationState::STOP, Monster_Giant::AniAction::CINEMATIC_001_08, 0.0f, true);
				GAMEOBJ->eventTarget = sceneActors[1];
				//sceneActors[1]->Appear(true);
				break;
			}
		}

		// 시네마틱 페이드
		float t1 = 0.5f;	// 페이드 인 타임
		float t2 = 1.0f;	// 페이드 아웃 타임
		if (!b_cinematicEnding)
		{
			// 페이드 인
			if (cinematicTime < t1)
			{
				b_normalBG = false;
				SCENE->postEffect->blur._Color = Util::Lerp(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.5f, 0.5f, 0.5f), cinematicTime / t1);
				cinematicTime += DELTA;
			}
			else
			{
				// 페이드아웃 조건
				if (INPUT->KeyDown(VK_ESCAPE, false) || INPUT->tracker.menu == GamePad::ButtonStateTracker::ButtonState::PRESSED)
				{
					b_cinematicEnding = true;
					cinematicTime = 0.0f;
				}
				else
				{
					switch (cinematicID)
					{
					case 1:
					{
						// fps 30 기준
						float fadeFrame = 30.0f;
						float maxFrame = 165.0f;
						if (sceneActors[1]->GetActor()->anim->PlayingIdx() == Monster_Giant::AniAction::CINEMATIC_001_13 &&
							sceneActors[1]->GetActor()->anim->GetPlayTime() >= (maxFrame - fadeFrame) / maxFrame)
						{
							b_cinematicEnding = true;
							cinematicTime = 0.0f;
						}
					}
					break;
					}
				}
			}
		}
		else
		{
			// 페이드 아웃
			cinematicTime += DELTA;
			if (cinematicTime < t2)
			{
				b_normalBG = false;
				SCENE->postEffect->blur._Color = Util::Lerp(Vector3(0.5f, 0.5f, 0.5f), Vector3(0.0f, 0.0f, 0.0f), cinematicTime / t2);
			}
			else
			{
				b_normalBG = false;
				SCENE->postEffect->blur._Color = { 0.0f, 0.0f, 0.0f };
				SetCinematicLevel(3);
			}
		}

		//시네마틱 진행 중
		switch (cinematicID)
		{
		case 1:
			if(centerActor == 0)
				CameraControl::SetCamState(Camera::CameraState::PLAYER_EVENT);
			else
				CameraControl::SetCamState(Camera::CameraState::OTHER_EVENT);

			// 진행중 연출
			if (sceneActors[0]->GetActor()->anim->PlayingIdx() == Player::PlayerAction::CINEMATIC_001_11)
			{
				float maxFrame = 60.0f;
				SCENE->postEffect->blur._Filter = 0;
				SCENE->postEffect->blur._Select = 2;
				SCENE->postEffect->blur._Count = static_cast<int>(Util::Lerp(3, 0, sceneActors[0]->GetActor()->anim->GetPlayTime()));
				if (SCENE->postEffect->blur._Count <= 0)
				{
					SCENE->postEffect->blur._Select = 0;
					SCENE->postEffect->blur._Count = 1;
				}
			}
			else
			{
				SCENE->postEffect->blur._Filter = 0;
				SCENE->postEffect->blur._Select = 0;
				SCENE->postEffect->blur._Count = 1;
			}
			if (soundBoard[SoundCondition::CINEMATIC_BGM_CHANGE] == false &&
				sceneActors[1]->GetActor()->anim->PlayingIdx() == Monster_Giant::AniAction::CINEMATIC_001_12 &&
				sceneActors[1]->GetActor()->anim->GetPlayTime() >= 12.0f / 60.0f)
			{
				SOUND->StopSoundType(SDTYPE::BGM);
				SOUND->SetVolumeType(SDTYPE::BGM, 1.0f);
				SOUND->Play(SDKEY::BGM01, soundBoard, SoundCondition::CINEMATIC_BGM_CHANGE, SDTYPE::BGM, 0.2f);
			}

			// 플레이어중심 컷신전환
			if (centerActor == 0 && sceneActors[0]->GetActor()->anim->GetPlayTime() >= 1.0f)
			{
				switch (sceneActors[0]->GetActor()->anim->PlayingIdx())
				{
				case Player::PlayerAction::CINEMATIC_001_01: sceneActors[0]->ChangeAnim(AnimationState::ONCE, Player::PlayerAction::CINEMATIC_001_02, 0.0f, true); break;
				case Player::PlayerAction::CINEMATIC_001_02: sceneActors[0]->ChangeAnim(AnimationState::ONCE, Player::PlayerAction::CINEMATIC_001_03, 0.0f, true); break;
				case Player::PlayerAction::CINEMATIC_001_03: sceneActors[0]->ChangeAnim(AnimationState::ONCE, Player::PlayerAction::CINEMATIC_001_04, 0.0f, true); break;
				case Player::PlayerAction::CINEMATIC_001_04: sceneActors[0]->ChangeAnim(AnimationState::ONCE, Player::PlayerAction::CINEMATIC_001_05, 0.0f, true); break;
				case Player::PlayerAction::CINEMATIC_001_05: sceneActors[0]->ChangeAnim(AnimationState::ONCE, Player::PlayerAction::CINEMATIC_001_06, 0.0f, true); break;
				case Player::PlayerAction::CINEMATIC_001_06: 
					sceneActors[0]->ChangeAnim(AnimationState::ONCE, Player::PlayerAction::CINEMATIC_001_07, 0.0f, true);
					sceneActors[1]->GetActor()->visible = true;
					break;
				case Player::PlayerAction::CINEMATIC_001_07: 
					sceneActors[1]->ChangeAnim(AnimationState::ONCE, Monster_Giant::AniAction::CINEMATIC_001_08, 0.0f, true);
					centerActor = 1;
					break;
				case Player::PlayerAction::CINEMATIC_001_09:
					sceneActors[1]->ChangeAnim(AnimationState::ONCE, Monster_Giant::AniAction::CINEMATIC_001_10, 0.0f, true);
					centerActor = 1;
					break;
				case Player::PlayerAction::CINEMATIC_001_11:
					sceneActors[0]->ChangeAnim(AnimationState::ONCE, Player::PlayerAction::CINEMATIC_001_12, 0.0f, true);
					sceneActors[1]->ChangeAnim(AnimationState::ONCE, Monster_Giant::AniAction::CINEMATIC_001_12, 0.0f, true);
					break;
				case Player::PlayerAction::CINEMATIC_001_12:
					sceneActors[0]->ChangeAnim(AnimationState::ONCE, Player::PlayerAction::CINEMATIC_001_13, 0.0f, true);
					sceneActors[1]->ChangeAnim(AnimationState::ONCE, Monster_Giant::AniAction::CINEMATIC_001_13, 0.0f, true);
					centerActor = 1;
					break;
				}
			}

			// 보스중심 컷신전환
			if (centerActor == 1 && sceneActors[1]->GetActor()->anim->GetPlayTime() >= 1.0f)
			{
				switch (sceneActors[1]->GetActor()->anim->PlayingIdx())
				{
				case Monster_Giant::AniAction::CINEMATIC_001_08:
					sceneActors[0]->ChangeAnim(AnimationState::ONCE, Player::PlayerAction::CINEMATIC_001_09, 0.0f, true);
					centerActor = 0;
					break;
				case Monster_Giant::AniAction::CINEMATIC_001_10:
					sceneActors[0]->ChangeAnim(AnimationState::ONCE, Player::PlayerAction::CINEMATIC_001_11, 0.0f, true);
					centerActor = 0;
					break;
				}
			}
			break;
		}
	}
		break;
	case 3:
	{
		// 시네마틱 마무리 (인게임으로 돌아가기 위한 작업)
		b_normalBG = false;
		SCENE->postEffect->blur._Filter = 0;
		SCENE->postEffect->blur._Select = 0;
		SCENE->postEffect->blur._Count = 1;
		SCENE->postEffect->blur._Color = { 0.0f, 0.0f, 0.0f };

		float t1 = 1.0f;
		if (cinematicTime >= t1)
		{
			switch (cinematicID)
			{
			case 1:
				sceneActors[0]->ChangeAnim(AnimationState::ONCE, Player::PlayerAction::IDLE, 0.0f, true);
				sceneActors[1]->Spawn((*SCENE->Map)[0]->Find("BossSpawn1"));
				sceneActors[1]->ChangeAnim(AnimationState::ONCE, Monster_Giant::AniAction::IDLE, 0.0f, true);
				GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GIANT, 0)->Appear(true, true);

				if (soundBoard[SoundCondition::CINEMATIC_BGM_CHANGE] == false)
				{
					INPUT->ResetVibration();
					SOUND->StopSoundType(SDTYPE::SE);
					SOUND->StopSoundType(SDTYPE::BGM);
					SOUND->SetVolumeType(SDTYPE::BGM, 1.0f);
					SOUND->Play(SDKEY::BGM01, soundBoard, SoundCondition::CINEMATIC_BGM_CHANGE, SDTYPE::BGM, 0.2f);
				}
				break;
			}
			CloseCinematic();
		}
		else
			cinematicTime += DELTA_NS;
	}
		break;
	}

	switch (dying)
	{
	case 0:
	{
		float t1 = 0.5f;
		float t2 = 1.0f;
		if (deathTime < t1)
		{
			b_normalBG = false;
			SCENE->postEffect->blur._Filter = 0;
			SCENE->postEffect->blur._Select = 0;
			SCENE->postEffect->blur._Count = 1;
			SCENE->postEffect->blur._Color = { 0.0f,0.0f,0.0f };
			App.deltaScale = 1.0f;
			Camera::GetFollowCam()->fov = 60.0f * TORADIAN;
			deathTime += DELTA_NS;
		}
		else if (deathTime < t2)
		{
			b_normalBG = false;
			if (!resurrectionMSG)
			{
				PLAYER_UI->PushMessage(InfoMsg::Subject::CheckPoint, InfoMsg::MSG::Resurrection);
				resurrectionMSG = true;
			}
			SCENE->postEffect->blur._Filter = 0;
			SCENE->postEffect->blur._Select = 0;
			SCENE->postEffect->blur._Count = 1;
			SCENE->postEffect->blur._Color = Util::Lerp(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.5f, 0.5f, 0.5f), (deathTime - t1) / (t2 - t1));
			App.deltaScale = 1.0f;
			Camera::GetFollowCam()->fov = 60.0f * TORADIAN;
			deathTime += DELTA_NS;
		}
		else
		{
			resurrectionMSG = false;
		}
	}
		break;
	case 1:
	{
		float t1 = 0.1f;
		float t2 = 1.5f;
		float fov_value = fov_valueDefault;

		// 사운드 출력
		if (deathTime >= 0.05f && soundBoard[SoundCondition::DIE] == false)
		{
			SOUND->FadeAllVolumeType(0.0f, 0.25f, 0.0f, 0.1f);
			SOUND->Play(SDKEY::death_impact, soundBoard, SoundCondition::DIE, SDTYPE::SE, 1.0f, ChannelFlags_Independent);
		}
		if (deathTime >= t2 && soundBoard[SoundCondition::DIE] == true &&
			GET_PLAYER->hp > 0.0f && !GET_PLAYER->Check_Death() && soundBoard[SoundCondition::SALVATION] == false)
		{
			SOUND->FadeAllVolumeType(1.0f, 1.0f, 1.0f, 0.3f);
			soundBoard[SoundCondition::SALVATION] = true;
		}

		// 화면효과
		if (deathTime < t1)
		{
			b_normalBG = false;
			SCENE->postEffect->blur._Select = 3;
			SCENE->postEffect->blur._Count = Util::Lerp(1, 30, deathTime / t1);
			SCENE->postEffect->blur._Color = Util::Lerp(Vector3(0.5f, 0.5f, 0.5f), Vector3(0.6f, 0.4f, 0.4f), deathTime / t1);
			//SCENE->postEffect->blur._Color = { 0.5f, 0.5f, 0.5f };
			App.deltaScale = Util::Lerp(1.0f, 0.1f, pow(deathTime / t1, 2));
			fov_value = Util::Lerp(60.0f, 30.0f, deathTime / t1) * TORADIAN;
		}
		else if (deathTime < t2)
		{
			b_normalBG = false;
			float temp_t = 0.1f;
			SCENE->postEffect->blur._Filter = 1;
			SCENE->postEffect->blur._Select = 3;
			SCENE->postEffect->blur._Count = Util::Lerp(30, 1, (deathTime - t1) / (t2 - t1));
			SCENE->postEffect->blur._Color = (deathTime - t1 < temp_t) ? Util::Lerp(Vector3(0.6f, 0.4f, 0.4f), Vector3(0.5f, 0.5f, 0.5f), (deathTime - t1) / temp_t) : Vector3(0.5f, 0.5f, 0.5f);
			//SCENE->postEffect->blur._Color = { 0.5f, 0.5f, 0.5f };
			App.deltaScale = Util::Lerp(0.1f, 0.3f, (deathTime - t1) / (t2 - t1));
			fov_value = Util::Lerp(30.0f, 40.0f, (deathTime - t1) / (t2 - t1)) * TORADIAN;
		}
		else if (GET_PLAYER->hp > 0.0f && !GET_PLAYER->Check_Death())
		{
			SCENE->postEffect->blur._Filter = 1;
			SCENE->postEffect->blur._Select = 0;
			SCENE->postEffect->blur._Count = 1;
			App.deltaScale = 1.0f;
			fov_value += (fov_valueDefault / 3.0f) * TORADIAN * DELTA;
			if (fov_value >= fov_valueDefault)
			{
				Camera::GetFollowCam()->fov = fov_valueDefault;
				SCENE->postEffect->blur._Filter = 0;

				dying = 0;
				deathTime = FLT_MAX;
				resurrectionMSG = true;
				soundBoard[SoundCondition::DIE] = false;
				soundBoard[SoundCondition::SALVATION] = false;
				break;
			}
		}
		else
		{
			SCENE->postEffect->blur._Filter = 1;
			SCENE->postEffect->blur._Select = 0;
			SCENE->postEffect->blur._Count = 1;
			//SCENE->postEffect->blur._Color = { 0.5f, 0.5f, 0.5f };
			App.deltaScale = 1.0f;
			fov_value = 40.0f * TORADIAN;
		}
		Camera::GetFollowCam()->fov = fov_value;
	}
		deathTime += DELTA_NS;
		break;
	case 2:
		float max_t = 1.0f;
		if (soundBoard[SoundCondition::DIE] == false)
		{
			SOUND->SetAllVolumeType(0.0f, 0.25f, 0.0f);
			soundBoard[SoundCondition::DIE] = true;
		}

		float fadeFrame = 60.0f;
		float maxFrame;
		SCENE->postEffect->blur._Filter = 1;
		SCENE->postEffect->blur._Select = 0;
		SCENE->postEffect->blur._Count = 1;
		Camera::GetFollowCam()->fov = 60.0f * TORADIAN;
		App.deltaScale = 1.0f;
		if (GET_PLAYER->GetActor()->anim->PlayingIdx() == Player::PlayerAction::DIE)
			maxFrame = 300.0f;
		else if (GET_PLAYER->GetActor()->anim->PlayingIdx() == Player::PlayerAction::DIE_LAND)
			maxFrame = 240.0f;
		else
		{
			Camera::GetFollowCam()->fov = fov_valueDefault;
			SCENE->postEffect->blur._Filter = 0;

			dying = 0;
			deathTime = FLT_MAX;
			resurrectionMSG = true;
			soundBoard[SoundCondition::DIE] = false;
			soundBoard[SoundCondition::SALVATION] = false;
			SOUND->SetAllVolumeType(1.0f, 1.0f, 1.0f);
			break;
		}

		if (GET_PLAYER->GetActor()->anim->GetPlayTime() >= 1.0f)
		{
			b_normalBG = false;
			SCENE->postEffect->blur._Color = { 0.0f, 0.0f, 0.0f };
			soundBoard[SoundCondition::DIE] = false;
			soundBoard[SoundCondition::SALVATION] = false;
			SOUND->FadeAllVolumeType(1.0f, 1.0f, 1.0f, 1.0f);
			GET_PLAYER->Resurrection();
		}
		else if (GET_PLAYER->GetActor()->anim->GetPlayTime() >= (maxFrame - fadeFrame) / maxFrame)
		{
			b_normalBG = false;
			SCENE->postEffect->blur._Color = Util::Lerp(Vector3(0.5f, 0.5f, 0.5f), Vector3(0.0f, 0.0f, 0.0f), (GET_PLAYER->GetActor()->anim->GetPlayTime() * maxFrame - (maxFrame - fadeFrame)) / fadeFrame);
		}
		break;
	}

	if (gameStartState == STATE_STARTING_GAME)
	{
		gameStartTime += DELTA;

		const float t1 = 1.0f;
		const float t2 = 4.0f;
		const float t3 = 4.2f;
		if (gameStartTime < t1)
		{
			soundBoard[SoundCondition::TITLE_TRANSITION] = false;
		}
		else if (gameStartTime < t2)
		{
			SOUND->Play(SDKEY::misc06, soundBoard, SoundCondition::TITLE_TRANSITION, SDTYPE::SE, 1.0f);
			b_normalBG = false;
			SCENE->postEffect->blur._Color = Util::Lerp(Vector3(0.5f, 0.5f, 0.5f), Vector3(1.0f, 1.0f, 1.0f), (gameStartTime - t1) / (t2 - t1));
		}
		else if (gameStartTime < t3)
		{
			b_normalBG = false;
			SCENE->postEffect->blur._Color = { 1.0f, 1.0f, 1.0f };
		}
		else
			gameStartState = STATE_START_GAME;
	}
	else if (gameStartState == STATE_START_GAME)
		gameStartState = STATE_IN_GAME;

	if (b_normalBG)
		SCENE->postEffect->blur._Color = { 0.5f, 0.5f, 0.5f };
}

void Player_Interface::DebugRender()
{
	bar_hp->RenderHierarchy();
	bar_st->RenderHierarchy();
	bar_bossHp->RenderHierarchy();
	bar_bossGroggy->RenderHierarchy();
	bar_item->RenderHierarchy();
	log_info->RenderHierarchy();
	log_clear->RenderHierarchy();
	text_gameStart->RenderHierarchy();
	text_gameClear->RenderHierarchy();
	text_title->RenderHierarchy();
}

void Player_Interface::Render()
{
	BLEND->Set(true);
	if (gameStartState == STATE_TITLE)
	{
		text_title->Render();
	}
	else if (gameStartState >= STATE_START_GAME && cinematicLevel <= 1 && CONFIG->settings.b_HUDVisible)
	{
		// 시네마틱 진행중일때 숨김
		if (dying <= 1)
		{
			// 리타이어씬이 진행중일때 숨김
			for (auto it = log_dmg.begin(); it != log_dmg.end(); it++)
				(*it)->Render();	// deque의 begin은 back부터 참조한다.


			bar_hp->Render();
			bar_st->Render();
			if (boss != nullptr)
			{
				bar_bossHp->Render();
				bar_bossGroggy->Render();
			}
			if (menu != MenuState::INVISIBLE)
				bar_item->Render();
			lock_mark->Render();
		}
		text_gameStart->Render();
		text_gameClear->Render();
		log_info->Render();
		log_clear->Render();
	}
	BLEND->Set(false);
}

void Player_Interface::ShowDamage(Vector3 pos, int value, DmgNumber::DmgType type)
{
	DmgNumber* temp = log_dmg.front();
	log_dmg.pop_front();
	log_dmg.emplace_back(temp);
	pos += {
		RANDOM->Float(-1.0f, 1.0f), 
		RANDOM->Float(-1.0f, 1.0f), 
		RANDOM->Float(-1.0f, 1.0f)
	};
	log_dmg.back()->Play(pos, value, type);
}

void Player_Interface::PushMessage(InfoMsg::Subject subject, InfoMsg::MSG msg)
{
	queue_msg.push(InfoMsg::InfoText(subject, msg));
}

void Player_Interface::MakeGuage(Actor* parent, float val, float maxVal, Vector2 NDCPos, Vector2 Size, Color color, float stroke, bool relative)
{
	// Size의 정의
	// relative(true): 게이지 전체의 크기
	// relative(false): 수치 1당 게이지의 크기
	UI* temp;

	float x = NDCPos.x;
	float y = NDCPos.y;
	float w = Size.x;
	float h = Size.y;

	temp = UI::Create("1_maxVal");
	temp->CreateMesh("UI/pixel.png", UI::AlignX::LEFT, UI::AlignY::TOP);
	temp->material->diffuse = { 0.2f,0.2f,0.2f,1 };
	temp->SetNDCPos({ x - (stroke / 2.0f), y - (stroke / 2.0f) });
	temp->scale = {
		(relative ? w + stroke : w * maxVal + stroke), 
		h + stroke,
		1.0f 
	};
	parent->AddChild(temp);
	
	temp = UI::Create("2_val");
	temp->CreateMesh("UI/guage.png", UI::AlignX::LEFT, UI::AlignY::TOP);
	temp->material->diffuse = color;
	temp->SetNDCPos({ x, y });
	temp->scale = {
		(relative ? w * (val / maxVal) : w * val),
		h / 100.0f,
		1.0f
	};
	parent->AddChild(temp);

	temp = UI::Create("3_strokeU");
	temp->CreateMesh("UI/pixel.png", UI::AlignX::LEFT, UI::AlignY::TOP);
	temp->material->diffuse = { 0,0,0,1 };
	temp->SetNDCPos({ x - stroke, y - stroke });
	temp->scale = {
		(relative ? w + (stroke * 2.0f) : w * maxVal + (stroke * 2.0f)),
		stroke,
		1.0f
	};
	parent->AddChild(temp);

	temp = UI::Create("3_strokeD");
	temp->CreateMesh("UI/pixel.png", UI::AlignX::LEFT, UI::AlignY::BOTTOM);
	temp->material->diffuse = { 0,0,0,1 };
	temp->SetNDCPos({ x - stroke, y + h + stroke });
	temp->scale = { 
		(relative ? w + (stroke * 2.0f) : w * maxVal + (stroke * 2.0f)),
		stroke,
		1.0f 
	};
	parent->AddChild(temp);

	temp = UI::Create("3_strokeL");
	temp->CreateMesh("UI/pixel.png", UI::AlignX::LEFT, UI::AlignY::TOP);
	temp->material->diffuse = { 0,0,0,1 };
	temp->SetNDCPos({ x - stroke, y - stroke });
	temp->scale = { stroke, h + (stroke * 2.0f), 1.0f };
	parent->AddChild(temp);

	temp = UI::Create("3_strokeR");
	temp->CreateMesh("UI/pixel.png", UI::AlignX::RIGHT, UI::AlignY::TOP);
	temp->material->diffuse = { 0,0,0,1 };
	temp->SetNDCPos({ (relative ? x + w + stroke : x + w * maxVal + stroke), y - stroke });
	temp->scale = { stroke, h + (stroke * 2.0f), 1.0f };
	parent->AddChild(temp);
}

void Player_Interface::MakeItemBar(Actor* parent)
{
	UI* temp;
	UI* temp2;
	Object* tempMenu;

	temp = UI::Create("text");
	temp->CreateMesh("UI/text_item.png");
	temp->SetNDCPos({ 1795.5f,1030.0f });
	parent->AddChild(temp);

	temp = UI::Create("bt_key");
	temp->CreateMesh("UI/Button_Tab.png");
	temp->SetNDCPos({ 1860.0f,1030.0f });
	parent->AddChild(temp);

	temp = UI::Create("bt_pad");
	temp->CreateMesh("UI/Button_View.png");
	temp->SetNDCPos({ 1860.0f,1022.0f });
	parent->AddChild(temp);

	// 메뉴생성
	tempMenu = Object::Create("menu");
	parent->AddChild(tempMenu);

	temp = UI::Create("0_bar");
	temp->CreateMesh("UI/itemBar.png");
	temp->SetNDCPos({ 1662.0f,1022.0f });
	tempMenu->AddChild(temp);

	temp = UI::Create("1_text_item");
	temp->CreateMesh("UI/pixel.png");
	temp->SetNDCPos({ 1662.0f,1022.0f });
	tempMenu->AddChild(temp);

	// 아이템블록 생성
	for (int i = 0; i <= 6; i++)
	{
		temp = UI::Create("2_block" + to_string(i));
		temp->CreateMesh("UI/item_block.png", UI::AlignX::CENTER, UI::AlignY::BOTTOM);
		switch (i)
		{
		case 0: temp->SetNDCPos(centerPos + Vector2(-ITSizeITV1 - ITSizeITV2 * 2.0f, 0.0f)); break;
		case 1: temp->SetNDCPos(centerPos + Vector2(-ITSizeITV1 - ITSizeITV2, 0.0f));		 break;
		case 2: temp->SetNDCPos(centerPos + Vector2(-ITSizeITV1, 0.0f)); break;
		case 3: temp->SetNDCPos(centerPos); break;
		case 4: temp->SetNDCPos(centerPos + Vector2(ITSizeITV1, 0.0f)); break;
		case 5: temp->SetNDCPos(centerPos + Vector2(ITSizeITV1 + ITSizeITV2, 0.0f)); break;
		case 6: temp->SetNDCPos(centerPos + Vector2(ITSizeITV1 + ITSizeITV2 * 2.0f, 0.0f)); break;
		}
		
		temp->scale = { 0.7f,0.7f,1.0f };
		tempMenu->AddChild(temp);
		temp2 = UI::Create("item" + to_string(i));
		temp2->CreateMesh(ItemManager::itemInfo.begin()->second.image, UI::AlignX::CENTER, UI::AlignY::BOTTOM);
		temp->AddChild(temp2);
		temp2 = UI::Create("quantity" + to_string(i) + "_1");
		temp2->CreateMesh(DmgNumber::numTex[0], UI::AlignX::RIGHT, UI::AlignY::BOTTOM);
		temp2->SetLocalPos({ UI::NDCLengthX(44.0f - DmgNumber::numTex[0]->image.GetMetadata().width), UI::NDCLengthY(-6.0f),0.0f });
		temp->AddChild(temp2);
		temp2 = UI::Create("quantity" + to_string(i) + "_2");
		temp2->CreateMesh(DmgNumber::numTex[0], UI::AlignX::RIGHT, UI::AlignY::BOTTOM);
		temp2->SetLocalPos({ UI::NDCLengthX(44.0f), UI::NDCLengthY(-6.0f),0.0f });
		temp->AddChild(temp2);
		temp->visible = false;
	}

	temp = UI::Create("3_redStroke");
	temp->CreateMesh("UI/item_blockSelect.png", UI::AlignX::CENTER, UI::AlignY::BOTTOM);
	temp->SetNDCPos(centerPos);
	tempMenu->AddChild(temp);

	temp = UI::Create("4_bt_use");
	temp->CreateMesh(textureResources["bt_Space"]);
	temp->SetNDCPos(centerPos + Vector2(50.0f, -100.0f));
	tempMenu->AddChild(temp);

	temp = UI::Create("4_bt_left");
	temp->CreateMesh(textureResources["bt_Q"]);
	temp->SetNDCPos(centerPos + Vector2(-ITSizeITV1 - 35.0f, -70.0f));
	tempMenu->AddChild(temp);

	temp = UI::Create("4_bt_right");
	temp->CreateMesh(textureResources["bt_E"]);
	temp->SetNDCPos(centerPos + Vector2(ITSizeITV1 + 35.0f, -70.0f));
	tempMenu->AddChild(temp);
}

void Player_Interface::MakeInfoBar(Actor* parent)
{
	UI* temp;

	temp = UI::Create("1_bar");
	temp->CreateMesh("UI/Info/bar.png", UI::AlignX::LEFT, UI::AlignY::CENTER);
	temp->SetNDCPos({ 0.0f,msgBarPosY });
	temp->scale = { (1.0f / DEFAULT_RESOLUTION.x),1.0f,1.0f };
	parent->AddChild(temp);

	temp = UI::Create("2_header");
	temp->CreateMesh("UI/Info/header.png", UI::AlignX::LEFT, UI::AlignY::CENTER);
	temp->SetNDCPos({ 16.0f,msgBarPosY });
	parent->AddChild(temp);

	temp = UI::Create("3_subject");
	temp->CreateMesh(InfoMsg::subjectList[InfoMsg::Subject::Goblin], UI::AlignX::LEFT, UI::AlignY::CENTER);
	temp->SetNDCPos({ 124.0f,msgBarPosY });
	parent->AddChild(temp);

	temp = UI::Create("4_msg");
	temp->CreateMesh(InfoMsg::msgList[InfoMsg::MSG::Kill], UI::AlignX::LEFT, UI::AlignY::CENTER);
	temp->SetNDCPos({ 124.0f,msgBarPosY });
	parent->AddChild(temp);
}

void Player_Interface::MakeClearBar(Actor* parent)
{
	UI* temp;

	temp = UI::Create("1_bar");
	temp->CreateMesh("UI/Info/bar.png", UI::AlignX::LEFT, UI::AlignY::CENTER);
	temp->SetNDCPos({ 0.0f,clearBarPosY });
	temp->scale = { (550.0f / DEFAULT_RESOLUTION.x),1.0f,1.0f };
	parent->AddChild(temp);

	temp = UI::Create("2_log");
	temp->CreateMesh("UI/Info/text_clearTime.png", UI::AlignX::LEFT, UI::AlignY::CENTER);
	temp->SetNDCPos({ 35.0f,clearBarPosY });
	parent->AddChild(temp);

	temp = UI::Create("3_num1");
	temp->CreateMesh(DmgNumber::numTex[0], UI::AlignX::LEFT, UI::AlignY::CENTER);
	temp->SetNDCPos({ 300.0f,clearBarPosY });
	temp->material->diffuse = { 1.0f,0.5f,0.5f,1.0f };
	temp->scale = { 1.2f,1.2f,1.0f };
	parent->AddChild(temp);

	temp = UI::Create("4_num2");
	temp->CreateMesh(DmgNumber::numTex[0], UI::AlignX::LEFT, UI::AlignY::CENTER);
	temp->SetNDCPos({ 325.0f,clearBarPosY });
	temp->material->diffuse = { 1.0f,0.5f,0.5f,1.0f };
	temp->scale = { 1.2f,1.2f,1.0f };
	parent->AddChild(temp);
}

void Player_Interface::UpdateGuage(Actor* parent, float val, float maxVal, float width, bool relative)
{
	// width의 정의
	// relative(true): 게이지 전체의 너비
	// relative(false): 수치 1당 게이지의 너비
	parent->Find("2_val")->scale.x = relative ? width * (val / maxVal) : width * val;
}

void Player_Interface::SetCinematicLevel(int value)
{
	if (cinematicLevel != value) 
	{
		cinematicLevel = value;
		cinematicTime = 0.0f;
		b_singalForCinematic = false;
		if (value >= 2)
			menu = MenuState::INVISIBLE;
	}
}

void Player_Interface::OpenCinematic(int id)
{
	cinematicLevel = 1;
	cinematicTime = 0.0f;
	cinematicID = id;
	b_singalForCinematic = false;
}

void Player_Interface::GameStart()
{
	gameStartState = STATE_STARTING_GAME;
	gameStartTime = 0.0f;
}

void Player_Interface::GameClear(bool value)
{
	if (b_missionClear == value) return;

	b_missionClear = value;
	if (value == true)
	{
		SOUND->StopSoundType(SDTYPE::BGM);
		SOUND->Play(SDKEY::BGM02, SDTYPE::BGM, 0.35f);
		clearTime = clearMaxTime;
	}
}

void Player_Interface::CloseCinematic()
{
	cinematicLevel = 0;
	cinematicTime = 0.0f;
	cinematicID = 0;
	b_singalForCinematic = false;
	soundBoard[SoundCondition::CINEMATIC_BGM_CHANGE] = false;
	soundBoard[SoundCondition::CINEMATIC_FADEIN] = false;
	sceneActors.clear();

	GET_PLAYER->st = GET_PLAYER->st_max;
	menu = MenuState::NORMAL;
}
