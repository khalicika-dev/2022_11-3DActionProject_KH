#include "stdafx.h"

Monster_Giant::Monster_Giant()
{
	actor = Actor::Create();
	actor->LoadFile("Giant.xml");
	if (App.GetAppQuit())	// 1차 체크
	{
		return;
	}
	actor->rootBoneIndex = actor->Find("Root")->boneIndex;
	eventCamera = dynamic_cast<Camera*>(actor->Find("MotionCamera"));
	trail = new Object_Trail(actor->Find("TrailStart"), actor->Find("TrailEnd"));
	actor->SetStandardMatrix({
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, 1, 0, 0,
		0, 0, 0, 1 }
	);

	parts[PartsName::HEAD] = PartsStatus(InfoMsg::Subject::IronColossus_Head, 10000, 2500, actor->Find("HeadEffecter"));
	parts[PartsName::LSHOULDER] = PartsStatus(InfoMsg::Subject::IronColossus_LShoulder, 4000, 5000, actor->Find("UpperArm.L"));
	parts[PartsName::RSHOULDER] = PartsStatus(InfoMsg::Subject::IronColossus_RShoulder, 4000, 5000, actor->Find("UpperArm.R"));
	parts[PartsName::LFOOT] = PartsStatus(InfoMsg::Subject::IronColossus_LFoot, 2000, 12000, actor->Find("Foot.L"));
	parts[PartsName::RFOOT] = PartsStatus(InfoMsg::Subject::IronColossus_RFoot, 2000, 12000, actor->Find("Foot.R"));

	for (auto it = parts.begin(); it != parts.end(); it++)
	{
		BinaryReader in;
		UINT vertexSize;
		wstring filename;
		wstring path;
		switch (it->first)
		{
		case PartsName::HEAD: filename = L"Torso_Head"; break;
		case PartsName::LFOOT: filename = L"Limbs_LFoot"; break;
		case PartsName::RFOOT: filename = L"Limbs_RFoot"; break;
		case PartsName::LSHOULDER: filename = L"Torso_LShoulder"; break;
		case PartsName::RSHOULDER: filename = L"Torso_RShoulder"; break;
		}
		path = L"Contents/Mesh/Giant/" + filename + L".vData";

		it->second.vertices.clear();
		in.Open(path);
		vertexSize = in.UInt(RESOURCE->IsItInXmlList("Giant.xml"));
		for (int i = 0; i < vertexSize; i++)
		{
			it->second.vertices.emplace_back(in.Int(RESOURCE->IsItInXmlList("Giant.xml")));
			if (App.GetAppQuit()) // 버텍스 로딩 중 앱 종료 시 중단
				break;
		}
		in.Close();

		if (App.GetAppQuit())	// 2차 체크
		{
			return;
		}
	}

	for (int i = 0; i < SoundCondition::SE_ALL_COUNTS; i++)
		soundBoard.push_back(false);

	GeneratePatterns();
	Init();
	RESOURCE->DecreaseXmlList("Giant.xml");
}

Monster_Giant::~Monster_Giant()
{
	actor->Release();
	SafeDelete(trail);
}

void Monster_Giant::Calculate()
{
	Object_Trail::Calculate();
	RESOURCE->IncreaseXmlList_AddTotalCapacity("Giant.xml");
	RESOURCE->AddTotalCapacity(Util::CalculateFile("Contents/Mesh/Giant/Torso_Head.vData"));
	RESOURCE->AddTotalCapacity(Util::CalculateFile("Contents/Mesh/Giant/Limbs_LFoot.vData"));
	RESOURCE->AddTotalCapacity(Util::CalculateFile("Contents/Mesh/Giant/Limbs_RFoot.vData"));
	RESOURCE->AddTotalCapacity(Util::CalculateFile("Contents/Mesh/Giant/Torso_LShoulder.vData"));
	RESOURCE->AddTotalCapacity(Util::CalculateFile("Contents/Mesh/Giant/Torso_RShoulder.vData"));
}

void Monster_Giant::Update()
{
	if (!Enable) return;

	actor->Update();
	trail->Update();
}

