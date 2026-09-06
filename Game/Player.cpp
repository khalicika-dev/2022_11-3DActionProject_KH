#include "stdafx.h"

#define MENU Player_Interface::MenuState

inline Sound::SoundKey Player::SDKEY_FootstepA()
{
	return (SDKEY)((int)SDKEY::footstep_a00 + RANDOM->Int(0, 9));
}

bool Player::Check_Attack()
{
	for (auto n : atkNode)
	{
		if (actor->anim->PlayingIdx() == n.first)
			return true;
	}
	return false;
}

bool Player::Check_CutScene(bool isOption, bool isGravity)
{
	// 개인만 사용하는 카메라워킹 모션을 CUTSCENE
	// 여러개체가 사용하는 카메라워킹 모션을 CINEMATIC이라 칭한다

	bool b_motionWithGravity = (
		actor->anim->PlayingIdx() == PlayerAction::CUTSCENE_001_01 ||
		actor->anim->PlayingIdx() == PlayerAction::CUTSCENE_001_02 ||
		actor->anim->PlayingIdx() == PlayerAction::CUTSCENE_002_01 ||
		actor->anim->PlayingIdx() == PlayerAction::TITLE_POSE
		);	// 중력이 적용된 모션
	bool b_motionWithOutGravity = (PLAYER_UI->cinematicLevel >= 2 && PLAYER_UI->cinematicID == 1);	// 중력이 미적용된 모션
	if (isOption)
	{
		return isGravity ? b_motionWithGravity : b_motionWithOutGravity;
	}
	return b_motionWithGravity || b_motionWithOutGravity;
}

bool Player::Check_Unbreakable()
{
	return PLAYER_UI->b_missionClear == true;
}

void Player::Decrease_HP(int value)
{
	// 시네마틱 진행중이거나 Unbreakable 상태면 데미지를 받지 아니한다.
	if (PLAYER_UI->cinematicLevel > 0 || Check_Unbreakable())
		return;

	hp -= value;
}

Player::Player()
{
	actor = Actor::Create();
	actor->LoadFile("Claris.xml");
	camPivot = Actor::Create();
	camPivot->LoadFile("CamWithPivot.xml");
	if (App.GetAppQuit())
	{
		return;
	}

	actor->rootBoneIndex = actor->Find("Root")->boneIndex;
	trail = new Object_Trail(actor->Find("TrailStart"), actor->Find("TrailEnd"));
	camera = dynamic_cast<Camera*>(camPivot->Find("Camera"));
	eventCamera = dynamic_cast<Camera*>(actor->Find("MotionCamera"));
	actor->SetStandardMatrix({
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, 1, 0, 0,
		0, 0, 0, 1 }
	);

	for (int i = 0; i < SoundCondition::SE_ALL_COUNTS; i++)
		soundBoard.push_back(false);

	atkNode[PlayerAction::ATTACK_A1]			= AttackNode(1.0f, PlayerAction::ATTACK_A2, PlayerAction::ATTACK_B2_PrevA1, (50.0f / 120.0f), (70.0f / 120.0f), 0.05f, 0.1f);
	atkNode[PlayerAction::ATTACK_A2]			= AttackNode(1.0f, PlayerAction::ATTACK_A3, PlayerAction::ATTACK_B3, (35.0f / 80.0f), (50.0f / 80.0f));
	atkNode[PlayerAction::ATTACK_A3]			= AttackNode(1.0f, PlayerAction::ATTACK_A4, PlayerAction::ATTACK_B4, (50.0f / 100.0f), (80.0f / 100.0f));
	atkNode[PlayerAction::ATTACK_A4]			= AttackNode(1.0f, 0, 0, (70.0f / 100.0f), (90.0f / 100.0f), 0.15f, 0.1f);
	atkNode[PlayerAction::ATTACK_B1]			= AttackNode(1.0f, PlayerAction::ATTACK_A2, PlayerAction::ATTACK_B2_PrevB1, (80.0f / 120.0f), (100.0f / 120.0f), 0.1f, 0.1f);
	atkNode[PlayerAction::ATTACK_B2_PrevA1]		= AttackNode(1.3f, PlayerAction::ATTACK_A3, PlayerAction::ATTACK_B3, (90.0f / 110.0f), (100.0f / 110.0f), 0.075f);
	atkNode[PlayerAction::ATTACK_B2_PrevB1]		= AttackNode(1.3f, PlayerAction::ATTACK_A3, PlayerAction::ATTACK_B3, (100.0f / 120.0f), (110.0f / 120.0f), 0.075f);
	atkNode[PlayerAction::ATTACK_B3]			= AttackNode(1.0f, PlayerAction::ATTACK_A4, PlayerAction::ATTACK_B4, (60.0f / 120.0f), (90.0f / 120.0f), 0.175f, 0.1f);
	atkNode[PlayerAction::ATTACK_B4]			= AttackNode(1.0f, 0, 0, (65.0f / 110.0f), (90.0f / 110.0f), 0.1f);
	atkNode[PlayerAction::ATTACK_StepA1]		= AttackNode(1.0f, PlayerAction::ATTACK_A1, PlayerAction::ATTACK_B1, (45.0f / 70.0f), (50.0f / 70.0f));
	atkNode[PlayerAction::ATTACK_StepB1]		= AttackNode(1.0f, PlayerAction::ATTACK_A1_PrevStepB1, PlayerAction::ATTACK_B1_PrevStepB1, (30.0f / 80.0f), (60.0f / 80.0f));
	atkNode[PlayerAction::ATTACK_A1_PrevStepB1] = AttackNode(1.0f, PlayerAction::ATTACK_A2, PlayerAction::ATTACK_B2_PrevA1, (30.0f / 100.0f), (50.0f / 100.0f));
	atkNode[PlayerAction::ATTACK_B1_PrevStepB1] = AttackNode(1.0f, PlayerAction::ATTACK_A2, PlayerAction::ATTACK_B2_PrevB1, (70.0f / 110.0f), (90.0f / 110.0f), 0.175f);
	atkNode[PlayerAction::ATTACK_AirA1]			= AttackNode(1.0f, PlayerAction::ATTACK_AirA2, PlayerAction::ATTACK_AirB1, (35.0f / 80.0f), 0.0f, 0.05f, 0.1f);
	atkNode[PlayerAction::ATTACK_AirA2]			= AttackNode(1.0f, PlayerAction::ATTACK_AirA3, PlayerAction::ATTACK_AirB1, (35.0f / 100.0f), 0.0f, 0.05f);
	atkNode[PlayerAction::ATTACK_AirA3]			= AttackNode(1.0f, 0, PlayerAction::ATTACK_AirB1, (50.0f / 90.0f), 0.0f, 0.05f);
	atkNode[PlayerAction::ATTACK_AirB1]			= AttackNode(1.0f, 0, 0, 100.0f, 100.0f, 0.1f, 0.1f);
	atkNode[PlayerAction::ATTACK_AirB1_FALL]	= AttackNode(1.0f, 0, 0, 100.0f, 100.0f, 0.1f, 0.0f);
	atkNode[PlayerAction::ATTACK_AirB1_LAND]	= AttackNode(1.0f, PlayerAction::ATTACK_A1_PrevStepB1, PlayerAction::ATTACK_B1_PrevStepB1, (35.0f / 70.0f), (50.0f / 70.0f), 0.1f, 0.0f);

	
	Init();
	RESOURCE->DecreaseXmlList("Claris.xml");
	RESOURCE->DecreaseXmlList("CamWithPivot.xml");
}

Player::~Player()
{
	actor->Release();
	camPivot->Release();
	SafeDelete(trail);
}

void Player::Calculate()
{
	Object_Trail::Calculate();
	RESOURCE->IncreaseXmlList_AddTotalCapacity("Claris.xml");
	RESOURCE->IncreaseXmlList_AddTotalCapacity("CamWithPivot.xml");
}

void Player::Update()
{
	if (!Enable) return;

	actor->Update();
	camPivot->Update();
	trail->Update();
}

void Player::LateUpdate()
{
	if (!Enable) return;

	CameraControl::SetCamState(Camera::CameraState::PLAYER);
	Vector3 pos = actor->GetWorldPos();
	bool lastAir = isAir;

	if (resurrectionTime > 0.0f)
	{
		resurrectionTime -= DELTA_NS;
	}

	// 플레이어의 행동
	bool isMoved = false;	// 이번 업데이트내로 캐릭터가 통상이동 조작을 했다.
	bool isGuard = false;	// 방어조작을 했다.
	bool isAttack = false;	// 공격조작을 했다.

	// 아이템창 조작
	{
		if (current_itemIdx >= itemList.size())
		{
			current_itemIdx = itemList.size() - 1;
		}
		if (PLAYER_UI->menu == MENU::NORMAL)
		{
			if (!isAir && movable)
			{
				PLAYER_UI->menuOpen = 1;
				if (INPUT->KeyDown(VK_TAB)) PLAYER_UI->menu = MENU::ITEM;
			}
			else
				PLAYER_UI->menuOpen = 0;
		}
		else if (PLAYER_UI->menu == MENU::ITEM)
		{
			if (INPUT->KeyDown(VK_TAB) || INPUT->KeyDown(VK_RBUTTON, false) || INPUT->tracker.a == GamePad::ButtonStateTracker::ButtonState::PRESSED)
			{
				PLAYER_UI->menu = MENU::NORMAL;
			}

			bool leftPress = INPUT->KeyDelayPress('Q', false) || INPUT->PadDelayPress(INPUT->tracker.leftShoulder);
			bool rightPress = INPUT->KeyDelayPress('E', false) || INPUT->PadDelayPress(INPUT->tracker.rightShoulder);
			if (itemList.size() > 1 && leftPress && !rightPress)
			{
				PLAYER_UI->MoveItem(-1);
				current_itemIdx--;
				if (current_itemIdx < 0) current_itemIdx = itemList.size() - 1;
			}
			if (itemList.size() > 1 && rightPress && !leftPress)
			{
				PLAYER_UI->MoveItem(+1);
				current_itemIdx++;
				if (current_itemIdx >= itemList.size()) current_itemIdx = 0;
			}

			if (!isAir && movable && !itemList.empty() && INPUT->KeyDown(VK_SPACE, false) || INPUT->tracker.x == GamePad::ButtonStateTracker::ButtonState::PRESSED)
			{
				Swing = 0;
				Swing2 = 0;
				ChangeAnim(AnimationState::ONCE, PlayerAction::USE_ITEM, 0.05f);
				PLAYER_UI->menuItemUse = true;
				PLAYER_UI->menu = MENU::NORMAL;
				movable = actionable_atk = actionable_def = false;
			}
		}
	}

	// 플레이어의 방어
	{
		if ((actionable_def && (INPUT->KeyPress('R') || key_buffer == 'R')) ||	// 방어가 가능한 상태에서 가드키를 누르면 가드상태
			(Check_Guard() && guardTime <= 0.2f) ||		// 가드를 쓴 직후 0.2초까지는 방어상태
			actor->anim->PlayingIdx() == PlayerAction::GUARD_LAND)	// 가드 중 착지 직후에는 모션이 끝날때까지 가드상태
		{
			InitKeyBuffer();
			isGuard = true;
			airAttack = 0;
			guardTime += DELTA;

			if (!actor->anim->IsPlaying())
			{
				if (isAir && actor->anim->PlayingIdx() == PlayerAction::GUARDHIT_AIR)
					ChangeAnim(AnimationState::LOOP, PlayerAction::GUARD_AIR, 0.0f);
				if (!isAir && (actor->anim->PlayingIdx() == PlayerAction::GUARDHIT || actor->anim->PlayingIdx() == PlayerAction::GUARD_LAND))
					ChangeAnim(AnimationState::LOOP, PlayerAction::GUARD, 0.0f);
			}

			if (isAir && actor->anim->PlayingIdx() != PlayerAction::GUARDHIT_AIR && actor->anim->PlayingIdx() != PlayerAction::GUARDHIT)
			{
				if (ChangeAnim(AnimationState::LOOP, PlayerAction::GUARD_AIR, 0.1f))
				{
					if (!(actor->anim->isChanging && actor->anim->currentAnimator.animIdx == PlayerAction::GUARDHIT_AIR))
						YSpeed = GetJump(1.0f, 0.1f);
				}
			}
			else if (!isAir && actor->anim->PlayingIdx() != PlayerAction::GUARD_LAND && actor->anim->PlayingIdx() != PlayerAction::GUARDHIT)
			{
				ChangeAnim(AnimationState::LOOP, PlayerAction::GUARD, 0.1f);
			}
		}

		if (!isGuard) guardTime = 0.0f;

		if (!isGuard && isAir && Check_Guard())
		{
			actionable_def = false;
			airGuard = true;
		}
	}
	
	// 플레이어의 공격
	{
		if (attackStop && Check_Attack() && attackStopTime)
		{
			attackStopTime -= DELTA;
			if (attackStopTime <= 0.0f)
			{
				attackStop = false;
				actor->anim->aniScale = atkNode[actor->anim->PlayingIdx()].scale;
			}
		}

		if (actionable_atk && (INPUT->KeyDown(VK_LBUTTON) || INPUT->KeyDown(VK_RBUTTON) || key_buffer == VK_LBUTTON || key_buffer == VK_RBUTTON))
		{
			bool heavy = INPUT->KeyDown(VK_RBUTTON) || key_buffer == VK_RBUTTON;
			UINT idx;

			InitKeyBuffer();

			if (Check_Attack() && (actor->anim->GetPlayTime() <= atkNode[actor->anim->PlayingIdx()].comboTime || isAir))
			{
				// 콤보가 가능한 상태이다.
				if (!heavy && atkNode[actor->anim->PlayingIdx()].nextA_animIdx != 0)
				{
					idx = atkNode[actor->anim->PlayingIdx()].nextA_animIdx;
					if (isAir) airAttack++;
					isAttack = true;
				}
				else if (heavy && atkNode[actor->anim->PlayingIdx()].nextB_animIdx != 0)
				{
					st -= 5.0f;
					idx = atkNode[actor->anim->PlayingIdx()].nextB_animIdx;
					if (isAir) airAttack++;
					isAttack = true;
				}
			}
			else
			{
				// 첫 공격이다.
				if (isAir)
				{
					bool test = true;
					// 공중공격
					if (heavy)
					{
						st -= 5.0f;
						idx = PlayerAction::ATTACK_AirB1;
					}
					else
					{
						switch (airAttack)
						{
						case 0:idx = PlayerAction::ATTACK_AirA1; break;
						case 1:idx = PlayerAction::ATTACK_AirA2; break;
						case 2:idx = PlayerAction::ATTACK_AirA3; break;
						default: test = false;
						}
					}
					if (test)
					{
						stepCount = 1;
						airAttack++;
						isAttack = true;
					}
				}
				else if (Check_Step())
				{
					// 지상스텝공격
					if (heavy) st -= 5.0f;
					idx = heavy ? PlayerAction::ATTACK_StepB1 : PlayerAction::ATTACK_StepA1;
					isAttack = true;
				}
				else
				{
					// 지상공격
					if (heavy) st -= 5.0f;
					idx = heavy ? PlayerAction::ATTACK_B1 : PlayerAction::ATTACK_A1;
					isAttack = true;
				}
			}
			if (isAttack)
			{
				Vector3 dir = GetMov();
				dir.Normalize();
				realDir = dir;
				actor->rotation.y = atan2(-dir.x, -dir.z);
				actor->ClearAttackTouchedList();
				ChangeAnim(AnimationState::ONCE, idx, atkNode[idx].blend, true);
				actor->anim->aniScale = atkNode[idx].scale;
				attackTime = 0.0f;
				attackStopTime = 0.0f;
				attackStop = false;
				Swing = 0;
				Swing2 = 0;
			}
		}
		if (Check_Attack()) attackTime += DELTA;
	}

	// 모션으로 인한 공격판정 생성
	{
		Object* ob;
		int dmg = 0;
		int knock = 0;
		SDKEY hitSE = SDKEY::NONE;
		SDKEY guardSE = SDKEY::NONE;
		for (int i = 1; ob = GetActor()->Find("AttackCollider1_" + to_string(i)); i++)
		{
			if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_A1 &&
				((actor->anim->GetPlayTime() >= 7.0f / 120.0f && actor->anim->GetPlayTime() <= 20.0f / 120.0f) ||
					(actor->anim->GetPlayTime() >= 30.0f / 120.0f && actor->anim->GetPlayTime() <= 40.0f / 120.0f)))
			{
				ob->collider->enable = (i == 1);
				trail->AddTrail();
				if (Swing == 0 && actor->anim->GetPlayTime() >= 30.0f / 120.0f)
				{
					// 두번째 스윙때 재공격 가능
					Swing = 1;
					ob->collider->touched.clear();
				}
				switch (Swing)
				{
				case 0: dmg = 280; knock = 280; hitSE = SDKEY::hit_sword01; guardSE = SDKEY::block_sword01; break;
				case 1: dmg = 300; knock = 300; hitSE = SDKEY::hit_sword01; guardSE = SDKEY::block_sword01; break;
				}
			}
			else if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_A2 &&
				(actor->anim->GetPlayTime() >= 8.0f / 80.0f && actor->anim->GetPlayTime() <= 20.0f / 80.0f))
			{
				ob->collider->enable = (i == 1);
				trail->AddTrail();
				dmg = 520; knock = 480; hitSE = SDKEY::hit_sword01; guardSE = SDKEY::block_sword00;
			}
			else if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_A3 &&
				(actor->anim->GetPlayTime() >= 23.0f / 100.0f && actor->anim->GetPlayTime() <= 40.0f / 100.0f))
			{
				ob->collider->enable = (i == 1);
				trail->AddTrail();
				dmg = 600; knock = 600; hitSE = SDKEY::hit_sword02; guardSE = SDKEY::block_sword02;
			}
			else if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_A4 &&
				(actor->anim->GetPlayTime() >= 37.0f / 100.0f && actor->anim->GetPlayTime() <= 50.0f / 100.0f))
			{
				ob->collider->enable = (i == 1);
				trail->AddTrail();
				dmg = 800; knock = 1125; hitSE = SDKEY::hit_sword03; guardSE = SDKEY::block_sword03;
			}
			else if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_B1 &&
				((actor->anim->GetPlayTime() >= 23.0f / 120.0f && actor->anim->GetPlayTime() <= 27.0f / 120.0f) ||
					(actor->anim->GetPlayTime() >= 50.0f / 120.0f && actor->anim->GetPlayTime() <= 55.0f / 120.0f)))
			{
				ob->collider->enable = (i == 1);
				trail->AddTrail();
				if (Swing == 0 && actor->anim->GetPlayTime() >= 50.0f / 120.0f)
				{
					// 두번째 스윙때 재공격 가능
					Swing = 1;
					ob->collider->touched.clear();
				}
				switch (Swing)
				{
				case 0: dmg = 275; knock = 400; hitSE = SDKEY::hit_sword02; guardSE = SDKEY::block_sword01; break;
				case 1: dmg = 425; knock = 300; hitSE = SDKEY::hit_sword03; guardSE = SDKEY::block_sword02; break;
				}
			}
			else if ((actor->anim->PlayingIdx() == PlayerAction::ATTACK_B2_PrevA1 &&
				((actor->anim->GetPlayTime() >= 23.0f / 110.0f && actor->anim->GetPlayTime() <= 30.0f / 110.0f) ||
					(actor->anim->GetPlayTime() >= 43.0f / 110.0f && actor->anim->GetPlayTime() <= 50.0f / 110.0f)))
				|| (actor->anim->PlayingIdx() == PlayerAction::ATTACK_B2_PrevB1 &&
					((actor->anim->GetPlayTime() >= 33.0f / 120.0f && actor->anim->GetPlayTime() <= 40.0f / 120.0f) ||
						(actor->anim->GetPlayTime() >= 53.0f / 120.0f && actor->anim->GetPlayTime() <= 60.0f / 120.0f))))
			{
				ob->collider->enable = (i == 1);
				trail->AddTrail();
				if (Swing == 0 &&
					((actor->anim->PlayingIdx() == PlayerAction::ATTACK_B2_PrevA1 && actor->anim->GetPlayTime() >= 43.0f / 110.0f) ||
						(actor->anim->PlayingIdx() == PlayerAction::ATTACK_B2_PrevB1 && actor->anim->GetPlayTime() >= 53.0f / 120.0f))
					)
				{
					// 두번째 스윙때 재공격 가능
					Swing = 1;
					ob->collider->touched.clear();
				}
				switch (Swing)
				{
				case 0: dmg = 410; knock = 300; hitSE = SDKEY::hit_sword02; guardSE = SDKEY::block_sword02; break;
				case 1: dmg = 410; knock = 300; hitSE = SDKEY::hit_sword02; guardSE = SDKEY::block_sword02; break;
				}
			}
			else if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_B3 &&
				(actor->anim->GetPlayTime() >= 22.0f / 120.0f && actor->anim->GetPlayTime() <= 39.0f / 120.0f))
			{
				ob->collider->enable = (i == 1);
				trail->AddTrail();
				dmg = 750; knock = 650; hitSE = SDKEY::hit_sword02; guardSE = SDKEY::block_sword02;
			}
			else if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_StepA1 &&
				(actor->anim->GetPlayTime() >= 22.0f / 70.0f && actor->anim->GetPlayTime() <= 35.0f / 70.0f))
			{
				ob->collider->enable = (i == 1);
				trail->AddTrail();
				dmg = 480; knock = 485; hitSE = SDKEY::hit_sword01; guardSE = SDKEY::block_sword01;
			}
			else if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_A1_PrevStepB1 &&
				(actor->anim->GetPlayTime() >= 10.0f / 100.0f && actor->anim->GetPlayTime() <= 20.0f / 100.0f))
			{
				ob->collider->enable = (i == 1);
				trail->AddTrail();
				dmg = 500; knock = 500; hitSE = SDKEY::hit_sword01; guardSE = SDKEY::block_sword01;
			}
			else if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_B1_PrevStepB1 &&
				(actor->anim->GetPlayTime() >= 37.0f / 110.0f && actor->anim->GetPlayTime() <= 45.0f / 110.0f))
			{
				ob->collider->enable = (i == 1);
				trail->AddTrail();
				dmg = 700; knock = 700; hitSE = SDKEY::hit_sword03; guardSE = SDKEY::block_sword03;
			}
			else if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_AirA1 &&
				(actor->anim->GetPlayTime() >= 11.0f / 80.0f && actor->anim->GetPlayTime() <= 25.0f / 80.0f))
			{
				ob->collider->enable = (i == 1);
				trail->AddTrail();
				dmg = 590; knock = 490; hitSE = SDKEY::hit_sword01; guardSE = SDKEY::block_sword01;
			}
			else if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_AirA2 &&
				(actor->anim->GetPlayTime() >= 12.0f / 100.0f && actor->anim->GetPlayTime() <= 30.0f / 100.0f))
			{
				ob->collider->enable = (i == 1);
				trail->AddTrail();
				dmg = 605; knock = 505; hitSE = SDKEY::hit_sword01; guardSE = SDKEY::block_sword01;
			}
			else if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_AirA3 &&
				(actor->anim->GetPlayTime() >= 18.0f / 90.0f && actor->anim->GetPlayTime() <= 30.0f / 90.0f))
			{
				ob->collider->enable = true;
				trail->AddTrail();
				dmg = 650; knock = 550; hitSE = SDKEY::hit_sword02; guardSE = SDKEY::block_sword02;
			}
			else if ((actor->anim->PlayingIdx() == PlayerAction::ATTACK_AirB1 && actor->anim->GetPlayTime() > 55.0f / 65.0f) ||
				actor->anim->PlayingIdx() == PlayerAction::ATTACK_AirB1_FALL ||
				(actor->anim->PlayingIdx() == PlayerAction::ATTACK_AirB1_LAND && actor->anim->GetPlayTime() <= 10.0f / 70.0f))
			{
			ob->collider->enable = true;
			trail->AddTrail();
			dmg = 750; knock = 450; hitSE = SDKEY::hit_sword03; guardSE = SDKEY::block_sword02;
			}
			else
			{
				ob->collider->enable = false;
			}
			AttackToMonster(ob->collider, dmg, knock, hitSE, guardSE);
		}
		for (int i = 1; ob = GetActor()->Find("AttackCollider2_" + to_string(i)); i++)
		{
			if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_B4 &&
				(actor->anim->GetPlayTime() >= 36.0f / 110.0f && actor->anim->GetPlayTime() <= 45.0f / 110.0f))
			{
				ob->collider->enable = (i == 1);
				trail->AddTrail();
				dmg = 1225; knock = 775; hitSE = SDKEY::hit_sword03; guardSE = SDKEY::block_sword02;
			}
			else
			{
				ob->collider->enable = false;
			}
			AttackToMonster(ob->collider, dmg, knock, hitSE, guardSE);
		}
		for (int i = 1; ob = GetActor()->Find("AttackCollider3_" + to_string(i)); i++)
		{
			if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_StepB1 &&
				(actor->anim->GetPlayTime() >= 1.0f / 80.0f && actor->anim->GetPlayTime() <= 7.0f / 80.0f))
			{
				ob->collider->enable = (i == 1);
				trail->AddTrail();
				dmg = 565; knock = 375; hitSE = SDKEY::hit_sword00; guardSE = SDKEY::block_sword00;
			}
			else
			{
				ob->collider->enable = false;
			}
			AttackToMonster(ob->collider, dmg, knock, hitSE, guardSE);
		}
	}

	// 모션으로 인한 사운드 생성
	switch (actor->anim->PlayingIdx())
	{
	case PlayerAction::MOVE:
	case PlayerAction::DASH:
	{
		if (actor->anim->GetPlayTime() >= 10.0f / 60.0f)
			SOUND->Play(SDKEY_FootstepA(), soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.25f, pos);
		if (actor->anim->GetPlayTime() >= 40.0f / 60.0f)
			SOUND->Play(SDKEY_FootstepA(), soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.25f, pos);

		if (actor->anim->IsLooped())
		{
			soundBoard[SoundCondition::SE_SPACE0] = false;
			soundBoard[SoundCondition::SE_SPACE1] = false;
		}
		break;
	}
	case PlayerAction::DOWN_LAND:
		if (actor->anim->GetPlayTime() >= 3.0f / 60.0f) SOUND->Play(SDKEY::down00, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::USE_ITEM:
		if (actor->anim->GetPlayTime() >= 60.0f / 100.0f) SOUND->Play(SDKEY::item_use, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::ATTACK_A1:
		if (actor->anim->GetPlayTime() >= 2.0f / 120.0f) SOUND->Play(SDKEY::slidestep00, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 15.0f / 120.0f) SOUND->Play(SDKEY::attack_sword00, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 30.0f / 120.0f) SOUND->Play(SDKEY::attack_sword00, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::ATTACK_A2:
		if (actor->anim->GetPlayTime() >= 8.0f / 80.0f) SOUND->Play(SDKEY::attack_sword01, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::ATTACK_A3:
		if (actor->anim->GetPlayTime() >= 10.0f / 100.0f) SOUND->Play(SDKEY::evade01, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.5f, actor);
		if (actor->anim->GetPlayTime() >= 23.0f / 100.0f) SOUND->Play(SDKEY::attack_sword02, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::ATTACK_A4:
		if (actor->anim->GetPlayTime() >= 10.0f / 100.0f) SOUND->Play(SDKEY::evade02, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.5f, actor);
		if (actor->anim->GetPlayTime() >= 37.0f / 100.0f) SOUND->Play(SDKEY::attack_sword03, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::ATTACK_B1:
		if (actor->anim->GetPlayTime() >= 2.0f / 120.0f) SOUND->Play(SDKEY::evade02, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.5f, actor);
		if (actor->anim->GetPlayTime() >= 23.0f / 120.0f) SOUND->Play(SDKEY::attack_sword02, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 55.0f / 120.0f) SOUND->Play(SDKEY::attack_sword04, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::ATTACK_B2_PrevA1:
	case PlayerAction::ATTACK_B2_PrevB1:
	{
		bool prevB1 = actor->anim->PlayingIdx() == PlayerAction::ATTACK_B2_PrevB1;
		float addFrame = prevB1 ? 10.0f : 0.0f;
		//if (prevB1 && actor->anim->GetPlayTime() >= 4.0f / (110.0f + addFrame)) SOUND->Play(SDKEY::slidestep00, soundList, SoundType::SE_SPACE4, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= (5.0f + addFrame) / (110.0f + addFrame)) SOUND->Play(SDKEY::evade00, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.5f, actor);
		if (actor->anim->GetPlayTime() >= (23.0f + addFrame) / (110.0f + addFrame)) SOUND->Play(SDKEY::attack_sword00, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= (43.0f + addFrame) / (110.0f + addFrame)) SOUND->Play(SDKEY::attack_sword00, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= (50.0f + addFrame) / (110.0f + addFrame)) SOUND->Play(SDKEY::jump00, soundBoard, SoundCondition::SE_SPACE3, SDTYPE::SE, 0.5f, actor);
		break;
	}
	case PlayerAction::ATTACK_B3:
		SOUND->Play(SDKEY::slidestep01, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 22.0f / 120.0f) SOUND->Play(SDKEY::attack_sword02, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::ATTACK_B4:
		if (actor->anim->GetPlayTime() >= 3.0f / 110.0f) SOUND->Play(SDKEY::slidestep01, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 5.0f / 110.0f) SOUND->Play(SDKEY::draw_sword00, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 36.0f / 110.0f) SOUND->Play(SDKEY::attack_sword05, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::ATTACK_StepA1:
		if (actor->anim->GetPlayTime() >= 3.0f / 70.0f) SOUND->Play(SDKEY::slidestep01, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 22.0f / 70.0f) SOUND->Play(SDKEY::attack_sword02, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::ATTACK_StepB1:
		SOUND->Play(SDKEY::evade00, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.5f, actor);
		if (actor->anim->GetPlayTime() >= 1.0f / 80.0f) SOUND->Play(SDKEY::attack_sword01, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::ATTACK_A1_PrevStepB1:
		if (actor->anim->GetPlayTime() >= 2.0f / 100.0f) SOUND->Play(SDKEY::slidestep00, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 10.0f / 100.0f) SOUND->Play(SDKEY::attack_sword00, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::ATTACK_B1_PrevStepB1:
		if (actor->anim->GetPlayTime() >= 2.0f / 110.0f) SOUND->Play(SDKEY::evade02, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.5f, actor);
		if (actor->anim->GetPlayTime() >= 20.0f / 110.0f) SOUND->Play(SDKEY_FootstepA(), soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.5f, actor);
		if (actor->anim->GetPlayTime() >= 43.0f / 110.0f) SOUND->Play(SDKEY::attack_sword04, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::ATTACK_AirA1:
		if (actor->anim->GetPlayTime() >= 11.0f / 80.0f) SOUND->Play(SDKEY::attack_sword01, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::ATTACK_AirA2:
		if (actor->anim->GetPlayTime() >= 12.0f / 100.0f) SOUND->Play(SDKEY::attack_sword01, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::ATTACK_AirA3:
		if (actor->anim->GetPlayTime() >= 15.0f / 90.0f) SOUND->Play(SDKEY::evade00, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.5f, actor);
		if (actor->anim->GetPlayTime() >= 18.0f / 90.0f) SOUND->Play(SDKEY::attack_sword02, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::ATTACK_AirB1:
		if (actor->anim->GetPlayTime() >= 10.0f / 65.0f) SOUND->Play(SDKEY::jump00, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.75f, actor);
		if (actor->anim->GetPlayTime() >= 30.0f / 65.0f) SOUND->Play(SDKEY::echo00, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.75f, actor);
		if (actor->anim->GetPlayTime() >= 35.0f / 65.0f) SOUND->Play(SDKEY::draw_sword00, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 0.5f, actor);
		if (actor->anim->GetPlayTime() >= 60.0f / 65.0f) SOUND->Play(SDKEY::attack_sword05, soundBoard, SoundCondition::SE_SPACE3, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::ATTACK_AirB1_LAND:
		if (actor->anim->GetPlayTime() >= 2.0f / 70.0f) SOUND->Play(SDKEY::evade02, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 3.0f / 70.0f) SOUND->Play(SDKEY::slidestep02, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::DIE:
		if (actor->anim->GetPlayTime() >= 47.0f / 300.0f) SOUND->Play(SDKEY_FootstepA(), soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.75f, actor);
		if (actor->anim->GetPlayTime() >= 98.0f / 300.0f) SOUND->Play(SDKEY::down00, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.5f, actor);
		break;
	case PlayerAction::DIE_LAND:
		if (actor->anim->GetPlayTime() >= 3.0f / 240.0f) SOUND->Play(SDKEY::down01, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.5f, actor);
		break;
	case PlayerAction::CINEMATIC_001_01:
		if (actor->anim->GetPlayTime() > 40.0f / 100.0f) SOUND->Play(SDKEY::evade01, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor->Find("Root_Rot"));
		if (actor->anim->GetPlayTime() > 60.0f / 100.0f && soundBoard[SoundCondition::SE_SPACE1] == false)
		{
			INPUT->SetVibration(0.2f, 0.0f, 0.5f);
			SOUND->Play(SDKEY::footstep_misc00, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.5f, actor->Find("Root_Rot")->GetWorldPos());
		}
		break;
	case PlayerAction::CINEMATIC_001_02:
		if (actor->anim->GetPlayTime() >= 30.0f / 100.0f) SOUND->Play(SDKEY::misc00, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.25f, actor->Find("Root_Rot"));
		break;
	case PlayerAction::CINEMATIC_001_03:
		SOUND->Play(SDKEY::slidestep02, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.35f, actor->Find("Root_Rot"));
		if (actor->anim->GetPlayTime() >= 40.0f / 52.0f) SOUND->Play(SDKEY::footstep_a04, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.5f, actor->Find("Root_Rot")->GetWorldPos());
		break;
	case PlayerAction::CINEMATIC_001_04:
	{
		float maxFrame = 135.0f;
		float steps[4] = { 17.0f, 47.0f, 76.0f, 106.0f };
		SDKEY stepSD[4] = { SDKEY::footstep_a03, SDKEY::footstep_a00, SDKEY::footstep_a09, SDKEY::footstep_a08 };
		for (int i = 0; i < sizeof(steps) / sizeof(float); i++)
		{
			bool cond = (i % 2 == 0);
			float beginTime = steps[i] / maxFrame;
			float endTime = (i < (sizeof(steps) / sizeof(float)) - 1) ? (steps[i + 1] / maxFrame) : 1.0f;
			int curSpace = cond ? SoundCondition::SE_SPACE0 : SoundCondition::SE_SPACE1;
			int nextSpace = cond ? SoundCondition::SE_SPACE1 : SoundCondition::SE_SPACE0;

			if (actor->anim->GetPlayTime() >= beginTime && actor->anim->GetPlayTime() < endTime)
			{
				SOUND->Play(stepSD[i], soundBoard, curSpace, SDTYPE::SE, 0.5f, actor->Find("Root_Rot")->GetWorldPos());
				soundBoard[nextSpace] = false;
			}
		}
		break;
	}
	case PlayerAction::CINEMATIC_001_05:
	{
		float maxFrame = 236.0f;
		float steps[8] = { 0.0f, 30.0f, 59.0f, 89.0f, 118.0f, 148.0f, 177.0f, 207.0f };
		SDKEY stepSD[8] = { SDKEY::footstep_a05, SDKEY::footstep_a06, SDKEY::footstep_a02, SDKEY::footstep_a04,
							SDKEY::footstep_a03, SDKEY::footstep_a06, SDKEY::footstep_a08, SDKEY::footstep_a09 };
		for (int i = 0; i < sizeof(steps) / sizeof(float); i++)
		{
			bool cond = (i % 2 == 0);
			float beginTime = steps[i] / maxFrame;
			float endTime = (i < (sizeof(steps) / sizeof(float)) - 1) ? (steps[i + 1] / maxFrame) : 1.0f;
			int curSpace = cond ? SoundCondition::SE_SPACE0 : SoundCondition::SE_SPACE1;
			int nextSpace = cond ? SoundCondition::SE_SPACE1 : SoundCondition::SE_SPACE0;

			if (actor->anim->GetPlayTime() >= beginTime && actor->anim->GetPlayTime() < endTime)
			{
				SOUND->Play(stepSD[i], soundBoard, curSpace, SDTYPE::SE, 0.5f, actor->Find("Root_Rot")->GetWorldPos());
				soundBoard[nextSpace] = false;
			}
		}
		break;
	}
	case PlayerAction::CINEMATIC_001_06:
	{
		float maxFrame = 270.0f;
		float steps[7] = { 0.0f, 30.0f, 59.0f, 89.0f, 118.0f, 140.0f, 220.0f };
		SDKEY stepSD[7] = { SDKEY::footstep_a02, SDKEY::footstep_a05, SDKEY::footstep_a07, 
							SDKEY::footstep_a00, SDKEY::footstep_a03, SDKEY::footstep_misc00, SDKEY::footstep_a03 };
		for (int i = 0; i < sizeof(steps) / sizeof(float); i++)
		{
			bool cond = (i % 2 == 0);
			float beginTime = steps[i] / maxFrame;
			float endTime = (i < (sizeof(steps) / sizeof(float)) - 1) ? (steps[i + 1] / maxFrame) : 1.0f;
			int curSpace = cond ? SoundCondition::SE_SPACE0 : SoundCondition::SE_SPACE1;
			int nextSpace = cond ? SoundCondition::SE_SPACE1 : SoundCondition::SE_SPACE0;

			if (actor->anim->GetPlayTime() >= beginTime && actor->anim->GetPlayTime() < endTime)
			{
				float stepVol = (i == 5) ? 0.4f : 0.8f;
				SOUND->Play(stepSD[i], soundBoard, curSpace, SDTYPE::SE, stepVol, actor->Find("Root_Rot")->GetWorldPos());
				soundBoard[nextSpace] = false;
			}
		}

		if (Object* bossObj = GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GIANT, 0)->GetActor()->Find("Root_Rot"))
		{
			if (actor->anim->GetPlayTime() >= 128.0f / maxFrame) 
				SOUND->Play(SDKEY::explosion00, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 1.0f, bossObj->GetWorldPos(), 100.0f);
			if (actor->anim->GetPlayTime() >= 130.0f / maxFrame && soundBoard[SoundCondition::SE_SPACE3] == false)
			{
				INPUT->SetVibration(0.4f,0.5f,0.6f);
				soundBoard[SoundCondition::SE_SPACE3] = true;
			}
			if (actor->anim->GetPlayTime() >= 200.0f / maxFrame)
			{
				SOUND->Play(SDKEY::robot04, soundBoard, SoundCondition::SE_SPACE4, SDTYPE::SE, 0.5f, bossObj, 50.0f);
				SOUND->Play(SDKEY::steam00, soundBoard, SoundCondition::SE_SPACE5, SDTYPE::SE, 0.5f, bossObj, 50.0f);
			}
		}
		break;
	}
	case PlayerAction::CINEMATIC_001_07:
		if (actor->anim->GetPlayTime() >= 15.0f / 90.0f) SOUND->Play(SDKEY::misc00, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.3f);
		if (actor->anim->GetPlayTime() >= 70.0f / 90.0f) SOUND->Play(SDKEY::misc01, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f);
		break;
	case PlayerAction::CINEMATIC_001_09:
		SOUND->Play(SDKEY::slidestep01, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor->Find("Root_Rot")->GetWorldPos());
		if (actor->anim->GetPlayTime() >= 10.0f / 30.0f) SOUND->Play(SDKEY::draw_sword01, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor->Find("Root_Rot"));
		break;
	case PlayerAction::CINEMATIC_001_11:
		SOUND->Play(SDKEY::misc00, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.25f);
		break;
	case PlayerAction::CINEMATIC_001_12:
		if (actor->anim->GetPlayTime() >= 20.0f / 163.0f) SOUND->Play(SDKEY::misc02, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f);
		break;
	case PlayerAction::CINEMATIC_001_13:
		if (actor->anim->GetPlayTime() >= 260.0f / 330.0f) SOUND->Play(SDKEY::attack_sword05, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor->Find("Root_Rot"));
		break;
	case PlayerAction::CUTSCENE_001_01:
		if (actor->anim->GetPlayTime() >= 30.0f / 74.0f) SOUND->Play(SDKEY::footstep_a08, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.25f, pos);
		if (actor->anim->GetPlayTime() >= 59.0f / 74.0f) SOUND->Play(SDKEY::footstep_a02, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.25f, pos);
		break;
	case PlayerAction::CUTSCENE_001_02:
		if (actor->anim->GetPlayTime() >= 15.0f / 220.0f) SOUND->Play(SDKEY::footstep_a04, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.25f, pos);
		if (actor->anim->GetPlayTime() >= 22.0f / 220.0f) SOUND->Play(SDKEY::draw_sword01, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 40.0f / 220.0f) SOUND->Play(SDKEY::footstep_a00, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 0.25f, pos);
		if (actor->anim->GetPlayTime() >= 55.0f / 220.0f) SOUND->Play(SDKEY::misc03, soundBoard, SoundCondition::SE_SPACE3, SDTYPE::SE, 1.0f);
		if (actor->anim->GetPlayTime() >= 75.0f / 220.0f) SOUND->Play(SDKEY::attack_sword01, soundBoard, SoundCondition::SE_SPACE4, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 100.0f / 220.0f) SOUND->Play(SDKEY::misc00, soundBoard, SoundCondition::SE_SPACE5, SDTYPE::SE, 1.0f);
		if (actor->anim->GetPlayTime() >= 123.0f / 220.0f) SOUND->Play(SDKEY::attack_sword05, soundBoard, SoundCondition::SE_SPACE6, SDTYPE::SE, 1.0f, actor);
		break;
	case PlayerAction::CUTSCENE_002_01:
		if (actor->anim->GetPlayTime() >= 3.0f / 80.0f) SOUND->Play(SDKEY::misc04, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f);
		if (actor->anim->GetPlayTime() >= 57.0f / 80.0f) SOUND->Play(SDKEY::hit_sword04, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		break;
	}

	// 모션으로 인한 이벤트 발생
	{
		if (actor->anim->PlayingIdx() == PlayerAction::USE_ITEM)
		{
			if ((Swing == 0 && actor->anim->GetPlayTime() >= 60.0f / 100.0f))
			{
				auto it = itemList.begin();
				for (int i = 0; i < current_itemIdx; i++) it++;

				Swing = 1;
				{
					Vector3 spawnPos = pos + actor->GetForward() * -1.1f + Vector3(0.0f, 2.8f, 0.0f) + actor->GetRight() * -0.4f;
					Pop* temp = dynamic_cast<Pop*>(GAMEOBJ->Generate(PTCTYPE::Effect_Hit, spawnPos)->GetActor());
					temp->scale = { 0.1f,0.1f,0.1f };
					temp->material->diffuse = ItemManager::itemInfo[it->first].color;
					temp->duration = 3.0f;
					temp->desc.gravity = 5.0f;
					temp->velocityScalar = 4.0f;
					temp->particleCount = 50;
					temp->Play();
				}
				ItemManager::itemInfo[it->first].effect();
				it->second--;
				if (it->second <= 0)
					itemList.erase(it);
			}
		}
		else if (Swing2 == 0 && actor->anim->PlayingIdx() == PlayerAction::ATTACK_B1 && actor->anim->GetPlayTime() >= 40.0f / 120.0f)
		{
			Swing2 = 1;
			if (Check_MoveControl())
			{
				Vector3 dir = GetMov();
				dir.Normalize();
				realDir = dir;
			}
		}
		else if (Swing2 == 0 && ((actor->anim->PlayingIdx() == PlayerAction::ATTACK_B2_PrevA1 && actor->anim->GetPlayTime() >= 30.0f / 110.0f)
			|| (actor->anim->PlayingIdx() == PlayerAction::ATTACK_B2_PrevB1 && actor->anim->GetPlayTime() >= 40.0f / 120.0f)))
		{
			Swing2 = 1;
			if (Check_MoveControl())
			{
				Vector3 dir = GetMov();
				dir.Normalize();
				realDir = dir;
			}
		}
		else if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_B4)
		{
			if (Swing2 == 0 && actor->anim->GetPlayTime() >= 34.0f / 110.0f)
			{
				Swing2 = 1;
				if (Check_MoveControl())
				{
					Vector3 dir = GetMov();
					dir.Normalize();
					Rotate(dir, PI_DIV4 / DELTA);
					realDir = Vector3::Transform({ 0.0f,0.0f,-1.0f }, Matrix::CreateFromAxisAngle({ 0.0f,1.0f,0.0f }, actor->rotation.y));
				}
			}
			else if (actor->anim->GetPlayTime() < 34.0f / 110.0f)
			{
				if (Check_MoveControl())
				{
					Vector3 dir = GetMov();
					dir.Normalize();
					Rotate(dir, 4.0f * PI);
					realDir = Vector3::Transform({ 0.0f,0.0f,-1.0f }, Matrix::CreateFromAxisAngle({ 0.0f,1.0f,0.0f }, actor->rotation.y));
				}
			}
		}
		else if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_B1_PrevStepB1)
		{
			if (Swing2 == 0 && actor->anim->GetPlayTime() >= 34.0f / 110.0f)
			{
				Swing2 = 1;
				if (Check_MoveControl())
				{
					Vector3 dir = GetMov();
					dir.Normalize();
					Rotate(dir, PI_DIV4 / DELTA);
					realDir = Vector3::Transform({ 0.0f,0.0f,-1.0f }, Matrix::CreateFromAxisAngle({ 0.0f,1.0f,0.0f }, actor->rotation.y));
				}
			}
			else if (actor->anim->GetPlayTime() < 34.0f / 110.0f)
			{
				if (Check_MoveControl())
				{
					Vector3 dir = GetMov();
					dir.Normalize();
					Rotate(dir, 4.0f * PI);
					realDir = Vector3::Transform({ 0.0f,0.0f,-1.0f }, Matrix::CreateFromAxisAngle({ 0.0f,1.0f,0.0f }, actor->rotation.y));
				}
			}
		}
		else if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_AirB1)
		{
			if ((Swing == 0 && actor->anim->GetPlayTime() >= 30.0f / 65.0f))
			{
				Swing = 1;
				{
					Vector3 spawnPos = pos + actor->GetForward() * 1.4f + Vector3(0.0f, 2.35f, 0.0f);
					Pop* temp = dynamic_cast<Pop*>(GAMEOBJ->Generate(PTCTYPE::Effect_Hit, spawnPos)->GetActor());
					temp->rotation = { -120.0f * TORADIAN, GetRealRot(), 0.0f };
					temp->rotRange = [=]() { return Vector3(0.0f, temp->DefaultRot(), 0.0f); };
					temp->scale = { 0.15f,0.15f,0.15f };
					temp->material->diffuse = { 0.33f,0.5f,0.87f };
					temp->duration = 1.0f;
					temp->desc.gravity = 0.0f;
					temp->velocityScalar = 4.0f;
					temp->particleCount = 50;
					temp->Play();
				}
			}

			if (!actor->anim->IsPlaying())
			{
				effectTime = 0.0f;
				ChangeAnim(AnimationState::ONCE, PlayerAction::ATTACK_AirB1_FALL, atkNode[PlayerAction::ATTACK_AirB1_FALL].blend);
			}
		}
		else if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_AirB1_FALL)
		{
			if(TIMER->GetTick(effectTime, 0.1f))
			{
				Vector3 spawnPos = pos + actor->GetForward() * 1.4f + Vector3(0.0f, 2.35f, 0.0f);
				Pop* temp = dynamic_cast<Pop*>(GAMEOBJ->Generate(PTCTYPE::Effect_Hit, spawnPos)->GetActor());
				temp->rotation = { -120.0f * TORADIAN, GetRealRot(), 0.0f };
				temp->rotRange = [=]() { return Vector3(0.0f, temp->DefaultRot(), 0.0f); };
				temp->scale = { 0.03f,0.03f,0.03f };
				temp->material->diffuse = { 0.33f,0.5f,0.87f };
				temp->duration = 1.0f;
				temp->desc.gravity = 0.0f;
				temp->velocityScalar = 5.0f;
				temp->particleCount = 100;
				temp->Play();
			}
		}
		else if (actor->anim->PlayingIdx() == PlayerAction::DIE || actor->anim->PlayingIdx() == PlayerAction::DIE_LAND)
		{
			if ((actor->anim->PlayingIdx() == PlayerAction::DIE && actor->anim->GetPlayTime() >= 150.0f / 300.0f) ||
				(actor->anim->PlayingIdx() == PlayerAction::DIE_LAND && actor->anim->GetPlayTime() >= 90.0f / 240.0f))
			{
				CameraControl::SetCamState(Camera::CameraState::PLAYER_EVENT);
				PLAYER_UI->SetBossUI(nullptr);
				PLAYER_UI->SetDying(2);
			}
		}
		else if (Check_CutScene(true, true))
		{
			CameraControl::SetCamState(Camera::CameraState::PLAYER_EVENT);
			if (actor->anim->PlayingIdx() == PlayerAction::CUTSCENE_001_01 && !actor->anim->IsPlaying())
				ChangeAnim(AnimationState::ONCE, PlayerAction::CUTSCENE_001_02, 0.0f);
			else if (actor->anim->PlayingIdx() == PlayerAction::CUTSCENE_001_02 && !actor->anim->IsPlaying())
			{
				ChangeAnim(AnimationState::ONCE, PlayerAction::IDLE, 0.0f);
				PLAYER_UI->startTextTime = 0.0f;
			}
		}
	}

	// 플레이어의 점프, 스텝, 이동
	{
		if (actionable_def && !isGuard && jumpCount > 0 && (INPUT->KeyDown(VK_SPACE) || key_buffer == VK_SPACE))
		{
			// 플레이어의 점프
			InitKeyBuffer();
			st -= 6.0f;
			YSpeed = GetJump(7.0f, 0.5f);
			isAir = true;
			airAttack = 0;
			jumpCount--;
			INPUT->SetVibration(0.03f, 0.0f, 0.1f);
			SOUND->Play(SDKEY::jump00, SDTYPE::SE, 1.0f, actor);
			ChangeAnim(AnimationState::ONCE, PlayerAction::JUMP);
		}
		else if (Check_MoveControl() || key_buffer == 'E')
		{
			// 플레이어의 이동행동
			Vector3 mov = (key_buffer == 'E') ? mov_buffer : GetMov();
			Vector3 dir = mov;
			dir.Normalize();

			if (isGuard && Check_Guard())
			{
				if ((actor->anim->PlayingIdx() == PlayerAction::GUARD || actor->anim->PlayingIdx() == PlayerAction::GUARD_AIR) &&
					(INPUT->KeyPress('R')))
					realDir = dir;
			}
			else if (actionable_def && stepCount > 0 && !Check_Step() && (INPUT->KeyDown('E') || key_buffer == 'E'))
			{
				// 스텝
				InitKeyBuffer();
				st -= 8.0f;
				realDir = dir;
				INPUT->SetVibration(0.03f, 0.05f, 0.05f);
				airAttack = 0;
				SOUND->Play(SDKEY::evade00, SDTYPE::SE, 1.0f, actor);
				if (isAir && actor->anim->PlayingIdx() != PlayerAction::STEP_AIR)
				{
					stepCount--;
					ChangeAnim(AnimationState::ONCE, PlayerAction::STEP_AIR, 0.05f);
				}
				else if (!isAir && actor->anim->PlayingIdx() != PlayerAction::STEP)
					ChangeAnim(AnimationState::ONCE, PlayerAction::STEP, 0.05f);
				actor->rotation.y = atan2(-dir.x, -dir.z);
			}
			else if ((movable || (st <= 0.0f && isAir && !Check_Attack())) && !isAttack && !Check_Step())
			{
				// 이동
				bool dash = !isAir && (INPUT->KeyPress(VK_LSHIFT) || (actor->anim->isChanging && actor->anim->PlayingIdx() == PlayerAction::DASH)) &&
					!(actor->anim->isChanging && actor->anim->PlayingIdx() == PlayerAction::MOVE);
				float speed = dash ? 25.0f : 20.0f;
				if (dash) st -= 5.0f * DELTA;
				realDir = dir;
				isMoved = true;
				if (!isAir)
				{
					if (ChangeAnim(AnimationState::LOOP, dash ? PlayerAction::DASH : PlayerAction::MOVE))
					{
						if ((dash && actor->anim->currentAnimator.animIdx == PlayerAction::MOVE) ||
							(!dash && actor->anim->currentAnimator.animIdx == PlayerAction::DASH))
						{
							actor->anim->nextAnimator.currentFrame = actor->anim->currentAnimator.currentFrame;
							actor->anim->nextAnimator.nextFrame = actor->anim->currentAnimator.nextFrame;
						}
					}
				}
				if (actor->anim->PlayingIdx() == PlayerAction::MOVE || actor->anim->PlayingIdx() == PlayerAction::DASH)
					actor->anim->aniScale = (speed / 10.5f) * mov.Length(); // mov.Length(): 0 ~ 1
				mov *= speed * DELTA;
				pos += { mov.x, 0.0f, mov.z };
			}
		}
	}

	// 지상상태에서 이동 외 특별한 조작이 없으면 통상모션으로 변경
	if (st >= st_max && actor->anim->PlayingIdx() == PlayerAction::IDLE_BREATH)
		actor->anim->currentAnimator.animState = actor->anim->nextAnimator.animState = AnimationState::STOP;
	if (!isAir && !Check_Death() && !Check_CutScene() && (
		(!isMoved && (actor->anim->PlayingIdx() == PlayerAction::MOVE || actor->anim->PlayingIdx() == PlayerAction::DASH)) ||
		(!isGuard && Check_Guard()) || 
		!actor->anim->IsPlaying()))
	{
		if (hp <= 0.0f)
		{
			if (PLAYER_UI->menu == MENU::ITEM) PLAYER_UI->menu = MENU::NORMAL; //아이템창을 닫는다
			ChangeAnim(AnimationState::ONCE, PlayerAction::DIE);
		}
		else if (st <= 0.0f)
		{
			if (PLAYER_UI->menu == MENU::ITEM) PLAYER_UI->menu = MENU::NORMAL; //아이템창을 닫는다
			ChangeAnim(AnimationState::LOOP, PlayerAction::IDLE_BREATH);
		}
		else
		{
			actionable_def = true;
			ChangeAnim(AnimationState::LOOP, GetHpRatio() > 0.3f ? PlayerAction::IDLE : PlayerAction::IDLE_CAPEK);
		}
	}

	//피격시
	{
		bool death = false;
		if (damageInfo.size() > 0)
		{
			int n;
			int guard_success = 0;	// 0:피격, 1:가드, 2:퍼펙트가드

			if (PLAYER_UI->menu == MENU::ITEM) PLAYER_UI->menu = MENU::NORMAL; //피격되면 메뉴를 닫는다
			while (damageInfo.size() > 0)
			{
				vector<int> temp;
				for (int i = 0; i < damageInfo.size(); i++)
				{
					if (damageInfo[i].kb_velocity.Length() > 0)
						temp.push_back(i);
				}
				// 넉백되는 공격이 있으면 그것을 우선시한다
				if (temp.size() > 0)
					n = temp[RANDOM->Int(0, temp.size() - 1)];
				else
					n = RANDOM->Int(0, damageInfo.size() - 1);
				temp.clear();

				// 피해내역 중 하나만 골라 적용
				Vector3 dir = damageInfo[n].atkPos - pos;
				dir.Normalize();

				INPUT->SetVibration(0.7f, 0.0f, 0.3f);

				{
					Vector3 spawnPos = pos + (realDir * 0.6f) + Vector3(0.0f, 2.4f, 0.0f);
					Pop* temp = dynamic_cast<Pop*>(GAMEOBJ->Generate(PTCTYPE::Effect_Hit, spawnPos)->GetActor());
					temp->rotation.y = atan2(-dir.x, -dir.z);
					temp->rotRange = [=]() { return Vector3(0.0f, RANDOM->Float(-PI_DIV4, PI_DIV4), temp->DefaultRot()); };
					temp->scale = { 0.1f,0.1f,0.1f };
					temp->material->diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
					temp->duration = 0.3f;
					temp->desc.gravity = 0.0f;
					temp->particleCount = 20;
					temp->velocityScalar = 15.0f;
					temp->Play();
				}

				if (isGuard && guardTime >= 0.1f)
				{
					float pRot = GetRealRot();
					float hitRot = atan2(-dir.x, -dir.z);
					if (fabs((pRot + PI_2) - hitRot) < fabs(pRot - hitRot))
					{
						hitRot -= PI_2;
					}
					else if (fabs((pRot - PI_2) - hitRot) < fabs(pRot - hitRot))
					{
						hitRot += PI_2;
					}

					if (fabs(pRot - hitRot) <= PI_DIV2)
					{
						int tempDmg;
						
						// 1. 가드 판정
						if (guardTime <= 0.2f)
						{
							guard_success = 2;
							tempDmg = 0.0f;
						}
						else
						{
							guard_success = 1;
							tempDmg = static_cast<int>(ceilf((float)damageInfo[n].damage * 0.3f));
							st -= 5.0f;
						}

						if (hp - tempDmg <= 0.0f || st <= 0.0f)
						{
							Decrease_HP(static_cast<int>(ceilf((float)damageInfo[n].damage * 0.65f)));
							guard_success = 0;
						}
						else
							Decrease_HP(tempDmg);

						// 2. 퍼펙트 가드 시 이펙트 추가
						if (guard_success >= 2)
						{
							Vector3 spawnPos = pos + (realDir * 0.6f) + Vector3(0.0f, 2.4f, 0.0f);
							Pop* temp = dynamic_cast<Pop*>(GAMEOBJ->Generate(PTCTYPE::Effect_Hit, spawnPos)->GetActor());
							temp->rotation.y = pRot;
							temp->rotRange = [=]() { return Vector3(0.0f, RANDOM->Float(-PI_DIV4, PI_DIV4), temp->DefaultRot()); };
							temp->scale = { 0.3f,0.3f,0.3f };
							temp->material->diffuse = Color(1.0f, 1.0f, 0.0f, 1.0f);
							temp->duration = 0.5f;
							temp->desc.gravity = 10.0f;
							temp->particleCount = 30;
							temp->velocityScalar = 15.0f;
							temp->Play();
						}

						// 3. 가드 사운드 출력
						if (guard_success >= 2)
							SOUND->Play(SDKEY::guard_perfect, SDTYPE::SE, 1.0f, actor);
						else if (guard_success >= 1)
							SOUND->Play(damageInfo[n].guardSound, SDTYPE::SE, 1.0f, actor);

						// 4. 무적 판정 (피격 또는 퍼펙트가드 시)
						if (guard_success == 0 || guard_success >= 2)
						{
							if (guard_success >= 2) invincibleTime = 0.5f;
							break;
						}
					}
					else
					{
						Decrease_HP(damageInfo[n].damage);
						guard_success = 0;
						break;
					}
				}
				else
				{
					Decrease_HP(damageInfo[n].damage);
					guard_success = 0;
					break;
				}
				damageInfo.erase(damageInfo.begin() + n);
			}

			if (guard_success >= 1)
			{
				actor->rotation.y = GetRealRot();
				if (isAir)
				{
					ChangeAnim(AnimationState::ONCE, PlayerAction::GUARDHIT_AIR, 0.05f);
					YSpeed = GetJump(1.0f, 0.1f);
				}
				else
				{
					// 제자리에서 피격되는 모션
					ChangeAnim(AnimationState::ONCE, PlayerAction::GUARDHIT, 0.05f);
				}
			}
			else
			{
				Vector3 dir = damageInfo[n].atkPos - pos;
				dir.Normalize();

				realDir = dir;
				actor->rotation.y = atan2(-dir.x, -dir.z);

				SOUND->Play(damageInfo[n].hitSound, SDTYPE::SE, 1.0f, actor);

				if (hp <= 0.0f)
					death = true;

				if (damageInfo[n].kb_velocity.Length() > 0)
				{
					// 맞고 날아가는 모션 (소경직 피격이어도 공중에 있으면 넉백된다)
					ChangeAnim(AnimationState::ONCE, PlayerAction::DOWN, 0.05f);
					kb_speed = fabs(damageInfo[n].kb_velocity.x);
					YSpeed = fabs(damageInfo[n].kb_velocity.y);
					isAir = true;
				}
				else if (isAir || st <= 0.0f)
				{
					ChangeAnim(AnimationState::ONCE, PlayerAction::DOWN, 0.05f);
					kb_speed = 15.0f;
					YSpeed = GetJump(5.0f, 0.5f);
					isAir = true;
				}
				else if(!death)
				{
					// 제자리에서 피격되는 모션
					ChangeAnim(AnimationState::ONCE, PlayerAction::HIT, 0.05f);
					actor->anim->aniScale = 1.5f;
				}
				invincibleTime = 0.2f;
			}
			damageInfo.clear();
		}

		if (death)
		{
			PLAYER_UI->SetDying(1);
			if(!isAir) ChangeAnim(AnimationState::ONCE, PlayerAction::DIE, 0.05f);
		}
	}

	// 피격되고 날아가고 있는 상태인가?
	if (actor->anim->PlayingIdx() == PlayerAction::DOWN)
	{
		pos += actor->GetForward() * kb_speed * DELTA;
	}

	// 무적판정
	// 무적시간 적용중이거나, 이미 리타이어했거나, 시네마틱이 진행중일 때 무적
	if (invincibleTime > 0.0f || Check_Death() || PLAYER_UI->GetCinematicLevel() > 0 || PLAYER_UI->IsGameEnding() || Check_CutScene(true,true))
	{
		Invincible(true);
		if (actor->anim->PlayingIdx() != PlayerAction::HIT &&
			actor->anim->PlayingIdx() != PlayerAction::DOWN && 
			actor->anim->PlayingIdx() != PlayerAction::DOWN_LAND &&
			!Check_Death())
			invincibleTime -= DELTA;
	}
	else
	{
		Invincible(false);
	}

	// 스태미나 회복
	if (st > 0.0f || actor->anim->PlayingIdx() == PlayerAction::IDLE_BREATH || Check_Hit())
	{
		if (!Check_Step() && !Check_Guard() && !Check_Attack() &&
			actor->anim->PlayingIdx() != PlayerAction::DASH &&
			actor->anim->PlayingIdx() != PlayerAction::JUMP &&
			actor->anim->PlayingIdx() != PlayerAction::FALL)
		{
			st += (actor->anim->PlayingIdx() == PlayerAction::IDLE_BREATH ? 40.0f : 20.0f) * DELTA;
		}
	}
	else
		st = 0.0f;
	if (st > st_max) st = st_max;
	if (hp > hp_max) hp = hp_max;

	// 애니메이션에 따른 위치이동
	//pos += Vector3::Transform(actor->animOffset, actor->R);
	pos += Vector3::Transform(actor->animOffset, Matrix::CreateFromAxisAngle({0.0f,1.0f,0.0f},actor->rotation.y));

	// 자유낙하
	if (isAir)
	{
		// 특정 모션은 공중부양한다.
		if (actor->anim->PlayingIdx() == PlayerAction::STEP_AIR ||
			(actor->anim->PlayingIdx() == PlayerAction::ATTACK_AirA1 && actor->anim->GetPlayTime() <= 35.0f / 80.0f) ||
			(actor->anim->PlayingIdx() == PlayerAction::ATTACK_AirA2 && actor->anim->GetPlayTime() <= 30.0f / 100.0f) ||
			(actor->anim->PlayingIdx() == PlayerAction::ATTACK_AirA3 && actor->anim->GetPlayTime() <= 50.0f / 90.0f))
			YSpeed = 0.0f;
		else if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_AirB1 || actor->anim->PlayingIdx() == PlayerAction::ATTACK_AirB1_FALL)
		{
			YSpeed = 0.0f;
			if (!attackStop && !(actor->anim->PlayingIdx() == PlayerAction::ATTACK_AirB1 && actor->anim->GetPlayTime() <= 55.0f / 65.0f))
			{
				float XSpeed = 50.0f;
				YSpeed = -XSpeed / 2.0f;
				pos += -actor->GetForward() * XSpeed * DELTA;
				pos.y += YSpeed * DELTA;
			}
		}
		else
		{
			YSpeed -= SCENE->gravity * DELTA;
			pos.y += YSpeed * DELTA;
			if (YSpeed < YminSpeed) YSpeed = YminSpeed;
		}
	}

	// 공중상태에서 특별한 조작이 없으면 낙하모션으로 변경
	if (isAir && !Check_CutScene() && actor->anim->PlayingIdx() != PlayerAction::DOWN && actor->anim->PlayingIdx() != PlayerAction::ATTACK_AirB1_FALL && (
		(!isGuard && Check_Guard()) ||
		(!actor->anim->isChanging && actor->anim->currentAnimator.animState == AnimationState::STOP)))
		ChangeAnim(AnimationState::LOOP, PlayerAction::FALL, actor->anim->PlayingIdx() == PlayerAction::JUMP ? 0.35f : 0.2f);

	// 시네마틱 상태일 때 특수모션 적용
	if (PLAYER_UI->b_singalForCinematic)
	{
		switch (PLAYER_UI->cinematicID)
		{
		case 1: ChangeAnim(AnimationState::STOP, PlayerAction::CINEMATIC_001_01, 0.0f, true); break;
		}
	}

	float rotSpeed;
	if (isGuard && isAir)
		rotSpeed = 10.0f * PI;
	else
		rotSpeed = 4.0f * PI;

	// 최종이동 및 회전
	if (Check_CutScene())
	{
		actor->rotation.y = GetRealRot();
		if (Check_CutScene(true, true)) MoveOnTerrain(pos);
	}
	else
	{
		Rotate(realDir, rotSpeed);
		MoveOnTerrain(pos);
	}

	if (!lastAir && isAir && 
		actor->anim->PlayingIdx() != PlayerAction::JUMP &&
		actor->anim->PlayingIdx() != PlayerAction::DOWN &&
		actor->anim->PlayingIdx() != PlayerAction::DOWN_LAND &&
		!Check_Guard() && !Check_Death() && !Check_CutScene())
	{
		// 고저차로 인한 자유낙하시
		jumpCount = 1;

		ChangeAnim(AnimationState::LOOP, PlayerAction::FALL);
	}
	else if (lastAir && !isAir)
	{
		// 땅에 착지할 시
		bool b_playFootstep = false;
		jumpCount = 2;
		if (isGuard)
		{
			b_playFootstep = true;
			ChangeAnim(AnimationState::ONCE, PlayerAction::GUARD_LAND, 0.1f);
			actor->anim->aniScale = 2.5f;
		}
		else if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_AirB1_FALL)
			ChangeAnim(AnimationState::ONCE, PlayerAction::ATTACK_AirB1_LAND, atkNode[PlayerAction::ATTACK_AirB1_LAND].blend);
		else if (actor->anim->PlayingIdx() == PlayerAction::DOWN)
		{
			ChangeAnim(AnimationState::ONCE, (hp <= 0.0f) ? PlayerAction::DIE_LAND : PlayerAction::DOWN_LAND, 0.05f);
		}
		else if(actor->anim->PlayingIdx() != PlayerAction::DOWN_LAND && !Check_Death() && !Check_CutScene())
		{
			b_playFootstep = true;
			if (hp <= 0.0f)
				ChangeAnim(AnimationState::ONCE, PlayerAction::DIE, 0.05f);
			else if (st <= 0.0f)
				ChangeAnim(AnimationState::LOOP, PlayerAction::IDLE_BREATH);
			else
				ChangeAnim(AnimationState::ONCE, PlayerAction::FALL_LAND, 0.05f);
		}
		if(b_playFootstep)
			SOUND->Play(SDKEY_FootstepA(), SDTYPE::SE, 0.5f, pos);
	}
	if (!isAir)
	{
		stepCount = 1;
		jumpCount = 2;
		airGuard = false;
		airAttack = 0;
	}

	// 클리어 전용모션
	if (PLAYER_UI->IsGameEnding() && !isAir && (actor->anim->PlayingIdx() == PlayerAction::IDLE || actor->anim->PlayingIdx() == PlayerAction::IDLE_BREATH || actor->anim->PlayingIdx() == PlayerAction::IDLE_CAPEK))
		ChangeAnim(AnimationState::ONCE, PlayerAction::CUTSCENE_002_01, 0.0f);

	// 모션에 따른 플레이어의 상태변경 (movable, actionable_atk+def, can_input, ghost)
	{
		if (hp <= 0.0f || st <= 0.0f || Check_Hit() || Check_Guard() || Check_Attack() || Check_Step() || Check_CutScene() || VAR->playLock ||
			PLAYER_UI->IsGameEnding() || airGuard || airAttack || actor->anim->PlayingIdx() == PlayerAction::IDLE_BREATH || actor->anim->PlayingIdx() == PlayerAction::USE_ITEM ||
			(actor->anim->isChanging && (actor->anim->currentAnimator.animIdx == PlayerAction::GUARDHIT || actor->anim->currentAnimator.animIdx == PlayerAction::GUARDHIT_AIR)))
		{
			if (hp <= 0.0f || st <= 0.0f)
				movable = false;
			else if (Check_Attack() && !isAir && actor->anim->GetPlayTime() > atkNode[actor->anim->PlayingIdx()].comboTime)
				movable = true;
			else
				movable = false;
		}
		else
			movable = true;

		if (hp <= 0.0f || st <= 0.0f || PLAYER_UI->menu == MENU::ITEM || Check_Hit() || Check_Step() || Check_Attack() || Check_CutScene() || VAR->playLock ||
			PLAYER_UI->IsGameEnding() || airGuard || actor->anim->PlayingIdx() == PlayerAction::IDLE_BREATH || actor->anim->PlayingIdx() == PlayerAction::USE_ITEM ||
			(actor->anim->isChanging && (
				(actor->anim->currentAnimator.animIdx == PlayerAction::GUARDHIT && actor->anim->nextAnimator.animIdx != PlayerAction::GUARD) ||
				(actor->anim->currentAnimator.animIdx == PlayerAction::GUARDHIT_AIR && !(actor->anim->nextAnimator.animIdx == PlayerAction::GUARD_AIR || actor->anim->nextAnimator.animIdx == PlayerAction::GUARD_LAND)))
				))
		{
			if (hp <= 0.0f || st <= 0.0f || PLAYER_UI->menu == MENU::ITEM)
				actionable_def = false;
			else if ((actor->anim->PlayingIdx() == PlayerAction::DOWN_LAND && actor->anim->GetPlayTime() >= 34.0f / 60.0f) ||
				(Check_Attack() && actor->anim->GetPlayTime() >= atkNode[actor->anim->PlayingIdx()].actionTime) ||
				(Check_Step() && actor->anim->GetPlayTime() >= 15.0f / 25.0f))
				actionable_def = true;
			else
				actionable_def = false;
		}
		else
		{
			actionable_def = true;
		}

		if (hp <= 0.0f || st <= 0.0f || PLAYER_UI->menu == MENU::ITEM || Check_Hit() || Check_Step() || Check_Guard() || Check_Attack() || Check_CutScene() || VAR->playLock ||
			PLAYER_UI->IsGameEnding() || airGuard || actor->anim->PlayingIdx() == PlayerAction::IDLE_BREATH || actor->anim->PlayingIdx() == PlayerAction::USE_ITEM)
		{
			if (hp <= 0.0f || st <= 0.0f || PLAYER_UI->menu == MENU::ITEM)
				actionable_atk = false;
			else if ((Check_Step() && actor->anim->GetPlayTime() >= 20.0f / 25.0f) ||
				(Check_Attack() && actor->anim->GetPlayTime() >= atkNode[actor->anim->PlayingIdx()].actionTime))
				actionable_atk = true;
			else
				actionable_atk = false;
		}
		else
		{
			actionable_atk = true;
		}

		// 고스트판정
		if ((actor->anim->PlayingIdx() == PlayerAction::ATTACK_StepB1 && actor->anim->GetPlayTime() >= 1.0f / 80.0f && actor->anim->GetPlayTime() <= 8.0f / 80.0f) ||
			(actor->anim->PlayingIdx() == PlayerAction::ATTACK_AirA3 && actor->anim->GetPlayTime() >= 16.0f / 90.0f && actor->anim->GetPlayTime() <= 29.0f / 90.0f) ||
			Check_Death() || Check_CutScene(true, false))
			ghost = true;
		else
			ghost = false;

		if (Check_Step() || actor->anim->PlayingIdx() == PlayerAction::DOWN_LAND || (Check_Attack() && attackTime >= 0.05f))
		{
			if (actor->anim->PlayingIdx() == PlayerAction::ATTACK_AirB1 || 
				actor->anim->PlayingIdx() == PlayerAction::ATTACK_AirB1_FALL)
				can_input = false;
			else
				can_input = true;
		}
		else
			can_input = false;

		// 선입력
		if (can_input)
		{
			// 방어, 스텝, 점프
			int key[] = { 'R', 'E', VK_SPACE, VK_LBUTTON, VK_RBUTTON };

			for (int i = 0; i < (sizeof(key) / sizeof(*key)); i++)
			{
				if (INPUT->KeyDown(key[i]))
				{
					if (key[i] == 'E')
					{
						if (Check_MoveControl())
						{
							key_buffer = key[i];
							mov_buffer = GetMov();
							break;
						}
						else
							continue;
					}
					key_buffer = key[i];
					break;
				}
			}
		}
		else
		{
			key_buffer = 0;
			mov_buffer = { 0,0,0 };
		}
	}

	//------------------
	// 카메라 록온
	{
		Collider* temp = nullptr;
		Vector3 ndcPos;
		float ndcDist = FLT_MAX;
		bool visible;

		// 록온해제 설정(조건부)
		if (lockOnTarget)
		{
			auto it = GAMEOBJ->HitList.begin();
			while (it != GAMEOBJ->HitList.end())
			{
				if (it->collider == lockOnTarget)
					break;
				it++;
			}
			if (it == GAMEOBJ->HitList.end())
			{
				lockOnTarget = nullptr;
			}
			else
			{
				if (hp <= 0.0f || it->owner->GetHpRatio() <= 0.0f || Camera::GetMainCam() != camera || Vector3::Distance(actor->GetWorldPos(), lockOnTarget->GetWorldPos()) > 100.0f)
					lockOnTarget = nullptr;
			}

			if (!lockOnTarget && PLAYER_UI->GetCinematicLevel() != 2)
				SOUND->Play(SDKEY::interface_lockOff, SDTYPE::SE, 1.0f);
		}

		// 록온 표시여부
		if (lockOnTarget)
		{
			Matrix W = Matrix::CreateTranslation(lockOnTarget->GetWorldPos());
			Matrix Matrixtemp = W * Camera::GetFollowCam()->view * Camera::GetFollowCam()->proj;
			//Vector3 center = { 0.5f,0.5f,0.0f };
			Matrixtemp /= Matrixtemp._44;

			ndcPos = { (Matrixtemp._41),(Matrixtemp._42),0.0f };
			visible = Matrixtemp._43 < 1.0f;
			lockOnAnyAngleTime = lockOnAnyAngleMaxTime;
		}
		else
		{
			for (auto it = GAMEOBJ->HitList.begin(); it != GAMEOBJ->HitList.end(); it++)
			{
				bool cond = CONFIG->settings.b_lockOnParts ?
					it->collider->gameType & (int)GameType::LOCKON_MON_PARTS :
					it->collider->gameType & (int)GameType::LOCKON_MON;
				if (it->owner->GetHpRatio() > 0.0f && cond)
				{
					Vector3 tempPos;
					Matrix W = Matrix::CreateTranslation(it->collider->GetWorldPos());
					Matrix Matrixtemp = W * Camera::GetFollowCam()->view * Camera::GetFollowCam()->proj;
					Matrixtemp /= Matrixtemp._44;
					tempPos = { (Matrixtemp._41),(Matrixtemp._42),0.0f };

					Vector3 center = { 0.0f,0.0f,0.0f };
					//Matrix centerW = Matrix::CreateTranslation(actor->Find("MoveCollider")->GetWorldPos());
					//Matrix MatrixCenter = centerW * Camera::GetFollowCam()->view * Camera::GetFollowCam()->proj;
					//MatrixCenter /= MatrixCenter._44;
					//center = { (MatrixCenter._41),(MatrixCenter._42),0.0f };

					if ((lockOnAnyAngleTime > 0.0f || (Matrixtemp._43 < 1.0f && tempPos.x > -0.8f && tempPos.x < 0.8f && tempPos.y > -0.8f && tempPos.y < 0.8f)) && Vector3::Distance(actor->GetWorldPos(), it->collider->GetWorldPos()) < 60.0f && Vector3::Distance(center, tempPos) < ndcDist)
					{
						Vector3 targetPos = it->collider->GetWorldPos();
						Vector3 toTargetDir = targetPos - pos;
						toTargetDir.Normalize();
						Ray toTargetRay = Ray(pos, toTargetDir);

						Object* ob;
						int col_Idx = 0;
						Vector3 Hit;

						int idx = Util::GetMyStandMap(pos);			// 자신이 서있는 맵의 번호
						int midx = Util::GetMyStandMap(targetPos);	// 타겟이 서있는 맵의 번호
						int cidx = idx;							// 현재 검사중인 맵의 번호
						while (idx >= 0 && midx >= 0)
						{
							col_Idx++;
							if (ob = (*SCENE->Map)[cidx]->Find("Col" + to_string(col_Idx)))
							{
								if (!(ob->collider->gameType & (int)GameType::ONLY_PLAYER) && // 플레이어전용 col은 무시한다
									ob->collider->Intersect(toTargetRay, Hit))
								{
									// 자신과 타겟사이를 가로막는 벽이 있다.
									if (Vector3::Distance(pos, Hit) < Vector3::Distance(pos, targetPos))
										break;
								}
							}
							else
							{
								if (cidx == midx)
								{
									// 자신과 타겟사이를 가로막는 벽이 없다.
									temp = it->collider;
									visible = Matrixtemp._43 < 1.0f;
									ndcDist = Vector3::Distance(center, tempPos);
									ndcPos = tempPos;
									break;
								}
								else
								{
									// 몬스터가 있는 맵의 번호에서도 충돌확인
									cidx = midx;
									col_Idx = 0;
								}
							}
						}
					}
				}
			}
			lockOnAnyAngleTime -= DELTA_NS;
		}

		if (hp > 0.0f && Camera::GetMainCam() == camera && (temp || lockOnTarget))
		{
			PLAYER_UI->lock_mark->SetLocalPos(ndcPos);
			PLAYER_UI->lock_mark->visible = visible;
			if (INPUT->KeyDown(VK_MBUTTON))
			{
				if (lockOnTarget)
				{
					SOUND->Play(SDKEY::interface_lockOff, SDTYPE::SE, 1.0f);
					lockOnTarget = nullptr;
				}
				else
				{
					SOUND->Play(SDKEY::interface_lockOn, SDTYPE::SE, 1.0f);
					lockOnTarget = temp;
				}
			}
		}
		else
		{
			PLAYER_UI->lock_mark->visible = false;
		}
	}

	// 카메라 이동
	{
		camPivot->SetWorldPos(actor->GetWorldPos());

		if (Camera::GetMainCam() == camera && !PLAYER_UI->IsGameEnding())
		{
			// ---입력을 통한 각도조작---
			Vector3 camRot = { 0.0f,0.0f,0.0f };
			const float camSpeed = PI_DIV2;
			float speedScale = 1.0f;
			float zoomMin, zoomMax, yMin, yMax, relRotY, rightGap;

			// 마우스(3순위)
			if (!INPUT->usePad)
			{
				ConfigManager::CameraSettingsStruct& tg = CONFIG->cameraSettings[DeviceState::KEYBOARD];
				speedScale = tg.speedSettingVector[CONFIG->settings.Idx_speedSetting[DeviceState::KEYBOARD]].second;
				camRot.y += INPUT->relPosition.x * (CONFIG->settings.b_reverseDirHorizontal[DeviceState::KEYBOARD] ? -1.0f : 1.0f);
				camRot.x -= INPUT->relPosition.y * (CONFIG->settings.b_reverseDirVertical[DeviceState::KEYBOARD] ? -1.0f : 1.0f);
				camRot.y = Util::Saturate(camRot.y, -1.0f, 1.0f);
				camRot.x = Util::Saturate(camRot.x, -1.0f, 1.0f);
			}

			// 키보드(2순위)
			if (INPUT->KeyPress('I') || INPUT->KeyPress('J') || INPUT->KeyPress('K') || INPUT->KeyPress('L'))
			{
				ConfigManager::CameraSettingsStruct& tg = CONFIG->cameraSettings[DeviceState::KEYBOARD];
				speedScale = tg.speedSettingVector[CONFIG->settings.Idx_speedSetting[DeviceState::KEYBOARD]].second;
				camRot = { 0.0f,0.0f,0.0f };
				if (INPUT->KeyPress('J')) camRot.y -= 1.0f;
				else if (INPUT->KeyPress('L')) camRot.y += 1.0f;
				if (INPUT->KeyPress('I')) camRot.x += 1.0f;
				else if (INPUT->KeyPress('K')) camRot.x -= 1.0f;

				camRot.y *= CONFIG->settings.b_reverseDirHorizontal[DeviceState::KEYBOARD] ? -1.0f : 1.0f;
				camRot.x *= CONFIG->settings.b_reverseDirVertical[DeviceState::KEYBOARD] ? -1.0f : 1.0f;
			}

			// 컨트롤러(1순위)
			if (fabs(INPUT->tracker.GetLastState().thumbSticks.rightX) > 0.0f || fabs(INPUT->tracker.GetLastState().thumbSticks.rightY) > 0.0f)
			{
				ConfigManager::CameraSettingsStruct& tg = CONFIG->cameraSettings[DeviceState::CONTROLLER];
				speedScale = tg.speedSettingVector[CONFIG->settings.Idx_speedSetting[DeviceState::CONTROLLER]].second;
				camRot = { 0.0f,0.0f,0.0f };
				camRot.y += INPUT->tracker.GetLastState().thumbSticks.rightX * (CONFIG->settings.b_reverseDirHorizontal[DeviceState::CONTROLLER] ? -1.0f : 1.0f);
				camRot.x += INPUT->tracker.GetLastState().thumbSticks.rightY * (CONFIG->settings.b_reverseDirVertical[DeviceState::CONTROLLER] ? -1.0f : 1.0f);
				if (camRot.Length() > 1.0f) camRot.Normalize();
			}
			camRot *= camSpeed * speedScale * DELTA_NS;

			Vector3 toTargetDir;
			if (lockOnTarget)
			{
				toTargetDir = lockOnTarget->GetWorldPos() - pos;
				toTargetDir.Normalize();
			}

			camPivot->Find("CameraPivot")->rotation.x += camRot.x;
			if (lockOnTarget)
			{
				float startRot = camPivot->Find("CameraPivot")->rotation.y;
				float endRot = atan2(-toTargetDir.x, -toTargetDir.z);
				if (fabs((endRot + PI_2) - startRot) < fabs(endRot - startRot))
				{
					startRot -= PI_2;
				}
				else if (fabs((endRot - PI_2) - startRot) < fabs(endRot - startRot))
				{
					startRot += PI_2;
				}
				//float w = 1.0f;	// w수치가 상시 1.0라면 록온대상의 머리 위로 올라가는 순간 사방을 회전하게 되므로 아래처럼 거리에 따른 비례보정치를 준다.
				//타겟과의 각도차이를 기준으로 각도0~180에서 t0~1의 효과를 내는 커프그래프를 작성해볼까 ? ;
				//x < 0.5 ? 4 * x * x * x : 1 - Math.pow(-2 * x + 2, 3) / 2; // easeInOutCubic
				float t = 1.0f - (fabs(endRot - startRot) / PI);
				t = Util::Saturate(t, 0.1f, 1.0f);
				const float distMin = CONFIG->lockOnSpeedSettingVector[CONFIG->settings.idx_lockOnSpeedSetting].second.distMin;	// 록온회전이 적용받는 최소거리
				const float distMax = CONFIG->lockOnSpeedSettingVector[CONFIG->settings.idx_lockOnSpeedSetting].second.distMax - distMin;	// 록온회전이 최대치가 되는 최대거리
				assert(distMax >= 0.0f);
				float lockOnDist = Vector2::Distance({ pos.x,pos.z }, { lockOnTarget->GetWorldPos().x, lockOnTarget->GetWorldPos().z });	//록온 목표까지의 거리
				float w = (lockOnDist - distMin) / distMax;
				w = Util::Saturate(w, 0.0f, 1.0f);
				//camPivot->Find("CameraPivot")->rotation.y = Util::Lerp(startRot, endRot, lockOnSpeed * DELTA_NS * w);
				t += DELTA_NS / 0.5f;
				t *= w;
				float value = (t < 0.5) ? (4 * pow(t,3)) : (1 - pow(-2 * t + 2, 3) / 2);
				camPivot->Find("CameraPivot")->rotation.y = Util::Lerp(startRot, endRot, value);
			}
			else
				camPivot->Find("CameraPivot")->rotation.y += camRot.y;
			camPivot->Find("CameraPivot")->rotation.y = Util::NormalizeAngle(camPivot->Find("CameraPivot")->rotation.y);
			if (camPivot->Find("CameraPivot")->rotation.x < -40.0f * TORADIAN) camPivot->Find("CameraPivot")->rotation.x = -40.0f * TORADIAN;
			else if (camPivot->Find("CameraPivot")->rotation.x > 40.0f * TORADIAN) camPivot->Find("CameraPivot")->rotation.x = 40.0f * TORADIAN;


			if (INPUT->KeyDown('Q') && PLAYER_UI->menu == MENU::NORMAL && !lockOnTarget)
			{
				//캐릭터가 바라보는 방향으로 카메라 초기화
				camPivot->Find("CameraPivot")->rotation = { 0.0f,GetRealRot() ,0.0f };
			}
			// --------------

			// ---각도에 따른 카메라 좌표보정---
			if (PLAYER_UI->dying == 0)
			{
				// y각도에 따른 수평위치조정(캐릭터가 바라보는 방향으로 공간을 확보)
				relRotY = (GetRealRot() - camPivot->Find("CameraPivot")->rotation.y);
				relRotY = Util::NormalizeAngle(relRotY);
				//ImGui::Text("relRotY: %f", relRotY / TORADIAN);
				//float temp = camPivot->Find("CameraPivot")->rotation.x;
				//rightGap = -2.0f * (1.0f - temp / (40.0f * TORADIAN));
				//Util::Saturate(rightGap, -2.0f, 0.0f);
				rightGap = -2.0f;
				camPivot->Find("CameraPivot")->SetLocalPos(camPivot->Find("CameraPivot")->GetRight() * rightGap * sinf(relRotY));


				// x각도에 따른 수직위치조정(캐릭터가 잘 보이게)
				yMin = 3.0f + 1.0f * (camPivot->Find("CameraPivot")->rotation.x + 40.0f * TORADIAN) / (80.0f * TORADIAN);
				Util::Saturate(yMin, 3.0f, 4.0f);
				yMax = 3.5f + 1.0f * (camPivot->Find("CameraPivot")->rotation.x) / (40.0f * TORADIAN);
				Util::Saturate(yMax, 3.5f, 4.5f);
				camPivot->Find("CameraPivot")->SetLocalPosY(yMin + (yMax - yMin) * CONFIG->GetZoom());
			}
			else
			{
				camPivot->Find("CameraPivot")->SetLocalPos({ 0.0f,2.0f,0.0f });
			}

			if (INPUT->KeyDown(VK_PRIOR)) CONFIG->settings.Idx_zoomSetting--;
			else if (INPUT->KeyDown(VK_NEXT)) CONFIG->settings.Idx_zoomSetting++;
			Util::Saturate<int>(CONFIG->settings.Idx_zoomSetting, 0, CONFIG->zoomSettingVector.size() - 1);

			// 줌 비율 및 x각도에 따른 카메라 거리 조절(적당한 거리 유지)
			//zoomMin = 7.0f - 2.0f * fabs(camPivot->Find("CameraPivot")->rotation.x) / (40.0f * TORADIAN);
			//zoomMin = 7.0f - 2.0f * sinf(fabs(camPivot->Find("CameraPivot")->rotation.x) * 180.0f / 80.0f);
			zoomMin = 7.0f;
			//Util::Saturate(zoomMin, 5.0f, 7.0f);
			zoomMax = 10.0;
			camera->SetLocalPosZ(zoomMin + (zoomMax - zoomMin) * CONFIG->GetZoom());
		}
	}
}

void Player::Render()
{
	if (!Enable) return;
	actor->Render();
	trail->Render();
}

void Player::CubeMapRender()
{
	if (!Enable) return;
	actor->CubeMapRender();
}

void Player::ShadowMapRender()
{
	if (!Enable) return;
	actor->ShadowMapRender();
}

void Player::Init()
{
	hp_max = hp = 100;
	st_max = st = 100;
	weight = 1;
	resurrectionTime = 0.0f;

	// 게임 시작할때만 초기화
	itemList.clear();	
	itemList.push_back(make_pair(ItemManager::ItemType::RED, 10));
	itemList.push_back(make_pair(ItemManager::ItemType::PURPLE, 3));
	itemList.push_back(make_pair(ItemManager::ItemType::DUMMY1, 2));
	itemList.push_back(make_pair(ItemManager::ItemType::DUMMY2, 2));
	current_itemIdx = 0;

	Reset();
	ChangeAnim(AnimationState::LOOP, PlayerAction::TITLE_POSE, 0.0f);
}

void Player::Spawn(Object* point)
{
	realDir = point->GetForward();
	//camPivot->Find("CameraPivot")->rotation.y = point->rotation.y + PI;
	camPivot->Find("CameraPivot")->rotation = { 0.0f,GetRealRot() ,0.0f };
	GameObject::Spawn(point);
}

void Player::RenderDetail()
{
	ImGui::Text("Player");
	ImGui::Text("hp: %d", hp);
	ImGui::Text("st: %f", st);
	if (ImGui::Button("heal"))
	{
		hp = hp_max;
		st = st_max;
	}
	if (ImGui::Button("weak"))
	{
		hp = 5;
	}
	ImGui::Text("guardTime: %f", guardTime);
	ImGui::Text("attackTime: %f", attackTime);
	ImGui::Text("movable: %s", movable ? "true" : "false");
	ImGui::Text("actionable_atk: %s", actionable_atk ? "true" : "false");
	ImGui::Text("actionable_def: %s", actionable_def ? "true" : "false");
}

void Player::Reset()
{
	movable = false;
	actionable_atk = false;
	actionable_def = false;
	can_input = false;
	key_buffer = 0;
	mov_buffer = { 0,0,0 };
	isAir = false;
	ghost = false;
	YSpeed = 0.0f;
	jumpCount = 2;
	stepCount = 1;
	airGuard = false;
	airAttack = 0;
	guardTime = 0.0f;
	invincibleTime = 0.0f;
	attackTime = 0.0f;
	attackStopTime = 0.0f;
	attackStop = false;
	Swing = 0;
	Swing2 = 0;
	effectTime = 0.0f;
	lockOnTarget = nullptr;
	lockOnAnyAngleTime = 0.0f;
}

void Player::Start(Object* point)
{
	Spawn(point);
	ChangeAnim(AnimationState::ONCE, PlayerAction::CUTSCENE_001_01, 0.0f);
	Update();
}

Vector3 Player::GetMov()
{
	Vector3 mov = { 0.0f,0.0f,0.0f };
	if (Check_MoveControl())
	{
		Vector3 camForward = Camera::GetMainCam()->GetForward();
		camForward.y = 0.0f;
		camForward.Normalize();
		Vector3 camRight = Camera::GetMainCam()->GetRight();
		camRight.y = 0.0f;
		camRight.Normalize();

		if (INPUT->KeyPress('A')) mov -= camRight;
		else if (INPUT->KeyPress('D')) mov += camRight;
		if (INPUT->KeyPress('W')) mov += camForward;
		else if (INPUT->KeyPress('S')) mov -= camForward;
		mov.Normalize();

		if (fabs(INPUT->tracker.GetLastState().thumbSticks.leftX) > 0.0f || fabs(INPUT->tracker.GetLastState().thumbSticks.leftY) > 0.0f)
		{
			mov = { 0.0f,0.0f,0.0f };
			mov += camRight * INPUT->tracker.GetLastState().thumbSticks.leftX;
			mov += camForward * INPUT->tracker.GetLastState().thumbSticks.leftY;
			if (mov.Length() > 1.0f) mov.Normalize();
		}

		//cout << "(" << mov.x << ", " << mov.z << ") distance = " << mov.Length() << endl;
	}
	else
		mov = -actor->GetForward();
	return mov;
}

void Player::AttackToMonster(Collider* atkCol, int dmg, int knock, Sound::SoundKey hitSound, Sound::SoundKey guardSound)
{
	atkCol->atk.damage = dmg;
	atkCol->atk.knockdown = knock;
	atkCol->atk.hitSound = hitSound;
	atkCol->atk.guardSound = guardSound;
	if (atkCol->enable)
	{
		if (Attack(atkCol, GameType::HIT_MONSTER))
		{
			float stopTime = atkNode[actor->anim->PlayingIdx()].stopTime;
			INPUT->SetVibration(0.1f, 0.0f, (stopTime > 0.05f) ? stopTime : 0.05f);
			if (stopTime > 0.0f)
			{
				attackStopTime = atkNode[actor->anim->PlayingIdx()].stopTime;
				actor->anim->aniScale = 0.0f;
				attackStop = true;
			}
		}
	}
}

void Player::Resurrection()
{
	hp = hp_max;
	st = st_max;

	resurrectionTime = 3.0f;
	PLAYER_UI->SetDying(0);
	for (auto it = SCENE->Map->begin(); it != SCENE->Map->end(); it++)
	{
		Object* obj;
		if (obj = (*it)->Find("CPoint" + to_string(SCENE->checkPoint)))
		{
			actor->SetWorldPos(obj->GetWorldPos());

			Vector3 dir = obj->GetForward();
			realDir = dir;
			actor->rotation.y = atan2(-dir.x, -dir.z);
			camPivot->Find("CameraPivot")->rotation = { 0.0f,GetRealRot() ,0.0f };
			break;
		}
	}

	Reset();
	ChangeAnim(AnimationState::LOOP, PlayerAction::IDLE, 0.0f, true);
}

#undef MENU