void Monster_Giant::LateUpdate()
{
	if (!Enable) return;

	Vector3 pos = actor->GetWorldPos();
	Vector3 dir = -actor->GetForward();
	bool lastAir = isAir;

	// 플레이어(타겟)의 좌표
	Vector3 playerPos = GET_PLAYER->GetActor()->GetWorldPos();
	Vector3 toPlayerDir = playerPos - pos;
	toPlayerDir.Normalize();

	// 자신->타겟의 ray
	Ray toPlayerRay = Ray(pos, toPlayerDir);
	float mRot = atan2(-dir.x, -dir.z);
	float pRot = atan2(-toPlayerDir.x, -toPlayerDir.z);

	// 인식과정
	if (!beware && movable)
	{
		if (Vector3::Distance(pos, playerPos) <= 150.0f || !damageInfo.empty())
		{
			destPos = playerPos;
			ChangePattern(PatternType::PT_RECOGNIZE);
			pattern[PatternType::PT_RECOGNIZE].mustUse = true;
			PLAYER_UI->SetBossUI(this);
			beware = true;
		}
	}
	if (beware && GET_PLAYER->IsResurrection())
	{
		beware = false;
		Init();
		Spawn((*SCENE->Map)[0]->Find("BossSpawn1"));
		Appear(true);

		for (int i = 8; i <= 18; i++)
		{
			Monster* mob = GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GOBLIN, i);
			mob->Init();
			
		}
		for (int i = 2; i <= 8; i++)
		{
			Monster* mob = GAMEOBJ->Find_Mob(GOTYPE::MONSTER_DRONE, i);
			mob->Init();
		}
		return;
	}

	if (beware)
	{
		// 주기가 지나면 타겟의 위치 갱신
		if (pattern[currentPattern].playing == 0)
		{
			targetTime -= DELTA;
			if (targetTime < 0.0f)
			{
				targetTime = RANDOM->Float(0.5f, 1.0f);

				destUpdate(pos, playerPos, toPlayerRay);
				
				if (!pattern[currentPattern].mustUse)
				{
					// 사용가능한 패턴을 탐색(발견되지 않으면 최대 5번까지 재시도하고 없으면 대기모션).
					UINT searchMaxRepeat = 5;
					for (int i = 0; i < searchMaxRepeat; i++)
					{
						if (!RandomPattern())
						{
							if (i >= searchMaxRepeat - 1)
							{
								pattern[PatternType::PT_NONE].duration = RANDOM->Float(0.5f, 1.5f);
								ChangePattern(PatternType::PT_NONE);
							}
						}
						else
							break;
					}
				}
			}
		}
	}

	// 그로기 회복
	{
		if (Check_Groggy())
		{
			// 그로기상태
			if (actor->anim->PlayingIdx() == AniAction::GROGGY_ING)
			{
				float healGroggyPerSecond = groggy_max / (8.0f - (1.0f - GetHpRatio()) * 5.0f);
				groggy += healGroggyPerSecond * DELTA;
			}
		}
		else
		{
			// 통상
			const float decreaseGroggyByFoot = 50.0f;
			float healGroggyPerSecond = (100.0f
				- (decreaseGroggyByFoot * parts[PartsName::LFOOT].bDestroy)
				- (decreaseGroggyByFoot * parts[PartsName::RFOOT].bDestroy));
			if (healGroggyPerSecond < 0.0f) healGroggyPerSecond = 0.0f;
			groggy += healGroggyPerSecond * DELTA;
		}
		if (groggy > groggy_max) groggy = groggy_max;
	}

	bool isMoved = false;
	// 패턴실행
	if (movable)
	{
		AllPatternUpdate();
		// 현재 패턴이 종료 시 다음 패턴 준비
		if (pattern[currentPattern].playing == 3 && beware)
		{
			destUpdate(pos, playerPos, toPlayerRay);

			// 조건에 맞는 특수패턴 존재 시 사용
			if ((taunt_level == 0 && GetHpRatio() <= 0.95f) ||
				(taunt_level == 1 && GetHpRatio() <= 0.8f) ||
				(taunt_level == 2 && GetHpRatio() <= 0.55f) ||
				(taunt_level == 3 && GetHpRatio() <= 0.2f))
			{
				ChangePattern(PatternType::PT_TAUNT);
			}
			else
			{
				// 사용가능한 패턴을 탐색(발견되지 않으면 최대 5번까지 재시도하고 없으면 대기모션).
				UINT searchMaxRepeat = 5;
				for (int i = 0; i < searchMaxRepeat; i++)
				{
					if (!RandomPattern())
					{
						if (i >= searchMaxRepeat - 1)
						{
							// 적당한 패턴이 없으면 플레이어 추적만 실행한다
							pattern[PatternType::PT_NONE].duration = RANDOM->Float(0.5f, 1.5f);
							ChangePattern(PatternType::PT_NONE);
						}
					}
					else
						break;
				}
			}
		}

		// 현재 패턴을 준비중이거나 사용중일 때
		switch (currentPattern)
		{
		case PatternType::PT_NONE:
			if (pattern[currentPattern].playing == 2)
			{
				if (pattern[currentPattern].time <= 0.0f)
					pattern[currentPattern].playing = 3;
				else if(find(pattern[currentPattern].actionNums.begin(), pattern[currentPattern].actionNums.end(), actor->anim->PlayingIdx()) == pattern[currentPattern].actionNums.end())
					ChangeAnim(AnimationState::LOOP, pattern[currentPattern].actionNums.front());	// 실행중인 패턴과 관련된 모션이 아니라면 패턴의 첫 모션을 재생
				//else if (actor->anim->PlayingIdx() != pattern[currentPattern].actionNum)
				//	ChangeAnim(AnimationState::LOOP, pattern[currentPattern].actionNum);
			}
			else
			{
				if (beware)
				{
					isMoved = true;
					Move(pos, dir, toPlayerDir);
				}
			}
			break;
		case PatternType::PT_RECOGNIZE:
		case PatternType::PT_ATTACK1:
		case PatternType::PT_ATTACK2:
		case PatternType::PT_ATTACK3:
		case PatternType::PT_ATTACK_STOMP:
		case PatternType::PT_ATTACK_JUMP:
		case PatternType::PT_BACKSTEP:
		case PatternType::PT_TAUNT:
			if (pattern[currentPattern].playing == 2)
			{
				//if (actor->anim->PlayingIdx() == pattern[currentPattern].actionNum && !actor->anim->IsPlaying())
				if (actor->anim->PlayingIdx() == pattern[currentPattern].actionNums.back() && !actor->anim->IsPlaying())
					pattern[currentPattern].playing = 3;	// 패턴의 마지막 모션이 정지되면 패턴종료
				//else if (actor->anim->PlayingIdx() != pattern[currentPattern].actionNum)
				else if (find(pattern[currentPattern].actionNums.begin(), pattern[currentPattern].actionNums.end(), actor->anim->PlayingIdx()) == pattern[currentPattern].actionNums.end())
				{
					actor->ClearAttackTouchedList();
					Swing = 0;
					jump_lastYOffset = 0.0f;
					jump_time = 0.0f;
					jump_startPos = Vector3::Zero;
					ChangeAnim(AnimationState::ONCE, pattern[currentPattern].actionNums.front());
					actor->anim->aniScale = pattern[currentPattern].aniScale;
				}

				switch (currentPattern)
				{
				case PT_ATTACK2:
					{
						bool bRotate = false;
						Vector3 armPos;	// 무기손잡이 위치
						float rotSpeed;	// 회전속도 (deg/s)
						float angleOffset;	// 목표각도 오프셋 (degree)
						if (actor->anim->GetPlayTime() <= 20.0f / 105.0f)
						{
							// 1번 찌르기때의 회전
							armPos = actor->Find("Offset")->scale.x * -actor->GetRight() * 1.0625f;	// 오른쪽으로 1.0625f만큼 오프셋이동 (28프레임 기준)
							rotSpeed = 180.0f - (parts[PartsName::LFOOT].bDestroy ? 60.0f : 0.0f);
							angleOffset = 12.0f;
							bRotate = true;
						}
						else if (actor->anim->GetPlayTime() >= 38.0f / 105.0f && actor->anim->GetPlayTime() <= 57.0f / 105.0f)
						{
							// 2번 찌르기때의 회전
							armPos = actor->Find("Offset")->scale.x * -actor->GetRight() * 0.6875f;	// 오른쪽으로 1.0625f만큼 오프셋이동
							rotSpeed = 140.0f - (parts[PartsName::LFOOT].bDestroy ? 50.0f : 0.0f);
							angleOffset = 8.0f;
							bRotate = true;
						}

						if (Vector3::Distance(pos, playerPos) <= 20.0f)
						{
							// 적이 근접할 경우 회전을 포기
							bRotate = false;
						}
						Vector3 dirResult = playerPos - (pos + armPos);
						dirResult.Normalize();
						if (bRotate)
							Rotate(dirResult, rotSpeed * TORADIAN, angleOffset * TORADIAN);
					}
					break;
				case PT_ATTACK3:
					{
						if (actor->anim->GetPlayTime() <= 26.0f / 171.0f)
							Rotate(toPlayerDir, (140.0f - (parts[PartsName::RFOOT].bDestroy ? 40.0f : 0.0f)) * TORADIAN);
						else if (actor->anim->GetPlayTime() >= 57.0f / 171.0f && actor->anim->GetPlayTime() <= 74.0f / 171.0f)
							Rotate(toPlayerDir, (160.0f - (parts[PartsName::LFOOT].bDestroy ? 40.0f : 0.0f)) * TORADIAN);
						else if (actor->anim->GetPlayTime() >= 82.0f / 171.0f && actor->anim->GetPlayTime() <= 100.0f / 171.0f)
							Rotate(toPlayerDir, (150.0f - (parts[PartsName::RFOOT].bDestroy ? 40.0f : 0.0f)) * TORADIAN);
						//if (actor->anim->GetPlayTime() <= 100.0f / 171.0f)
						//	Rotate(toPlayerDir, 90.0f * TORADIAN);
						
					}
					break;
				case PT_ATTACK_JUMP:
					{
						float jumpOffset = 2.75f * actor->Find("Offset")->scale.y;
						if (actor->anim->PlayingIdx() == AniAction::ATTACK_JUMP_BEGIN && actor->anim->GetPlayTime() <= 19.0f / 44.0f)
						{
							destPos = playerPos;
							Rotate(toPlayerDir, (190.0f - (parts[PartsName::LFOOT].bDestroy ? 25.0f : 0) - (parts[PartsName::RFOOT].bDestroy ? 25.0f : 0)) * TORADIAN);
						}

						// BEGIN_27프레임부터 END_3프레임까지 20프레임동안 지정된 위치로 도약 (블렌더 30프레임은 1초에 해당, 20프레임은 0.66초(40/60)에 해당하지만 실제 테스트시간은 0.6166초(37/60))
						// 도약 전 모션 자체의 이동거리에 의해 1.0f * scale.y 만큼의 지면거리를 이동한다. (Giant의 scale.y는 정면으로의 사이즈)
						// 착지직후 본체와 창의 도약찍기 충격파간의 거리는 2.75f * scale.y 만큼의 거리를 가진다. (scale=8 기준 22.0f)
						// aniscale값이 너무 높을수록(모션속도가 너무 빠를수록) 오차가 발생하나 해결방도를 찾지 못함. 어차피 정상적인 방법으로 발생하지 않음.
						if ((actor->anim->PlayingIdx() == AniAction::ATTACK_JUMP_BEGIN && actor->anim->GetPlayTime() > 27.0f / 44.0f) ||
							(actor->anim->PlayingIdx() == AniAction::ATTACK_JUMP_END && actor->anim->GetPlayTime() < 3.0f / 55.0f))
						{
							Vector3 jumpEndPos;
							if (jump_time <= 0.0f)
								jump_startPos = pos;
							jumpEndPos = destPos;
							jump_startPos.y = jumpEndPos.y = pos.y;
							if(Vector3::Distance(jump_startPos, jumpEndPos) > pattern[PT_ATTACK_JUMP].distance)
								jumpEndPos = jump_startPos + dir * (pattern[PT_ATTACK_JUMP].distance + 10.0f - jumpOffset);	// 도약거리 최대값
							else if (Vector3::Distance(jump_startPos, jumpEndPos) > jumpOffset)
								jumpEndPos = jump_startPos + dir * (Vector3::Distance(jump_startPos, jumpEndPos) - jumpOffset);
							else
								jumpEndPos = jump_startPos;	//도약거리 최소값


							// aniscale=1.0f, fixFrame60 기준 약 20/60초의 시간만큼 도약해야한다.
							jump_time += DELTA * actor->anim->aniScale;
							pos = Util::Lerp(jump_startPos, jumpEndPos, jump_time * App.fixFrame / 37.0f);
							//cout << "t=" << jump_time << ", lerp level : " << jump_time * App.fixFrame / 37.0f << endl;
						}
					}
					break;
				}
			}
			else
			{
				if (beware)
				{
					isMoved = true;
					Move(pos, dir, toPlayerDir);
				}
			}
			break;
		}
	}

	// 모션으로 인한 공격판정 생성
	{
		Object* ob;
		float decDam_Shoulders = (parts[PartsName::LSHOULDER].bDestroy ? 0.4f : 0.0f) + (parts[PartsName::RSHOULDER].bDestroy ? 0.6f : 0.0f);
		int decDam_LF = parts[PartsName::LFOOT].bDestroy ? 1 : 0;
		int decDam_RF = parts[PartsName::RFOOT].bDestroy ? 1 : 0;

		switch (actor->anim->PlayingIdx())
		{
		case AniAction::ATTACK1:
			{
				if (actor->anim->GetPlayTime() >= 25.0f / 79.0f && actor->anim->GetPlayTime() <= 44.0f / 79.0f)
				{
					trail->AddTrail();
					ob = GetActor()->Find("AttackCollider1_1");
					ob->collider->atk.damage = 25 - (decDam_Shoulders * 6.0f);
					ob->collider->atk.kb_velocity = { 20.0f, GetJump(3.0f,0.5f) };
					ob->collider->atk.hitSound = SDKEY::hit_pole01;
					ob->collider->atk.guardSound = (SDKEY)((int)SDKEY::guard02 + RANDOM->Int(0, 2));
					AttackPlayerAndMonster(ob->collider);
					ob = GetActor()->Find("AttackCollider1_2");
					ob->collider->atk.damage = 12 - (decDam_Shoulders * 4.0f);
					ob->collider->atk.kb_velocity = { 15.0f, GetJump(3.0f,0.5f) };
					ob->collider->atk.hitSound = SDKEY::hit_punch03;
					ob->collider->atk.guardSound = SDKEY::guard01;
					AttackPlayerAndMonster(ob->collider);
				}
				if (actor->anim->GetPlayTime() >= 30.0f / 79.0f &&
					actor->anim->GetPlayTime() <= 45.0f / 79.0f)
				{
					ob = GetActor()->Find("AttackCollider2_1");
					ob->collider->atk.damage = 7;
					ob->collider->atk.kb_velocity = { 15.0f, GetJump(5.0f,0.5f) };
					ob->collider->atk.hitSound = SDKEY::hit_punch03;
					ob->collider->atk.guardSound = (SDKEY)((int)SDKEY::guard02 + RANDOM->Int(0, 2));
					AttackPlayerAndMonster(ob->collider);
				}
				if (actor->anim->GetPlayTime() >= 59.0f / 79.0f &&
					actor->anim->GetPlayTime() <= 67.0f / 79.0f)
				{
					ob = GetActor()->Find("AttackCollider2_2");
					ob->collider->atk.damage = 5 - (decDam_RF * 1);
					ob->collider->atk.kb_velocity = Vector2::Zero;
					ob->collider->atk.hitSound = SDKEY::hit_punch02;
					ob->collider->atk.guardSound = SDKEY::guard01;
					AttackPlayerAndMonster(ob->collider);
				}
				if (actor->anim->GetPlayTime() >= 28.0f / 79.0f &&
					actor->anim->GetPlayTime() <= 38.0f / 79.0f)
				{
					ob = GetActor()->Find("AttackCollider2_3");
					ob->collider->atk.damage = 18 - (decDam_LF * 2);
					ob->collider->atk.kb_velocity = { 15.0f, GetJump(5.0f,0.5f) };
					ob->collider->atk.hitSound = SDKEY::hit_punch03;
					ob->collider->atk.guardSound = (SDKEY)((int)SDKEY::guard02 + RANDOM->Int(0, 2));
					AttackPlayerAndMonster(ob->collider);
				}
			}
			break;
		case AniAction::ATTACK2:
			{
				if (Swing == 0 && actor->anim->GetPlayTime() >= 55.0f / 105.0f)
				{
					// 두번째 스윙때 재공격 가능
					Swing = 1;
					GetActor()->ClearAttackTouchedList();
				}

				if ((actor->anim->GetPlayTime() >= 20.0f / 105.0f && actor->anim->GetPlayTime() <= 38.0f / 105.0f) ||
					(actor->anim->GetPlayTime() >= 59.0f / 105.0f && actor->anim->GetPlayTime() <= 65.0f / 105.0f))
				{
					trail->AddTrail();
					ob = GetActor()->Find("AttackCollider1_1");
					switch (Swing)
					{
					case 0:
						ob->collider->atk.damage = 22 - (decDam_Shoulders * 4.0f);
						ob->collider->atk.kb_velocity = { 25.0f, GetJump(3.0f,0.5f) };
						break;
					case 1:
						ob->collider->atk.damage = 28 - (decDam_Shoulders * 8.0f);
						ob->collider->atk.kb_velocity = { 30.0f, GetJump(3.0f,0.5f) };
						break;
					}
					ob->collider->atk.hitSound = SDKEY::hit_pole01;
					ob->collider->atk.guardSound = (SDKEY)((int)SDKEY::guard02 + RANDOM->Int(0, 2));
					AttackPlayerAndMonster(ob->collider);
				}
				if ((actor->anim->GetPlayTime() >= 22.0f / 105.0f && actor->anim->GetPlayTime() <= 31.0f / 105.0f) ||
					(actor->anim->GetPlayTime() >= 59.0f / 105.0f && actor->anim->GetPlayTime() <= 63.0f / 105.0f))
				{
					ob = GetActor()->Find("AttackCollider2_1");
					switch (Swing)
					{
					case 0:
						ob->collider->atk.damage = 6;
						ob->collider->atk.kb_velocity = { 12.0f, GetJump(3.0f,0.5f) };
						break;
					case 1:
						ob->collider->atk.damage = 9;
						ob->collider->atk.kb_velocity = { 18.0f, GetJump(3.5f,0.5f) };
						break;
					}
					ob->collider->atk.hitSound = SDKEY::hit_punch03;
					ob->collider->atk.guardSound = (SDKEY)((int)SDKEY::guard02 + RANDOM->Int(0, 2));
					AttackPlayerAndMonster(ob->collider);
				}
				if ((actor->anim->GetPlayTime() >= 22.0f / 105.0f && actor->anim->GetPlayTime() <= 31.0f / 105.0f) ||
					(actor->anim->GetPlayTime() >= 58.0f / 105.0f && actor->anim->GetPlayTime() <= 63.0f / 105.0f))
				{
					ob = GetActor()->Find("AttackCollider2_2");
					switch (Swing)
					{
					case 0:
						ob->collider->atk.damage = 16 - (decDam_RF * 3);
						ob->collider->atk.kb_velocity = { 15.0f, GetJump(4.0f,0.5f) };
						break;
					case 1:
						ob->collider->atk.damage = 13 - (decDam_RF * 2);
						ob->collider->atk.kb_velocity = { 10.0f, GetJump(3.0f,0.5f) };
						break;
					}
					ob->collider->atk.hitSound = SDKEY::hit_punch03;
					ob->collider->atk.guardSound = (SDKEY)((int)SDKEY::guard02 + RANDOM->Int(0, 2));
					AttackPlayerAndMonster(ob->collider);
				}
				if (actor->anim->GetPlayTime() >= 85.0f / 105.0f && actor->anim->GetPlayTime() <= 94.0f / 105.0f)
				{
					ob = GetActor()->Find("AttackCollider2_3");
					ob->collider->atk.damage = 6 - (decDam_LF * 2);
					ob->collider->atk.kb_velocity = Vector2::Zero;
					ob->collider->atk.hitSound = SDKEY::hit_punch02;
					ob->collider->atk.guardSound = SDKEY::guard01;
					AttackPlayerAndMonster(ob->collider);
				}
			}
			break;
		case AniAction::ATTACK3:
			{
				if (Swing == 0 && actor->anim->GetPlayTime() >= 57.0f / 171.0f)
				{
					// 두번째 스윙때 재공격 가능
					Swing = 1;
					GetActor()->ClearAttackTouchedList();
				}
				else if (Swing == 1 && actor->anim->GetPlayTime() >= 82.0f / 171.0f)
				{
					// 세번째 스윙때 재공격 가능
					Swing = 2;
					GetActor()->ClearAttackTouchedList();
				}

				if ((actor->anim->GetPlayTime() >= 35.0f / 171.0f && actor->anim->GetPlayTime() <= 49.0f / 171.0f) ||
					(actor->anim->GetPlayTime() >= 69.0f / 171.0f && actor->anim->GetPlayTime() <= 80.0f / 171.0f) ||
					(actor->anim->GetPlayTime() >= 97.0f / 171.0f && actor->anim->GetPlayTime() <= 106.0f / 171.0f))
				{
					trail->AddTrail();
					ob = GetActor()->Find("AttackCollider1_1");
					switch (Swing)
					{
					case 0:
						ob->collider->atk.damage = 20 - (decDam_Shoulders * 4.0f);
						ob->collider->atk.kb_velocity = { 15.0f, GetJump(8.0f,0.5f) };
						break;
					case 1:
						ob->collider->atk.damage = 23 - (decDam_Shoulders * 5.0f);
						ob->collider->atk.kb_velocity = { 20.0f, GetJump(3.0f,0.5f) };
						break;
					case 2:
						ob->collider->atk.damage = 26 - (decDam_Shoulders * 6.0f);
						ob->collider->atk.kb_velocity = { 15.0f, GetJump(3.0f,0.5f) };
						break;
					}
					ob->collider->atk.hitSound = SDKEY::hit_pole01;
					ob->collider->atk.guardSound = (SDKEY)((int)SDKEY::guard02 + RANDOM->Int(0, 2));
					AttackPlayerAndMonster(ob->collider);
					ob = GetActor()->Find("AttackCollider1_2");
					switch (Swing)
					{
					case 0:
						ob->collider->atk.damage = 14 - (decDam_Shoulders * 2.0f);
						ob->collider->atk.kb_velocity = { 12.0f, GetJump(5.0f,0.5f) };
						break;
					case 1:
						ob->collider->atk.damage = 15 - (decDam_Shoulders * 2.0f);
						ob->collider->atk.kb_velocity = { 15.0f, GetJump(3.0f,0.5f) };
						break;
					case 2:
						ob->collider->atk.damage = 17 - (decDam_Shoulders * 2.0f);
						ob->collider->atk.kb_velocity = { 15.0f, GetJump(3.0f,0.5f) };
						break;
					}
					ob->collider->atk.hitSound = SDKEY::hit_punch03;
					ob->collider->atk.guardSound = SDKEY::guard01;
					AttackPlayerAndMonster(ob->collider);
				}
				if ((actor->anim->GetPlayTime() >= 40.0f / 171.0f && actor->anim->GetPlayTime() <= 55.0f / 171.0f) ||
					(actor->anim->GetPlayTime() >= 74.0f / 171.0f && actor->anim->GetPlayTime() <= 80.0f / 171.0f) ||
					(actor->anim->GetPlayTime() >= 99.0f / 171.0f && actor->anim->GetPlayTime() <= 106.0f / 171.0f))
				{
					ob = GetActor()->Find("AttackCollider2_1");
					switch (Swing)
					{
					case 0:
						ob->collider->atk.damage = 6;
						ob->collider->atk.kb_velocity = { 12.0f, GetJump(3.0f,0.5f) };
						break;
					case 1:
						ob->collider->atk.damage = 8;
						ob->collider->atk.kb_velocity = { 15.0f, GetJump(3.0f,0.5f) };
						break;
					case 2:
						ob->collider->atk.damage = 10;
						ob->collider->atk.kb_velocity = { 18.0f, GetJump(3.0f,0.5f) };
						break;
					}
					ob->collider->atk.hitSound = SDKEY::hit_punch03;
					ob->collider->atk.guardSound = (SDKEY)((int)SDKEY::guard02 + RANDOM->Int(0, 2));
					AttackPlayerAndMonster(ob->collider);
				}
				if ((actor->anim->GetPlayTime() >= 63.0f / 171.0f && actor->anim->GetPlayTime() <= 75.0f / 171.0f) ||
					(actor->anim->GetPlayTime() >= 132.0f / 171.0f && actor->anim->GetPlayTime() <= 150.0f / 171.0f))
				{
					ob = GetActor()->Find("AttackCollider2_2");
					switch (Swing)
					{
					case 1:
						ob->collider->atk.damage = 17 - (decDam_RF * 4);
						ob->collider->atk.kb_velocity = { 14.0f, GetJump(5.0f,0.5f) };
						ob->collider->atk.hitSound = SDKEY::hit_punch03;
						ob->collider->atk.guardSound = (SDKEY)((int)SDKEY::guard02 + RANDOM->Int(0, 2));
						break;
					case 2:
						ob->collider->atk.damage = 5 - (decDam_RF * 2);
						ob->collider->atk.kb_velocity = Vector2::Zero;
						ob->collider->atk.hitSound = SDKEY::hit_punch02;
						ob->collider->atk.guardSound = SDKEY::guard01;
						break;
					}
					AttackPlayerAndMonster(ob->collider);
				}
				if ((actor->anim->GetPlayTime() >= 39.0f / 171.0f && actor->anim->GetPlayTime() <= 49.0f / 171.0f) ||
					(actor->anim->GetPlayTime() >= 59.0f / 171.0f && actor->anim->GetPlayTime() <= 80.0f / 171.0f) ||
					(actor->anim->GetPlayTime() >= 85.0f / 171.0f && actor->anim->GetPlayTime() <= 106.0f / 171.0f))
				{
					ob = GetActor()->Find("AttackCollider2_3");
					switch (Swing)
					{
					case 0:
						ob->collider->atk.damage = 17 - (decDam_LF * 4);
						ob->collider->atk.kb_velocity = { 14.0f, GetJump(5.0f,0.5f) };
						ob->collider->atk.hitSound = SDKEY::hit_punch03;
						ob->collider->atk.guardSound = (SDKEY)((int)SDKEY::guard02 + RANDOM->Int(0, 2));
						break;
					case 1:
						ob->collider->atk.damage = 3 - (decDam_LF * 2);
						ob->collider->atk.kb_velocity = Vector2::Zero;
						ob->collider->atk.hitSound = SDKEY::hit_punch00;
						ob->collider->atk.guardSound = SDKEY::guard01;
						break;
					case 2:
						ob->collider->atk.damage = 15 - (decDam_LF * 4);
						ob->collider->atk.kb_velocity = { 12.0f, GetJump(3.0f,0.5f) };
						ob->collider->atk.hitSound = SDKEY::hit_punch03;
						ob->collider->atk.guardSound = (SDKEY)((int)SDKEY::guard02 + RANDOM->Int(0, 2));
						break;
					}
					AttackPlayerAndMonster(ob->collider);
				}
			}
			break;
		case AniAction::ATTACK_STOMP:
			{
				if (Swing <= 1 && actor->anim->GetPlayTime() >= 40.0f / 77.0f)
				{
					// 1~2번째 스윙때 재공격 가능
					Swing = 2;
					GetActor()->ClearAttackTouchedList();
				}
				if (actor->anim->GetPlayTime() > 27.0f / 77.0f && actor->anim->GetPlayTime() <= 30.0f / 77.0f)
				{
					ob = GetActor()->Find("AttackCollider2_1");
					ob->collider->atk.damage = 12;
					ob->collider->atk.kb_velocity = { 10.0f, GetJump(3.0f,0.5f) };
					ob->collider->atk.hitSound = SDKEY::hit_punch03;
					ob->collider->atk.guardSound = (SDKEY)((int)SDKEY::guard02 + RANDOM->Int(0, 2));
					AttackPlayerAndMonster(ob->collider);
				}
				if ((actor->anim->GetPlayTime() > 27.0f / 77.0f && actor->anim->GetPlayTime() <= 30.0f / 77.0f) ||
					(actor->anim->GetPlayTime() > 46.0f / 77.0f && actor->anim->GetPlayTime() <= 58.0f / 77.0f))
				{
					ob = GetActor()->Find("AttackCollider2_3");
					switch (Swing)
					{
					case 0:
						ob->collider->atk.damage = 30 - (decDam_LF * 8);
						ob->collider->atk.kb_velocity = { 15.0f, GetJump(5.0f,0.5f) };
						ob->collider->atk.hitSound = SDKEY::hit_punch03;
						ob->collider->atk.guardSound = SDKEY::guard05;
						break;
					case 2:
						ob->collider->atk.damage = 6 - (decDam_LF * 2);
						ob->collider->atk.kb_velocity = Vector2::Zero;
						ob->collider->atk.hitSound = SDKEY::hit_punch02;
						ob->collider->atk.guardSound = SDKEY::guard01;
						break;
					}
					AttackPlayerAndMonster(ob->collider);
				}
				if (actor->anim->GetPlayTime() > 30.0f / 77.0f && actor->anim->GetPlayTime() <= 35.0f / 77.0f)
				{
					ob = GetActor()->Find("AttackCollider2_4");
					ob->collider->atk.damage = 25 - (decDam_LF * 7);
					ob->collider->atk.kb_velocity = { 20.0f, GetJump(5.0f,0.5f) };
					ob->collider->atk.hitSound = SDKEY::NONE;
					ob->collider->atk.guardSound = SDKEY::guard06;
					AttackPlayerAndMonster(ob->collider);

					if (Swing == 0)
					{
						// 발구르기가 사용된 직후 지면에 Pop이펙트 생성
						Swing = 1;
						{
							Vector3 spawnPos = actor->Find("AttackCollider2_4")->collider->GetWorldPos();
							Pop* temp = dynamic_cast<Pop*>(GAMEOBJ->Generate(PTCTYPE::Effect_Hit, spawnPos)->GetActor());
							temp->scale = { 3.0f,3.0f,3.0f };
							temp->material->diffuse = Color(0.455f, 0.356f, 0.266f, 1.0f);
							temp->duration = 0.5f;
							temp->desc.gravity = 20.0f;
							temp->particleCount = 100;
							temp->velocityScalar = 50.0f;
							temp->Play();
						}
					}
				}
			}
			break;
		case AniAction::ATTACK_JUMP_BEGIN:
			{
				if (Swing == 0 && actor->anim->GetPlayTime() >= 25.0f / 44.0f)
				{
					// 두번째 스윙때 재공격 가능
					Swing = 1;
					GetActor()->ClearAttackTouchedList();
				}
				else if (Swing == 1 && actor->anim->GetPlayTime() >= 38.0f / 44.0f)
				{
					// 세번째 스윙때 재공격 가능
					Swing = 2;
					GetActor()->ClearAttackTouchedList();
				}

				if (actor->anim->GetPlayTime() >= 28.0f / 44.0f)
				{
					trail->AddTrail();
					ob = GetActor()->Find("AttackCollider1_1");
					ob->collider->atk.damage = 28 - (decDam_Shoulders * 8.0f);
					ob->collider->atk.kb_velocity = { 15.0f, GetJump(5.0f,0.5f) };
					ob->collider->atk.hitSound = SDKEY::hit_pole01;
					ob->collider->atk.guardSound = (SDKEY)((int)SDKEY::guard02 + RANDOM->Int(0, 2));
					AttackPlayerAndMonster(ob->collider);
					ob = GetActor()->Find("AttackCollider1_2");
					ob->collider->atk.damage = 22 - (decDam_Shoulders * 6.0f);
					ob->collider->atk.kb_velocity = { 15.0f, GetJump(5.0f,0.5f) };
					ob->collider->atk.hitSound = SDKEY::hit_punch03;
					ob->collider->atk.guardSound = SDKEY::guard01;
					AttackPlayerAndMonster(ob->collider);
				}
				if (actor->anim->GetPlayTime() >= 26.0f / 44.0f)
				{
					ob = GetActor()->Find("AttackCollider2_1");
					ob->collider->atk.damage = 20;
					ob->collider->atk.kb_velocity = { 18.0f, GetJump(5.0f,0.5f) };
					ob->collider->atk.hitSound = SDKEY::hit_punch03;
					ob->collider->atk.guardSound = (SDKEY)((int)SDKEY::guard02 + RANDOM->Int(0, 2));
					AttackPlayerAndMonster(ob->collider);
				}
				if (actor->anim->GetPlayTime() >= 27.0f / 44.0f)
				{
					ob = GetActor()->Find("AttackCollider2_2");
					ob->collider->atk.damage = 20 - (decDam_RF * 3);
					ob->collider->atk.kb_velocity = { 18.0f, GetJump(5.0f,0.5f) };
					ob->collider->atk.hitSound = SDKEY::hit_punch03;
					ob->collider->atk.guardSound = (SDKEY)((int)SDKEY::guard02 + RANDOM->Int(0, 2));
					AttackPlayerAndMonster(ob->collider);
				}
				if ((actor->anim->GetPlayTime() >= 13.0f / 44.0f && actor->anim->GetPlayTime() <= 19.0f / 44.0f) ||
					(actor->anim->GetPlayTime() >= 28.0f / 44.0f))
				{
					ob = GetActor()->Find("AttackCollider2_3");
					switch (Swing)
					{
					case 0:
						ob->collider->atk.damage = 6 - (decDam_LF * 2);
						ob->collider->atk.kb_velocity = Vector2::Zero;
						ob->collider->atk.hitSound = SDKEY::hit_punch02;
						ob->collider->atk.guardSound = SDKEY::guard01;
						break;
					case 1:
						ob->collider->atk.damage = 20 - (decDam_LF * 3);
						ob->collider->atk.kb_velocity = { 18.0f, GetJump(5.0f,0.5f) };
						ob->collider->atk.hitSound = SDKEY::hit_punch03;
						ob->collider->atk.guardSound = (SDKEY)((int)SDKEY::guard02 + RANDOM->Int(0, 2));
						break;
					}
					AttackPlayerAndMonster(ob->collider);
				}
			}
			break;
		case AniAction::ATTACK_JUMP_END:
			{
				// BEGIN모션의 Swing 최종값=2에서 이어짐
				int dmg_general = 40;
				Vector2 kb_general = { 18.0f, GetJump(5.0f,0.5f) };
				SDKEY hitSD_general = SDKEY::hit_punch03;
				SDKEY guardSD_general = SDKEY::guard05;

				if (Swing == 3 && actor->anim->GetPlayTime() >= 15.0f / 55.0f)
				{
					// 다섯번째 스윙때 재공격 가능
					Swing = 4;
					GetActor()->ClearAttackTouchedList();
				}

				if (actor->anim->GetPlayTime() <= 3.0f / 55.0f)
				{
					trail->AddTrail();
					ob = GetActor()->Find("AttackCollider1_1");
					ob->collider->atk.damage = 57 - (decDam_Shoulders * 12.0f);
					ob->collider->atk.kb_velocity = { 20.0f, GetJump(5.0f,0.5f) };
					ob->collider->atk.hitSound = SDKEY::hit_pole01;
					ob->collider->atk.guardSound = SDKEY::guard05;
					AttackPlayerAndMonster(ob->collider);

					

					ob = GetActor()->Find("AttackCollider1_2");
					ob->collider->atk.damage = dmg_general - (decDam_Shoulders * 3.0f);
					ob->collider->atk.kb_velocity = kb_general;
					ob->collider->atk.hitSound = hitSD_general;
					ob->collider->atk.guardSound = guardSD_general;
					AttackPlayerAndMonster(ob->collider);

					ob = GetActor()->Find("AttackCollider2_1");
					ob->collider->atk.damage = dmg_general;
					ob->collider->atk.kb_velocity = kb_general;
					ob->collider->atk.hitSound = hitSD_general;
					ob->collider->atk.guardSound = guardSD_general;
					AttackPlayerAndMonster(ob->collider);

					ob = GetActor()->Find("AttackCollider2_2");
					ob->collider->atk.damage = dmg_general - (decDam_RF * 5);
					ob->collider->atk.kb_velocity = kb_general;
					ob->collider->atk.hitSound = hitSD_general;
					ob->collider->atk.guardSound = guardSD_general;
					AttackPlayerAndMonster(ob->collider);

					ob = GetActor()->Find("AttackCollider2_3");
					ob->collider->atk.damage = dmg_general - (decDam_LF * 5);
					ob->collider->atk.kb_velocity = kb_general;
					ob->collider->atk.hitSound = hitSD_general;
					ob->collider->atk.guardSound = guardSD_general;
					AttackPlayerAndMonster(ob->collider);
				}
				else if (actor->anim->GetPlayTime() <= 10.0f / 55.0f)
				{
					ob = GetActor()->Find("AttackCollider1_3");
					ob->collider->atk.damage = 37 - (decDam_Shoulders * 8.0f);
					ob->collider->atk.kb_velocity = { 28.0f, GetJump(7.0f,0.5f) };
					ob->collider->atk.hitSound = SDKEY::NONE;
					ob->collider->atk.guardSound = SDKEY::guard06;
					AttackPlayerAndMonster(ob->collider);

					ob = GetActor()->Find("AttackCollider2_5");
					ob->collider->atk.damage = 31 - (decDam_LF * 3) - (decDam_RF * 3);
					ob->collider->atk.kb_velocity = { 23.0f, GetJump(5.5f,0.5f) };
					ob->collider->atk.hitSound = SDKEY::NONE;
					ob->collider->atk.guardSound = SDKEY::guard06;
					AttackPlayerAndMonster(ob->collider);

					if (Swing <= 2)
					{
						// 착지 직후 지면에 Pop이펙트 생성 (네번째 스윙으로 변경)
						Swing = 3;
						{
							Vector3 spawnPos = actor->Find("AttackCollider1_3")->collider->GetWorldPos();
							Pop* temp = dynamic_cast<Pop*>(GAMEOBJ->Generate(PTCTYPE::Effect_Hit, spawnPos)->GetActor());
							temp->scale = { 2.0f,2.0f,2.0f };
							temp->material->diffuse = Color(0.455f, 0.356f, 0.266f, 1.0f);
							temp->duration = 1.0f;
							temp->desc.gravity = 30.0f;
							temp->particleCount = 150;
							temp->velocityScalar = 40.0f;
							temp->Play();
						}
						{
							Vector3 spawnPos = actor->Find("AttackCollider2_5")->collider->GetWorldPos();
							Pop* temp = dynamic_cast<Pop*>(GAMEOBJ->Generate(PTCTYPE::Effect_Hit, spawnPos)->GetActor());
							temp->scale = { 3.0f,3.0f,3.0f };
							temp->material->diffuse = Color(0.455f, 0.356f, 0.266f, 1.0f);
							temp->duration = 3.0f;
							temp->desc.gravity = 50.0f;
							temp->particleCount = 200;
							temp->velocityScalar = 60.0f;
							temp->Play();
						}
					}
				}

				if (actor->anim->GetPlayTime() >= 41.0f / 55.0f && actor->anim->GetPlayTime() <= 50.0f / 55.0f)
				{
					ob = GetActor()->Find("AttackCollider2_2");
					ob->collider->atk.damage = 3 - (decDam_RF * 2.0);
					ob->collider->atk.kb_velocity = Vector2::Zero;
					ob->collider->atk.hitSound = SDKEY::hit_punch02;
					ob->collider->atk.guardSound = SDKEY::guard01;
					AttackPlayerAndMonster(ob->collider);
				}

				if (actor->anim->GetPlayTime() >= 21.0f / 55.0f && actor->anim->GetPlayTime() <= 37.0f / 55.0f)
				{
					ob = GetActor()->Find("AttackCollider2_3");
					ob->collider->atk.damage = 5 - (decDam_LF * 2.0);
					ob->collider->atk.kb_velocity = Vector2::Zero;
					ob->collider->atk.hitSound = SDKEY::hit_punch02;
					ob->collider->atk.guardSound = SDKEY::guard01;
					AttackPlayerAndMonster(ob->collider);
				}
			}
			break;
		case AniAction::TAUNT:
			{
				if (Swing == 0 && actor->anim->GetPlayTime() >= 29.0f / 72.0f)
				{
					Swing = 1;

					// 고블린은 8~18번 객체까지 소환
					// 드론은 2~8번 객체까지 소환
					switch (taunt_level)
					{
					case 0:
						// 1페이즈: 고블린 3마리
						for (int i = 0; i < 3; i++)
						{
							int num = i + 8;
							SummonMonster(static_cast<int>(GOTYPE::MONSTER_GOBLIN), num);
							GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GOBLIN, num)->ResetHP(1000);
							GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GOBLIN, num)->ResetST(250.0f);
						}
						break;
					case 1:
						// 2페이즈: 고블린 5마리
						for (int i = 0; i < 5; i++)
						{
							int num = i + 11;
							SummonMonster(static_cast<int>(GOTYPE::MONSTER_GOBLIN), num);
							GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GOBLIN, num)->ResetHP(1000);
							GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GOBLIN, num)->ResetST(250.0f);
						}
						break;
					case 2:
						// 3페이즈: 드론 4마리
						for (int i = 0; i < 4; i++)
						{
							int num = i + 2;
							SummonMonster(static_cast<int>(GOTYPE::MONSTER_DRONE), num);
							GAMEOBJ->Find_Mob(GOTYPE::MONSTER_DRONE, num)->ResetHP(1000);
						}
						break;
					case 3:
						// 4페이즈: 고블린 3마리 + 드론 3마리
						for (int i = 0; i < 3; i++)
						{
							int num = i + 16;
							SummonMonster(static_cast<int>(GOTYPE::MONSTER_GOBLIN), num);
							GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GOBLIN, num)->ResetHP(1000);
							GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GOBLIN, num)->ResetST(250.0f);

							num = i + 6;
							SummonMonster(static_cast<int>(GOTYPE::MONSTER_DRONE), num);
							GAMEOBJ->Find_Mob(GOTYPE::MONSTER_DRONE, num)->ResetHP(1000);
						}
						break;
					}
					taunt_level++;
				}
			}
			break;
		}
	}

	// 생존하는 동안 모터사운드 재생
	{
	const float maxVol = 0.5f;
	if (hp > 0.0f && actor->visible)
	{
		if (!motorChannel)
			motorChannel = SOUND->Play(SDKEY::motor00, SDTYPE::SE, 0.0f, actor, 15.0f);
		else if (motorChannel->volume < maxVol)
			motorChannel->volume += (motorChannel->volume + maxVol * (DELTA_NS / 0.5f));	// 0.5초 동안 볼륨이 최대값으로 변화
		else
			motorChannel->volume = maxVol;	// 최대값
	}
	else if (motorChannel)
	{
		motorChannel->volume -= (motorChannel->volume + maxVol * (DELTA_NS / 0.5f));
		if (motorChannel->volume <= 0.0f)
		{
			motorChannel->volume = 0.0f;
			motorChannel->channel->stop();
			motorChannel = nullptr;
		}
	}
	}

	// 모션으로 인한 사운드 생성
	switch (actor->anim->PlayingIdx())
	{
	case AniAction::RECOGNIZE:
		SOUND->Play(SDKEY::robot01, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 25.0f / 100.0f)
		{
			SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, GetPos_FootR());
			SOUND->Play(SDKEY::robot00, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 1.0f, actor, 100.0f);
		}
		if (actor->anim->GetPlayTime() >= 92.0f / 100.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE3, SDTYPE::SE, 0.25f, GetPos_FootR(), 5.0f);
		break;
	case AniAction::WALK:
		if (actor->anim->GetPlayTime() >= 18.0f / 57.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.5f, GetPos_FootL());
		if (actor->anim->GetPlayTime() >= 49.0f / 57.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.5f, GetPos_FootR());

		if (actor->anim->IsLooped())
		{
			soundBoard[SoundCondition::SE_SPACE0] = false;
			soundBoard[SoundCondition::SE_SPACE1] = false;
		}
		break;
	case AniAction::LTURN:
		if (actor->anim->GetPlayTime() >= 10.0f / 30.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.5f, GetPos_FootL());
		if (actor->anim->GetPlayTime() >= 24.0f / 30.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.5f, GetPos_FootR());
		break;
	case AniAction::RTURN:
		if (actor->anim->GetPlayTime() >= 10.0f / 30.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.5f, GetPos_FootR());
		if (actor->anim->GetPlayTime() >= 24.0f / 30.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.5f, GetPos_FootL());
		break;
	case AniAction::ATTACK1:
		SOUND->Play(SDKEY::robot02, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 12.0f / 79.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, GetPos_FootL());
		if (actor->anim->GetPlayTime() >= 25.0f / 79.0f) SOUND->Play(SDKEY::attack_pole00, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 1.0f, GetObj_WeaponBlade(), 30.0f);
		if (actor->anim->GetPlayTime() >= 38.0f / 79.0f) SOUND->Play(SDKEY::footstep_c01, soundBoard, SoundCondition::SE_SPACE3, SDTYPE::SE, 1.0f, GetPos_FootL());
		if (actor->anim->GetPlayTime() >= 68.0f / 79.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE4, SDTYPE::SE, 0.5f, GetPos_FootR());
		if (actor->anim->GetPlayTime() >= 78.0f / 79.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE5, SDTYPE::SE, 0.1f, GetPos_FootL(), 5.0f);
		break;
	case AniAction::ATTACK2:
		SOUND->Play(SDKEY::robot03, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 35.0f / 105.0f) SOUND->Play(SDKEY::robot04, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);

		if (actor->anim->GetPlayTime() >= 24.0f / 105.0f) SOUND->Play(SDKEY::attack_pole01, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 0.5f, GetObj_WeaponBlade(), 20.0f);
		if (actor->anim->GetPlayTime() >= 59.0f / 105.0f) SOUND->Play(SDKEY::attack_pole02, soundBoard, SoundCondition::SE_SPACE3, SDTYPE::SE, 0.5f, GetObj_WeaponBlade(), 20.0f);

		if (actor->anim->GetPlayTime() >= 30.0f / 105.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE4, SDTYPE::SE, 0.5f, GetPos_FootR());
		if (actor->anim->GetPlayTime() >= 63.0f / 105.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE5, SDTYPE::SE, 0.6f, GetPos_FootR());
		if (actor->anim->GetPlayTime() >= 96.0f / 105.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE6, SDTYPE::SE, 0.5f, GetPos_FootL());
		break;
	case AniAction::ATTACK3:
		SOUND->Play(SDKEY::drone01, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);

		SOUND->Play(SDKEY::robot02, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 55.0f / 171.0f) SOUND->Play(SDKEY::robot03, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 81.0f / 171.0f) SOUND->Play(SDKEY::robot04, soundBoard, SoundCondition::SE_SPACE3, SDTYPE::SE, 1.0f, actor);

		if (actor->anim->GetPlayTime() >= 25.0f / 171.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE4, SDTYPE::SE, 0.05f, GetPos_FootL(), 3.0f);
		if (actor->anim->GetPlayTime() >= 49.0f / 171.0f) SOUND->Play(SDKEY::footstep_c01, soundBoard, SoundCondition::SE_SPACE5, SDTYPE::SE, 0.75f, GetObj_FootL());
		if (actor->anim->GetPlayTime() >= 76.0f / 171.0f) SOUND->Play(SDKEY::footstep_c02, soundBoard, SoundCondition::SE_SPACE6, SDTYPE::SE, 0.75f, GetObj_FootR());
		if (actor->anim->GetPlayTime() >= 106.0f / 171.0f) SOUND->Play(SDKEY::footstep_c03, soundBoard, SoundCondition::SE_SPACE7, SDTYPE::SE, 0.75f, GetPos_FootL());
		if (actor->anim->GetPlayTime() >= 153.0f / 171.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE8, SDTYPE::SE, 0.75f, GetPos_FootR());

		if (actor->anim->GetPlayTime() > 30.0f / 171.0f) SOUND->Play(SDKEY::attack_pole00, soundBoard, SoundCondition::SE_SPACE9, SDTYPE::SE, 1.0f, GetObj_WeaponBlade(), 20.0f);
		if (actor->anim->GetPlayTime() > 55.0f / 171.0f) SOUND->Play(SDKEY::attack_pole03, soundBoard, SoundCondition::SE_SPACE10, SDTYPE::SE, 1.0f, GetObj_WeaponBlade(), 20.0f);
		if (actor->anim->GetPlayTime() > 90.0f / 171.0f)
			tempChannel = SOUND->Play(SDKEY::attack_pole04, soundBoard, SoundCondition::SE_SPACE11, SDTYPE::SE, 1.0f, GetObj_WeaponBlade(), 20.0f);

		if (actor->anim->GetPlayTime() >= 106.0f / 171.0f)
		{
			if (tempChannel) tempChannel->channel->stop();
			SOUND->Play(SDKEY::explosion00, soundBoard, SoundCondition::SE_SPACE12, SDTYPE::SE, 1.0f, GetObj_WeaponBlade(), 20.0f);
		}
		if (actor->anim->GetPlayTime() >= 125.0f / 171.0f)
		{
			SOUND->Play(SDKEY::steam00, soundBoard, SoundCondition::SE_SPACE13, SDTYPE::SE, 0.5f, actor);
			SOUND->Play(SDKEY::steam00, soundBoard, SoundCondition::SE_SPACE14, SDTYPE::SE, 0.25f, GetObj_FootL());
			SOUND->Play(SDKEY::steam00, soundBoard, SoundCondition::SE_SPACE15, SDTYPE::SE, 0.25f, GetObj_FootR());
		}
		break;
	case AniAction::ATTACK_STOMP:
		SOUND->Play(SDKEY::robot04, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() > 23.0f / 77.0f)
			tempChannel = SOUND->Play(SDKEY::evade03, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, GetObj_FootL());

		if (actor->anim->GetPlayTime() >= 30.0f / 77.0f)
		{
			if (tempChannel) tempChannel->channel->stop();
			SOUND->Play(SDKEY::footstep_c02, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 1.0f, GetObj_FootL(), 20.0f);
			SOUND->Play(SDKEY::explosion01, soundBoard, SoundCondition::SE_SPACE3, SDTYPE::SE, 1.0f, GetPos_FootL(), 30.0f);
		}
		if (actor->anim->GetPlayTime() >= 50.0f / 77.0f) SOUND->Play(SDKEY::steam00, soundBoard, SoundCondition::SE_SPACE4, SDTYPE::SE, 1.0f, GetObj_FootL());
		if (actor->anim->GetPlayTime() >= 60.0f / 77.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE5, SDTYPE::SE, 0.5f, GetPos_FootL());
		break;
	case AniAction::TAUNT:
		if (actor->anim->GetPlayTime() >= 5.0f / 72.0f) SOUND->Play(SDKEY::robot05, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.5f, actor);
		if (actor->anim->GetPlayTime() >= 27.0f / 72.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.05f, GetPos_FootR(), 3.0f);
		if (actor->anim->GetPlayTime() >= 29.0f / 72.0f) SOUND->Play(SDKEY::hit_pole00, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 1.0f, GetPos_WeaponHandle(), 30.0f);
		if (actor->anim->GetPlayTime() >= 31.0f / 72.0f) SOUND->Play(SDKEY::robot07, soundBoard, SoundCondition::SE_SPACE3, SDTYPE::SE, 1.0f, actor, 50.0f);
		if (actor->anim->GetPlayTime() >= 58.0f / 72.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE4, SDTYPE::SE, 0.05f, GetPos_FootR(), 3.0f);
		break;
	case AniAction::ATTACK_JUMP_BEGIN:
		SOUND->Play(SDKEY::robot01, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		SOUND->Play(SDKEY::robot02, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 19.0f / 44.0f) SOUND->Play(SDKEY::footstep_c01, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() > 26.0f / 44.0f)
		{
			SOUND->Play(SDKEY::steam00, soundBoard, SoundCondition::SE_SPACE3, SDTYPE::SE, 1.0f, GetObj_FootL());
			SOUND->Play(SDKEY::steam00, soundBoard, SoundCondition::SE_SPACE4, SDTYPE::SE, 1.0f, GetObj_FootR());
		}
		if (actor->anim->GetPlayTime() > 27.0f / 44.0f) SOUND->Play(SDKEY::tremble00, soundBoard, SoundCondition::SE_SPACE5, SDTYPE::SE, 1.0f, pos, 25.0f);
		break;
	case AniAction::ATTACK_JUMP_END:
		if (actor->anim->GetPlayTime() >= 3.0f / 55.0f)
		{
			SOUND->Play(SDKEY::explosion01, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.5f, GetPos_FootR(), 30.0f);
			SOUND->Play(SDKEY::explosion01, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.5f, GetPos_AnkleL(), 30.0f);
			SOUND->Play(SDKEY::explosion01, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 0.5f, GetPos_WeaponBlade(), 30.0f);
		}
		if (actor->anim->GetPlayTime() >= 20.0f / 55.0f) SOUND->Play(SDKEY::steam01, soundBoard, SoundCondition::SE_SPACE3, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() > 37.0f / 55.0f)
		{
			SOUND->Play(SDKEY::steam00, soundBoard, SoundCondition::SE_SPACE4, SDTYPE::SE, 0.35f, GetObj_FootL());
			SOUND->Play(SDKEY::steam00, soundBoard, SoundCondition::SE_SPACE5, SDTYPE::SE, 0.35f, GetObj_FootR());
		}

		if (actor->anim->GetPlayTime() >= 17.0f / 55.0f) SOUND->Play(SDKEY::robot02, soundBoard, SoundCondition::SE_SPACE6, SDTYPE::SE, 0.5f, actor);

		if (actor->anim->GetPlayTime() >= 37.0f / 55.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE7, SDTYPE::SE, 0.5f, GetPos_FootL());
		if (actor->anim->GetPlayTime() >= 49.0f / 55.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE8, SDTYPE::SE, 0.1f, GetPos_FootR(), 5.0f);
		break;
	case AniAction::BACKSTEP:
		if (actor->anim->GetPlayTime() >= 5.0f / 33.0f) SOUND->Play(SDKEY::robot03, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor, 20.0f);
		if (actor->anim->GetPlayTime() >= 9.0f / 33.0f)
		{
			SOUND->Play(SDKEY::evade03, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
			//SOUND->Play(SDKEY::steam00, soundList, SoundType::SE_SPACE2, SDTYPE::SE, 0.65f, actor);
			//SOUND->Play(SDKEY::steam00, soundList, SoundType::SE_SPACE3, SDTYPE::SE, 0.35f, GetObj_FootL());
		}
		if (actor->anim->GetPlayTime() >= 20.0f / 33.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE4, SDTYPE::SE, 1.0f, GetPos_FootR());
		if (actor->anim->GetPlayTime() >= 25.0f / 33.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE5, SDTYPE::SE, 1.0f, GetPos_FootL());
		break;
	case AniAction::STUN_L:
	case AniAction::STUN_R:
	{
		Vector3 firstStepPos  = (actor->anim->PlayingIdx() == AniAction::STUN_L) ? GetPos_FootL() : GetPos_FootR();
		Vector3 secondStepPos = (actor->anim->PlayingIdx() == AniAction::STUN_L) ? GetPos_FootR() : GetPos_FootL();

		SOUND->Play(SDKEY::robot04, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		SOUND->Play((SDKEY)((int)SDKEY::spark00 + RANDOM->Int(0, 1)), soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 9.0f / 39.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 1.0f, firstStepPos);
		if (actor->anim->GetPlayTime() >= 38.0f / 39.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE3, SDTYPE::SE, 0.5f, secondStepPos);
		break;
	}
	case AniAction::GROGGY_BEGIN:
		soundTime = 1.0f;
		SOUND->Play(SDKEY::spark02, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.75f, actor);
		SOUND->Play(SDKEY::robot02, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.5f, actor);
		if (actor->anim->GetPlayTime() >= 5.0f / 39.0f) SOUND->Play(SDKEY::robot08, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 0.75f, actor);
		if (actor->anim->GetPlayTime() >= 21.0f / 39.0f)
		{
			SDKEY tempKey = (SDKEY)((int)SDKEY::crash01 + RANDOM->Int(0, 1));
			SOUND->Play(tempKey, soundBoard, SoundCondition::SE_SPACE3, SDTYPE::SE, 0.5f, GetObj_AnkleL(), 20.0f);
			SOUND->Play(tempKey, soundBoard, SoundCondition::SE_SPACE4, SDTYPE::SE, 0.5f, actor, 20.0f);
			SOUND->Play(SDKEY::hit_pole00, soundBoard, SoundCondition::SE_SPACE5, SDTYPE::SE, 1.0f, GetPos_AnkleL(), 20.0f);
		}
		break;
	case AniAction::GROGGY_ING:
		if (soundTime <= 0.0f)
		{
			SOUND->Play((SDKEY)((int)SDKEY::spark00 + RANDOM->Int(0, 1)), SDTYPE::SE, 1.0f, actor);
			soundTime = 2.0f;
		}
		else
			soundTime -= DELTA_NS;
		break;
	case AniAction::GROGGY_END:
		SOUND->Play(SDKEY::robot01, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.75f, actor);
		SOUND->Play(SDKEY::robot02, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.5f, actor);
		SOUND->Play(SDKEY::steam01, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 0.75f, actor);
		if (actor->anim->GetPlayTime() >= 26.0f / 50.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE3, SDTYPE::SE, 0.5f, GetPos_FootL());
		break;
	case AniAction::DEATH:
		SOUND->Play(SDKEY::robot02, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		SOUND->Play(SDKEY::explosion02, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		SOUND->Play(SDKEY::spark02, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 10.0f / 180.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE3, SDTYPE::SE, 0.5f, GetPos_FootR());
		if (actor->anim->GetPlayTime() >= 35.0f / 180.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE4, SDTYPE::SE, 0.1f, GetPos_FootL(), 5.0f);
		if (actor->anim->GetPlayTime() >= 70.0f / 180.0f) SOUND->Play(SDKEY::spark03, soundBoard, SoundCondition::SE_SPACE5, SDTYPE::SE, 0.5f, actor);
		if (actor->anim->GetPlayTime() >= 75.0f / 180.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE6, SDTYPE::SE, 0.1f, GetPos_FootR(), 3.0f);
		if (actor->anim->GetPlayTime() >= 130.0f / 180.0f)
		{
			SOUND->Play(SDKEY::robot02, soundBoard, SoundCondition::SE_SPACE7, SDTYPE::SE, 0.5f, actor);
			SOUND->Play(SDKEY::spark03, soundBoard, SoundCondition::SE_SPACE8, SDTYPE::SE, 0.5f, actor);
			SOUND->Play(SDKEY::robot08, soundBoard, SoundCondition::SE_SPACE9, SDTYPE::SE, 0.75f, actor);
		}
		if (actor->anim->GetPlayTime() >= 150.0f / 180.0f) SOUND->Play(SDKEY::crash03, soundBoard, SoundCondition::SE_SPACE10, SDTYPE::SE, 1.0f, actor, 20.0f);
		break;
	case AniAction::CINEMATIC_001_08:
		if (actor->anim->GetPlayTime() >= 10.0f / 60.0f) SOUND->Play(SDKEY::robot09, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor->Find("Root_Rot"));
		break;
	case AniAction::CINEMATIC_001_10:
		SOUND->Play(SDKEY::robot03, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor->Find("Root_Rot"), 20.0f);
		if (actor->anim->GetPlayTime() >= 13.0f / 35.0f) SOUND->Play(SDKEY::footstep_c01, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor->Find("Root_Rot")->GetWorldPos(), 20.0f);
		if (actor->anim->GetPlayTime() > 18.0f / 35.0f) SOUND->Play(SDKEY::steam00, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 1.0f, actor->Find("Root_Rot"), 20.0f);
		break;
	case AniAction::CINEMATIC_001_12:
		if (actor->anim->GetPlayTime() >= 10.0f / 60.0f && soundBoard[SoundCondition::SE_SPACE0] == false)
		{
			INPUT->SetVibration(1.0f, 0.9f, 0.6f);
			SOUND->Play(SDKEY::explosion01, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor->Find("Root_Rot")->GetWorldPos(), 50.0f);
		}
		break;
	case AniAction::CINEMATIC_001_13:
		if (actor->anim->GetPlayTime() >= 10.0f / 165.0f) SOUND->Play(SDKEY::robot05, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor->Find("Root_Rot"));
		if (actor->anim->GetPlayTime() >= 40.0f / 165.0f) SOUND->Play(SDKEY::robot06, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor->Find("Root_Rot"), 50.0f);
		if (actor->anim->GetPlayTime() >= 80.0f / 165.0f) SOUND->Play(SDKEY::robot02, soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 1.0f, actor->Find("Root_Rot"), 20.0f);
		if (actor->anim->GetPlayTime() >= 95.0f / 165.0f) SOUND->Play(SDKEY::misc00, soundBoard, SoundCondition::SE_SPACE3, SDTYPE::SE, 1.0f);
		if (actor->anim->GetPlayTime() >= 115.0f / 165.0f) SOUND->Play(SDKEY::footstep_c03, soundBoard, SoundCondition::SE_SPACE4, SDTYPE::SE, 0.5f, GetPos_FootR(), 20.0f);
		if (actor->anim->GetPlayTime() >= 115.0f / 165.0f) SOUND->Play(SDKEY::steam01, soundBoard, SoundCondition::SE_SPACE5, SDTYPE::SE, 1.0f, actor->Find("Root_Rot"), 20.0f);
		if (actor->anim->GetPlayTime() >= 130.0f / 165.0f) SOUND->Play(SDKEY::footstep_c00, soundBoard, SoundCondition::SE_SPACE6, SDTYPE::SE, 0.5f, GetPos_FootL());
		break;
	}

	// 모션으로 인한 이벤트 생성
	{
		if (actor->anim->PlayingIdx() == AniAction::GROGGY_BEGIN && !actor->anim->IsPlaying())
		{
			ChangeAnim(AnimationState::LOOP, AniAction::GROGGY_ING, 3.0f);
		}
		else if (actor->anim->PlayingIdx() == AniAction::GROGGY_ING)
		{
			if (groggy >= groggy_max)
			{
				groggy = groggy_max;
				ChangeAnim(AnimationState::ONCE, AniAction::GROGGY_END);
			}
		}

		if (actor->anim->PlayingIdx() == AniAction::ATTACK_JUMP_BEGIN)
		{
			if (!isAir && actor->anim->GetPlayTime() > 28.0f / 44.0f)
				isAir = true;
		}

		if (actor->anim->PlayingIdx() == AniAction::DEATH && actor->anim->GetPlayTime() <= 145.0f / 180.0f)
		{
			if (PLAYER_UI->GetDying() > 0)
				b_deathSceneSkip = true;
			if (!actor->anim->isChanging && !b_deathSceneSkip)
			{
				CameraControl::SetCamState(Camera::CameraState::OTHER_EVENT);
				GAMEOBJ->eventTarget = this;

				if (INPUT->KeyDown(VK_ESCAPE, false) || INPUT->tracker.menu == GamePad::ButtonStateTracker::ButtonState::PRESSED)
					b_deathSceneSkip = true;
			}
			PLAYER_UI->SetBossUI(nullptr);
		}
	}

	/*
	// 엔딩상태라면 자동 리타이어
	if (PLAYER_UI->IsGameEnding() && hp > 0.0f)
	{
		hp = 0.0f;
		ChangeAnim(AnimationState::ONCE, AniAction::DEATH, 0.05f);
	}
	*/

	// 리타이어 시
	if (Check_Death())
	{
		//if (!actor->anim->IsPlaying()) corpseTime += DELTA;
		//if (corpseTime >= maxCorpseTime)
		//{
		//	corpseTime = 0.0f;
		//	Enable = false;
		//	return;
		//}
	}
	else if (!isAir && !Check_CutScene() && (!actor->anim->IsPlaying() ||
		(!isMoved && actor->anim->PlayingIdx() == AniAction::WALK)))
	{
		// 지상상태에서 이동 외 특별한 조작이 없으면 통상모션으로 변경
		ChangeAnim(AnimationState::LOOP, AniAction::IDLE);
	}

	// 피격으로 인해 경직이 발생하여 동작이 취소되면 현재 실행중인 패턴을 종료상태로 하고 time을 초기화한다.
	if (damageInfo.size() > 0)
	{
		// 특정 상황에 대하여 그로기, 스턴 면역 생성
		// 조건: 공중에 있을때, 타운트패턴 사용 중 소환전까지
		bool b_immune = isAir || (actor->anim->PlayingIdx() == AniAction::TAUNT && actor->anim->GetPlayTime() < 29.0f / 72.0f);
		float feet_TotalknockDmg[2] = { 0.0f,0.0f };	// 이 프레임에서의 부위(발)가 받은 스태미나 피해량 <[0]=왼발, [1]=오른발>
		float* feetKnockDmg = nullptr;	// 지정한 부위(발)가 받을 스태미나 피해량

		int n;
		while (damageInfo.size() > 0)
		{
			n = RANDOM->Int(0, damageInfo.size() - 1);

			// 피해내역 중 하나만 골라 적용
			Vector3 dir = damageInfo[n].atkPos - pos;
			dir.Normalize();

			{
				Vector3 spawnPos = actor->Find(damageInfo[n].hitName)->GetWorldPos();
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
			if (damageInfo[n].dmgType == DmgNumber::DmgType::ENDURE)
				SOUND->Play(damageInfo[n].guardSound, SDTYPE::SE, 1.0f, damageInfo[n].hitPos);
			else
				SOUND->Play(damageInfo[n].hitSound, SDTYPE::SE, 1.0f, damageInfo[n].hitPos);

			int* partsHp = nullptr;	// 가리키는 부위의 체력
			feetKnockDmg = nullptr;
			float groggyBonus_Feet = 1.0f;
			if (damageInfo[n].hitName == "HitCollider1") partsHp = &parts[PartsName::HEAD].hp_Parts;	// 머리
			else if (damageInfo[n].hitName == "HitCollider2") partsHp = &parts[PartsName::RSHOULDER].hp_Parts;	// 오른쪽 어깨
			else if (damageInfo[n].hitName == "HitCollider3") partsHp = &parts[PartsName::LSHOULDER].hp_Parts;	// 왼쪽 어깨
			else if (damageInfo[n].hitName == "HitCollider6")
			{
				partsHp = &parts[PartsName::RFOOT].hp_Parts;	// 오른쪽 발
				feetKnockDmg = &feet_TotalknockDmg[1];
				groggyBonus_Feet = 1.2f;
			}
			else if (damageInfo[n].hitName == "HitCollider7")
			{
				partsHp = &parts[PartsName::LFOOT].hp_Parts;	// 왼쪽 발
				feetKnockDmg = &feet_TotalknockDmg[0];
				groggyBonus_Feet = 1.2f;
			}

			if (!Check_Unbreakable())
			{
				hp -= damageInfo[n].damage;
				if (partsHp != nullptr)
					*partsHp -= damageInfo[n].damage;
				Decrease_ST(damageInfo[n].knockdown, feetKnockDmg);
				Decrease_Groggy(damageInfo[n].knockdown * groggy_bonus * groggyBonus_Feet);
			}
			damageInfo.erase(damageInfo.begin() + n);
		}

		// 부위파괴
		for (auto it = parts.begin() ; it != parts.end() ; it++)
		{
			if (!Check_Unbreakable() && !it->second.bDestroy && it->second.hp_Parts <= 0)
			{
				if (b_immune)
				{
					it->second.hp_Parts = 1;
					continue;
				}
				SetPartsTexWeights(it->first, it->second.vertices, 1.0f);

				{
					Vector3 spawnPos = it->second.PopLocation->GetWorldPos();
					Pop* temp = dynamic_cast<Pop*>(GAMEOBJ->Generate(PTCTYPE::Effect_Hit, spawnPos)->GetActor());
					temp->scale = { 1.5f,1.5f,1.5f };
					temp->material->diffuse = Color(0.45f, 0.19f, 0.14f, 1.0f);
					temp->duration = 0.75f;
					temp->desc.gravity = 30.0f;
					temp->particleCount = 50;
					temp->velocityScalar = 30.0f;
					temp->Play();
				}
				SOUND->Play(SDKEY::explosion03, SDTYPE::SE, 1.0f, it->second.PopLocation->GetWorldPos(), 20.0f);

				it->second.bDestroy = true;
				Decrease_ST(it->second.destroyDamage, feetKnockDmg);
				Decrease_Groggy(it->second.destroyDamage);
				groggy_bonus += 0.2f;

				switch (it->first)
				{
				case PartsName::HEAD:		// 머리
					GetActor()->Find("HitCollider1")->collider->hit = { 2.5f,2.5f };
					break;
				case PartsName::LSHOULDER:	// 왼쪽어깨
					GetActor()->Find("HitCollider3")->collider->hit = { 1.3f,0.85f };
					break;
				case PartsName::RSHOULDER:	// 오른쪽 어깨(파괴부위)
					GetActor()->Find("HitCollider2")->collider->hit = { 1.3f,0.85f };
					break;
				case PartsName::LFOOT:		// 왼쪽 발
					GetActor()->Find("HitCollider7")->collider->hit = { 0.8f,1.2f };
					break;
				case PartsName::RFOOT:		// 오른쪽 발
					GetActor()->Find("HitCollider6")->collider->hit = { 0.8f,1.2f };
					break;
				}
				PLAYER_UI->PushMessage(it->second.name, InfoMsg::MSG::Destroy);
			}
		}

		if (b_immune)
		{
			if (groggy <= 0.0f) groggy = 1.0f;
			if (st <= 0.0f) st = 1.0f;
		}

		// 사망, 그로기, 스턴
		if (!Check_Unbreakable())
		{
			if (hp <= 0.0f)
			{
				pattern[currentPattern].playing = 3;	// 현재 실행중인 공격패턴이 있으면 종료
				groggy = 0.0f;
				PLAYER_UI->PushMessage(InfoMsg::Subject::IronColossus, InfoMsg::MSG::Kill);
				ChangeAnim(AnimationState::ONCE, AniAction::DEATH);
			}
			else if (groggy <= 0.0f && !Check_Groggy())
			{
				pattern[currentPattern].playing = 3;	// 현재 실행중인 공격패턴이 있으면 종료
				ChangeAnim(AnimationState::ONCE, AniAction::GROGGY_BEGIN);
				actor->anim->aniScale = 0.75f;
				groggy = 0.0f;
				st = st_max;
			}
			else if (st <= 0.0f && !Check_Groggy())
			{
				pattern[currentPattern].playing = 3;	// 현재 실행중인 공격패턴이 있으면 종료
				if (feet_TotalknockDmg[0] > feet_TotalknockDmg[1])
					ChangeAnim(AnimationState::ONCE, AniAction::STUN_L);
				else if (feet_TotalknockDmg[0] < feet_TotalknockDmg[1])
					ChangeAnim(AnimationState::ONCE, AniAction::STUN_R);
				else
					ChangeAnim(AnimationState::ONCE, (RANDOM->Int(0, 1) == 0) ? AniAction::STUN_L : AniAction::STUN_R);
				st = st_max;
			}
		}
		damageInfo.clear();
	}

	// 고스트판정
	if (Check_Death() || Check_CutScene())
		ghost = true;
	else
		ghost = false;

	// 무적판정
	if (Check_CutScene())
		Invincible(true);
	else
		Invincible(false);

	//cout << "(" << destPos.x << ", " << destPos.y << ", " << destPos.z << ")" << endl;

	// 점프
	//if (!isAir && actor->anim->PlayingIdx() == AniAction::JUMP && actor->anim->GetPlayTime() >= 16.0f / 25.0f)
	//{
	//	YSpeed = GetJump(5.0f, 0.5f);
	//	isAir = true;
	//}

	// 자유낙하
	if (isAir)
	{
		if (actor->anim->PlayingIdx() == AniAction::ATTACK_JUMP_BEGIN)
		{
			if (actor->anim->IsPlaying())
				jump_lastYOffset = Vector3::Transform(actor->animOffset, actor->R).y;
			else
				pos.y += jump_lastYOffset;
		}
		else
		{
			YSpeed -= SCENE->gravity * DELTA;
			pos.y += YSpeed * DELTA;
			if (YSpeed < YminSpeed) YSpeed = YminSpeed;
		}
	}

	// 공중상태에서 특별한 조작이 없으면 낙하모션으로 변경
	//if (isAir && !actor->anim->isChanging && actor->anim->currentAnimator.animState == AnimationState::STOP)
	//	ChangeAnim(AnimationState::LOOP, AniAction::FALL, actor->anim->PlayingIdx() == AniAction::JUMP ? 0.35f : 0.2f);

	pos += Vector3::Transform(actor->animOffset, actor->R);

	// 최종이동
	if (!Check_CutScene())
		MoveOnTerrain(pos);

	//if (!lastAir && isAir && !Check_Hit() &&
	//	actor->anim->PlayingIdx() != AniAction::JUMP)
	//{
	//	// 고저차로 인한 자유낙하시
	//	pattern[currentPattern].playing = 3;	// 현재 실행중인 공격패턴이 있으면 종료
	//	ChangeAnim(AnimationState::LOOP, AniAction::FALL);
	//}
	//else if (lastAir && !isAir && !Check_Hit())
	//{
	//	// 땅에 착지할 시
	//	movable = false;
	//	destJump = false;
	//	ChangeAnim(AnimationState::ONCE, AniAction::FALL_LAND, 0.05f);
	//}

	if (lastAir && !isAir && actor->anim->PlayingIdx() == AniAction::ATTACK_JUMP_BEGIN)
	{
		// 땅에 착지할 시 (점프공격)
		float tempAniScale = actor->anim->aniScale;
		destJump = false;
		ChangeAnim(AnimationState::ONCE, AniAction::ATTACK_JUMP_END, 0.0f);
		actor->anim->aniScale = tempAniScale;
		actor->animOffset = { 0.0f,0.0f,0.0f };
	}

	// 모션에 따른 상태변경
	if (hp <= 0.0f || st <= 0.0f || Check_Hit() || Check_Groggy() || Check_CutScene())
	{
		movable = false;
		/*if (hp <= 0.0f || st <= 0.0f)
			movable = false;
		else if (actor->anim->PlayingIdx() == AniAction::JUMP && actor->anim->GetPlayTime() >= 16.0f / 25.0f)
			movable = true;
		else
			movable = false;*/
	}
	else
		movable = true;
}

void Monster_Giant::Render()
{
	if (!Enable) return;
	actor->Render();
	trail->Render();
}

void Monster_Giant::CubeMapRender()
{
	if (!Enable) return;
	actor->CubeMapRender();
}

void Monster_Giant::ShadowMapRender()
{
	if (!Enable) return;
	actor->ShadowMapRender();
}

void Monster_Giant::Init()
{
	Enable = false;

	hp_max = hp = 100000;
	st_max = st = 2000.0f;
	groggy_max = groggy = 10000.0f;
	groggy_bonus = 1.0f;
	weight = 2;
	Swing = 0;
	taunt_level = 0;
	maxCorpseTime = 60.0f;

	for (auto it = parts.begin(); it != parts.end(); it++)
	{
		it->second.bDestroy = false;
		it->second.hp_Parts = it->second.hpMax_Parts;
		SetPartsTexWeights(it->first, it->second.vertices, 0.0f);
	}

	isAir = false;
	movable = false;
	ghost = false;
	YSpeed = 0.0f;
	targetTime = 0.0f;
	beware = false;
	b_deathSceneSkip = false;
	corpseTime = 0.0f;
	currentPattern = PatternType::PT_NONE;
	actor->Find("HitCollider1")->collider->hit = { 2.0f,2.0f };		// 머리(파괴부위)
	actor->Find("HitCollider2")->collider->hit = { 1.1f,0.8f };		// 오른쪽 어깨(파괴부위)
	actor->Find("HitCollider3")->collider->hit = { 1.1f,0.8f };		// 왼쪽어깨(파괴부위)
	actor->Find("HitCollider4")->collider->hit = { 1.5f,1.2f };		// 오른쪽 팔
	actor->Find("HitCollider5")->collider->hit = { 1.5f,1.2f };		// 왼쪽 팔
	actor->Find("HitCollider6")->collider->hit = { 0.25f,1.0f };	// 오른쪽 발(파괴부위)
	actor->Find("HitCollider7")->collider->hit = { 0.25f,1.0f };	// 왼쪽 발(파괴부위)
	actor->Find("HitCollider8")->collider->hit = { 0.7f,0.5f };		// 몸통(앞)
	actor->Find("HitCollider9")->collider->hit = { 0.7f,0.5f };		// 몸통(뒤)
	actor->Find("HitCollider10")->collider->hit = { 1.2f,1.2f };	// 오른쪽 무릎
	actor->Find("HitCollider11")->collider->hit = { 1.2f,1.2f };	// 왼쪽 무릎
	PatternInit();
	ChangeAnim(AnimationState::STOP, AniAction::IDLE, 0.0f);
}

void Monster_Giant::GeneratePatterns()
{
	// 패턴 NONE은 다른 모든 패턴들이 사용불가능 할때 사용되는 패턴이다.
	pattern[PatternType::PT_NONE]
		= PatternData({ AniAction::IDLE }, 1.0f, 0.0f, 3.5f, []() { return false; });
	pattern[PatternType::PT_RECOGNIZE]
		= PatternData({ AniAction::RECOGNIZE }, FLT_MAX, 0.0f, FLT_MAX, []() {return false; });
	pattern[PatternType::PT_ATTACK1]
		= PatternData({ AniAction::ATTACK1 }, FLT_MAX, 0.3f, 20.0f,
			[&]()
			{
				if (pattern[PatternType::PT_ATTACK_STOMP].time <= 0.0f && pattern[PatternType::PT_ATTACK_STOMP].playing == 0 &&
					Vector2::Distance({ actor->GetWorldPos().x, actor->GetWorldPos().z }, { GET_PLAYER->GetActor()->GetWorldPos().x, GET_PLAYER->GetActor()->GetWorldPos().z }) <= 10.0f)
				{
					// 근접할 경우, 발구르기패턴을 사용할 수 있다면 일정확률로 발동되지 않고 다른 패턴을 시도한다.
					return RANDOM->Float() <= 0.33f; // 33%확률로 사용
				}
				else if (lastPattern == PatternType::PT_BACKSTEP && pattern[PatternType::PT_ATTACK_JUMP].time <= 0.0f && pattern[PatternType::PT_ATTACK_JUMP].playing == 0 &&
					Vector2::Distance({ actor->GetWorldPos().x, actor->GetWorldPos().z }, { GET_PLAYER->GetActor()->GetWorldPos().x, GET_PLAYER->GetActor()->GetWorldPos().z }) >= jump_minDistance)
				{
					// 가장 최근에 백스텝패턴을 사용했고, 도약패턴을 사용할 수 있고 그럴만한 거리가 주어졌다면 일정확률로 발동되지 않고 다른 패턴을 시도한다.
					return RANDOM->Float() <= 0.33f; // 33%확률로 사용
				}
				return true;
			}
	);
	//pattern[PatternType::PT_ATTACK].aniScale = 1.5f;
	pattern[PatternType::PT_ATTACK2]
		= PatternData({ AniAction::ATTACK2 }, FLT_MAX, 2.0f, 30.0f,
			[&]()
			{
				if (pattern[PatternType::PT_ATTACK_STOMP].time <= 0.0f && pattern[PatternType::PT_ATTACK_STOMP].playing == 0 &&
					Vector2::Distance({ actor->GetWorldPos().x, actor->GetWorldPos().z }, { GET_PLAYER->GetActor()->GetWorldPos().x, GET_PLAYER->GetActor()->GetWorldPos().z }) <= 10.0f)
				{
					return RANDOM->Float() <= 0.33f; // 33%확률로 사용
				}
				else if (lastPattern == PatternType::PT_BACKSTEP && pattern[PatternType::PT_ATTACK_JUMP].time <= 0.0f && pattern[PatternType::PT_ATTACK_JUMP].playing == 0 &&
					Vector2::Distance({ actor->GetWorldPos().x, actor->GetWorldPos().z }, { GET_PLAYER->GetActor()->GetWorldPos().x, GET_PLAYER->GetActor()->GetWorldPos().z }) >= jump_minDistance)
				{
					// 가장 최근에 백스텝패턴을 사용했고, 도약패턴을 사용할 수 있고 그럴만한 거리가 주어졌다면 일정확률로 발동되지 않고 다른 패턴을 시도한다.
					return RANDOM->Float() <= 0.33f; // 33%확률로 사용
				}
				return true;
				//return Vector3::Distance(actor->GetWorldPos(), GET_PLAYER->GetActor()->GetWorldPos()) >= 15.0f;
			}
	);
	pattern[PatternType::PT_ATTACK3]
		= PatternData({ AniAction::ATTACK3 }, FLT_MAX, 10.0f, 50.0f,
			[&]()
			{
				if (pattern[PatternType::PT_ATTACK_STOMP].time <= 0.0f && pattern[PatternType::PT_ATTACK_STOMP].playing == 0 &&
					Vector2::Distance({ actor->GetWorldPos().x, actor->GetWorldPos().z }, { GET_PLAYER->GetActor()->GetWorldPos().x, GET_PLAYER->GetActor()->GetWorldPos().z }) <= 10.0f)
				{
					return RANDOM->Float() <= 0.5f; // 50%확률로 사용
				}
				return true;
			}
	);
	pattern[PatternType::PT_ATTACK_STOMP]
		= PatternData({ AniAction::ATTACK_STOMP }, FLT_MAX, 0.5f, 10.0f,
			[&]()
			{
				// 근접 시 사용
				if (Vector2::Distance({ actor->GetWorldPos().x, actor->GetWorldPos().z }, { GET_PLAYER->GetActor()->GetWorldPos().x, GET_PLAYER->GetActor()->GetWorldPos().z }) <= 20.0f)
				{
					//플레이어의 높이가 일정치 이상이면 사용빈도 감소 추가
					if (GET_PLAYER->GetActor()->GetWorldPos().y - actor->GetWorldPos().y >= 5.0f)
					{
						return RANDOM->Float() <= 0.15f; // 플레이어가 자신보다 y높이 5이상 높게 뛰었으면 15%확률로 사용
					}
					return true;
				}
				return Vector2::Distance({ actor->GetWorldPos().x, actor->GetWorldPos().z }, { GET_PLAYER->GetActor()->GetWorldPos().x, GET_PLAYER->GetActor()->GetWorldPos().z }) <= 20.0f;
			}
	);
	pattern[PatternType::PT_ATTACK_JUMP]
		= PatternData({ AniAction::ATTACK_JUMP_BEGIN, AniAction::ATTACK_JUMP_END }, FLT_MAX, 12.0f, 150.0f,
			[&]()
			{
				// 거리 50만큼 멀면 사용
				return Vector2::Distance({ actor->GetWorldPos().x, actor->GetWorldPos().z }, { GET_PLAYER->GetActor()->GetWorldPos().x, GET_PLAYER->GetActor()->GetWorldPos().z }) >= jump_minDistance;
			}
	);
	pattern[PatternType::PT_BACKSTEP]
		= PatternData({ AniAction::BACKSTEP }, FLT_MAX, 5.0f, 30.0f,
			[&]()
			{
				// 근접 시 사용
				return Vector2::Distance({ actor->GetWorldPos().x, actor->GetWorldPos().z }, { GET_PLAYER->GetActor()->GetWorldPos().x, GET_PLAYER->GetActor()->GetWorldPos().z }) <= 40.0f;
			}
	);
	pattern[PatternType::PT_TAUNT]
		= PatternData({ AniAction::TAUNT }, FLT_MAX, 0.0f, FLT_MAX, []() { return false; });
}

void Monster_Giant::RenderDetail()
{
	if (!Enable) return;

	ImGui::Text("head hp: %d", parts[PartsName::HEAD].hp_Parts);
	ImGui::Text("leftShoulder hp: %d", parts[PartsName::LSHOULDER].hp_Parts);
	ImGui::Text("rightShoulder hp: %d", parts[PartsName::RSHOULDER].hp_Parts);
	ImGui::Text("leftFoot hp: %d", parts[PartsName::LFOOT].hp_Parts);
	ImGui::Text("rightFoot hp: %d", parts[PartsName::RFOOT].hp_Parts);
	ImGui::Text("currentPattern: %d", currentPattern);
	if (ImGui::Button("BossWeak"))
	{
		hp = 5;
	}
}

bool Monster_Giant::Move(Vector3& pos, Vector3 dir, Vector3 toPlayerDir)
{
	bool can_move = false;
	float mRot = atan2(-dir.x, -dir.z);
	float pRot = atan2(-toPlayerDir.x, -toPlayerDir.z);

	if (Way.empty() && !isAir && !destJump && pattern[currentPattern].playing == 0 && Vector2::Distance({ destPos.x,destPos.z }, { pos.x,pos.z }) < pattern[currentPattern].distance)
	{
		pattern[currentPattern].playing = 1;
	}

	if (actor->anim->IsPlaying() && (actor->anim->PlayingIdx() == AniAction::LTURN || actor->anim->PlayingIdx() == AniAction::RTURN))
	{
		float aniTime = actor->anim->GetPlayTime() * (30.0f / 25.0f);
		if (aniTime > 1.0f) aniTime = 1.0f;
		if (actor->anim->PlayingIdx() == AniAction::LTURN)
			mRot = Util::Lerp(startRot, startRot - PI_DIV2, aniTime);
		else if (actor->anim->PlayingIdx() == AniAction::RTURN)
			mRot = Util::Lerp(startRot, startRot + PI_DIV2, aniTime);

		if ((startRot > destRot && mRot < destRot) ||
			(startRot < destRot && mRot > destRot))
		{
			mRot = destRot;
			can_move = true;
		}
	}
	else
	{
		Vector3 toDestDir = destPos - pos;
		toDestDir.Normalize();
		startRot = mRot;
		destRot = atan2(-toDestDir.x, -toDestDir.z);

		bool isMoving = actor->anim->PlayingIdx() == AniAction::WALK ||
			actor->anim->PlayingIdx() == AniAction::WALK;
		if (fabs((destRot + PI_2) - startRot) < fabs(destRot - startRot))
		{
			startRot -= PI_2;
		}
		else if (fabs((destRot - PI_2) - startRot) < fabs(destRot - startRot))
		{
			startRot += PI_2;
		}
		mRot = startRot;

		if (startRot > destRot)
		{
			if (!isAir && fabs(startRot - destRot) > (isMoving ? PI_DIV4 : 5.0f * TORADIAN))
			{
				ChangeAnim(AnimationState::ONCE, AniAction::LTURN);
			}
			else if (fabs(startRot - destRot) > FLT_EPSILON)
			{
				can_move = true;
				mRot -= PI_DIV2 * DELTA;
				if (mRot < destRot)
					mRot = destRot;
			}
			else
			{
				can_move = true;
			}
		}
		else if (startRot < destRot)
		{
			if (!isAir && fabs(startRot - destRot) > (isMoving ? PI_DIV4 : 5.0f * TORADIAN))
			{
				ChangeAnim(AnimationState::ONCE, AniAction::RTURN);
			}
			else if (fabs(startRot - destRot) > FLT_EPSILON)
			{
				can_move = true;
				mRot += PI_DIV2 * DELTA;
				if (mRot > destRot)
					mRot = destRot;
			}
			else
			{
				can_move = true;
			}
		}
		else
		{
			can_move = true;
		}
	}
	actor->rotation.y = Util::NormalizeAngle(mRot);

	if (can_move)
	{
		//if (!isAir && Vector2::Distance({ destPos.x,destPos.z }, { pos.x,pos.z }) < 1.0f)
		//{
		//	if (Way.empty())
		//	{
		//		return true;
		//	}
		//	else
		//	{
		//		destPos = Way.back();
		//		Way.pop_back();
		//	}
		//}
		if (Way.empty() && !isAir && !destJump && Vector2::Distance({ destPos.x,destPos.z }, { pos.x,pos.z }) < pattern[currentPattern].distance)
		{
			pattern[currentPattern].playing = 2;
			return true;
		}
		else if (!Way.empty() && Vector2::Distance({ destPos.x,destPos.z }, { pos.x,pos.z }) < 1.0f)
		{
			destPos = Way.back();
			Way.pop_back();
		}

		bool jump = false;
		float jump_needDist = 5.0f;
		if (!isAir)
		{
			int idx = Util::GetMyStandMap(pos);
			if (idx >= 0)
			{
				Ray ray = Ray(pos, dir);
				Vector3 Hit;

				Object* ob;
				int col_Idx = 0;
				while (true)
				{
					col_Idx++;
					if (ob = (*SCENE->Map)[idx]->Find("Col" + to_string(col_Idx)))
					{
						if ((ob->collider->gameType & (int)GameType::JUMP) &&	// jump속성 콜라이더만 받는다
							ob->collider->Intersect(ray, Hit))
						{
							float hitDist = Vector3::Distance(pos, Hit);
							if (hitDist < jump_needDist && Vector2::Distance({ destPos.x,destPos.z }, { pos.x,pos.z }) > hitDist)
							{
								jump = true;
								break;
							}
						}
					}
					else
						break;
				}
			}
		}

		//if (jump)
		//{
		//	ChangeAnim(AnimationState::ONCE, AniAction::JUMP);
		//}
		//else
		{
			float speed = 20.0f;
			pos += -actor->GetForward() * speed * DELTA;
			if (!isAir)
				ChangeAnim(AnimationState::LOOP, AniAction::WALK);
		}
	}
	return false;
}

void Monster_Giant::Appear(bool instant, bool activate_beware)
{
	Enable = true;
	actor->visible = true;
	//if (instant)
	//{
	//	ChangeAnim(AnimationState::LOOP, AniAction::IDLE, 0.0f);
	//}
	//else
	//{
	//	ChangeAnim(AnimationState::ONCE, AniAction::CINEMATIC_001, 0.0f, true);
	//	movable = false;
	//}
	ChangeAnim(AnimationState::LOOP, AniAction::IDLE, 0.0f);

	if (activate_beware)
	{
		ChangePattern(PatternType::PT_RECOGNIZE);
		pattern[PatternType::PT_RECOGNIZE].mustUse = true;
		PLAYER_UI->SetBossUI(this);
		beware = true;
	}
	Update();
}

bool Monster_Giant::Check_CutScene()
{
	return PLAYER_UI->GetCinematicLevel() >= 2;
}

void Monster_Giant::SummonMonster(int GOTypeID, UINT num)
{
	Vector3 spawnPosCenter = (*SCENE->Map)[0]->Find("Spawn10")->GetWorldPos();
	Vector3 pos_player = GET_PLAYER->GetActor()->GetWorldPos();
	Vector3 pos_boss = actor->GetWorldPos();
	pos_player.y = pos_boss.y = spawnPosCenter.y;
	for (int tries = 0; tries < 100; tries++)
	{
		Vector3 spawnPos = spawnPosCenter + Vector3(RANDOM->Float(-100.0f, 100.0f), 0, RANDOM->Float(-100.0f, 100.0f));
		Vector3 spawnDir;

		// 소환될 위치가 플레이어나 보스와 너무 가까우면 위치 재변경, 100번 시도 시 조건무시하고 강행
		if (tries < 99 && Vector3::Distance(spawnPos, pos_player) <= 10.0 || Vector3::Distance(spawnPos, pos_boss) <= 10.0)
			continue;

		spawnDir = pos_player - spawnPos;
		GAMEOBJ->Find_Mob(static_cast<GObjectManager::GOType>(GOTypeID), num)->SetWorldPos(spawnPos);
		GAMEOBJ->Find_Mob(static_cast<GObjectManager::GOType>(GOTypeID), num)->GetActor()->rotation.y = atan2(-spawnDir.x, -spawnDir.z);
		Util::NormalizeAngle(actor->rotation.y);
		GAMEOBJ->Find_Mob(static_cast<GObjectManager::GOType>(GOTypeID), num)->Appear(false, true);
		break;
	}
}

void Monster_Giant::AttackPlayerAndMonster(Collider* atkCol)
{
	int orig_dmg = atkCol->atk.damage;
	Attack(atkCol, GameType::HIT_PLAYER);
	atkCol->atk.damage = orig_dmg * 10;		// 몬스터가 피격당하면 가하는 데미지가 10배로 보정
	atkCol->atk.knockdown = orig_dmg * 50;	// 넉다운수치는 50배 보정
	Attack(atkCol, GameType::HIT_MONSTER);
}

void Monster_Giant::DmgCor(int& damage)
{
	damage *= (Check_Groggy(true) ? groggy_bonus : 1.0f);
}

void Monster_Giant::SetPartsTexWeights(PartsName partsName, const vector<int>& vertices, float value)
{
	string meshName;
	switch (partsName)
	{
	case PartsName::HEAD:
	case PartsName::LSHOULDER:
	case PartsName::RSHOULDER:
		meshName = "Torso";
		break;
	case PartsName::LFOOT:
	case PartsName::RFOOT:
		meshName = "Limbs";
		break;
	}

	VertexModel* vertex = (VertexModel*)actor->Find(meshName)->mesh->vertices;
	for (int i = 0; i < vertices.size(); i++)
	{
		vertex[vertices[i]].tex_weights = value;
	}
	actor->Find(meshName)->mesh->UpdateMesh();
}

Vector3 Monster_Giant::GetPos_AnkleL()
{
	return actor->Find("SoundPoint_AnkleL")->GetWorldPos();
}

Object* Monster_Giant::GetObj_AnkleL()
{
	return actor->Find("SoundPoint_AnkleL");
}

Vector3 Monster_Giant::GetPos_FootL()
{
	return actor->Find("SoundPoint_FootL")->GetWorldPos();
}

Object* Monster_Giant::GetObj_FootL()
{
	return actor->Find("SoundPoint_FootL");
}

Vector3 Monster_Giant::GetPos_FootR()
{
	return actor->Find("SoundPoint_FootR")->GetWorldPos();
}

Object* Monster_Giant::GetObj_FootR()
{
	return actor->Find("SoundPoint_FootR");
}

Vector3 Monster_Giant::GetPos_WeaponBlade()
{
	return actor->Find("SoundPoint_WeaponBlade")->GetWorldPos();
}

Object* Monster_Giant::GetObj_WeaponBlade()
{
	return actor->Find("SoundPoint_WeaponBlade");
}

Vector3 Monster_Giant::GetPos_WeaponHandle()
{
	return actor->Find("SoundPoint_WeaponHandle")->GetWorldPos();
}