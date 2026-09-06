#include "stdafx.h"

inline Sound::SoundKey Monster_Goblin::SDKEY_FootstepA()
{
	return (SDKEY)((int)SDKEY::footstep_a00 + RANDOM->Int(0, 9));
}

inline Sound::SoundKey Monster_Goblin::SDKEY_FootstepB()
{
	return (SDKEY)((int)SDKEY::footstep_b00 + RANDOM->Int(0, 9));
}

Monster_Goblin::Monster_Goblin()
{
	actor = Actor::Create();
	actor->LoadFile("Goblin.xml");
	if (App.GetAppQuit())
	{
		return;
	}
	actor->rootBoneIndex = actor->Find("Root")->boneIndex;

	for (int i = 0; i < SoundCondition::SE_ALL_COUNTS; i++)
		soundBoard.push_back(false);

	GeneratePatterns();
	Init();
	RESOURCE->DecreaseXmlList("Goblin.xml");
}

Monster_Goblin::~Monster_Goblin()
{
	actor->Release();
}

void Monster_Goblin::Calculate()
{
	RESOURCE->IncreaseXmlList_AddTotalCapacity("Goblin.xml");
}

void Monster_Goblin::Update()
{
	if (!Enable) return;
	actor->Update();
}

void Monster_Goblin::LateUpdate()
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
		if (Vector3::Distance(pos, playerPos) <= 50.0f || !damageInfo.empty())
		{
			float startRot = mRot;
			if (fabs((pRot + PI_2) - mRot) < fabs(pRot - mRot))
			{
				startRot -= PI_2;
			}
			else if (fabs((pRot - PI_2) - mRot) < fabs(pRot - mRot))
			{
				startRot += PI_2;
			}
			if (fabs(pRot - startRot) <= PI_DIV2 || !damageInfo.empty())
			{
				destPos = playerPos;
				ChangePattern(PatternType::PT_RECOGNIZE);
				pattern[PatternType::PT_RECOGNIZE].mustUse = true;
				beware = true;
			}
		}
	}
	if (beware && GET_PLAYER->IsResurrection())
	{
		beware = false;
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
					if (!RandomPattern())
					{
						pattern[PatternType::PT_NONE].duration = RANDOM->Float(0.5f, 1.5f);
						ChangePattern(PatternType::PT_NONE);
					}
				}
			}
		}
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
			if (!RandomPattern())
			{
				// 적당한 패턴이 없으면 플레이어 추적만 실행한다
				pattern[PatternType::PT_NONE].duration = RANDOM->Float(0.5f, 1.5f);
				ChangePattern(PatternType::PT_NONE);
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
				else if (find(pattern[currentPattern].actionNums.begin(), pattern[currentPattern].actionNums.end(), actor->anim->PlayingIdx()) == pattern[currentPattern].actionNums.end())
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
		case PatternType::PT_ATTACK:
		case PatternType::PT_RUSH:
			if (pattern[currentPattern].playing == 2)
			{
				//if (actor->anim->PlayingIdx() == pattern[currentPattern].actionNum && !actor->anim->IsPlaying())
				if (actor->anim->PlayingIdx() == pattern[currentPattern].actionNums.back() && !actor->anim->IsPlaying())
					pattern[currentPattern].playing = 3;
				//else if (actor->anim->PlayingIdx() != pattern[currentPattern].actionNum)
				else if (find(pattern[currentPattern].actionNums.begin(), pattern[currentPattern].actionNums.end(), actor->anim->PlayingIdx()) == pattern[currentPattern].actionNums.end())
				{
					actor->ClearAttackTouchedList();
					ChangeAnim(AnimationState::ONCE, pattern[currentPattern].actionNums.front());
					actor->anim->aniScale = pattern[currentPattern].aniScale;
				}

				if (currentPattern == PT_RUSH && actor->anim->GetPlayTime() <= 44.0f / 118.0f)
					Rotate(toPlayerDir, 45.0f * TORADIAN);
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
		for (int i = 1; ob = GetActor()->Find("AttackCollider1_" + to_string(i)); i++)
		{
			bool cond = actor->anim->PlayingIdx() == AniAction::ATTACK &&
				actor->anim->GetPlayTime() >= 22.0f / 68.0f &&
				actor->anim->GetPlayTime() <= 28.0f / 68.0f;
			ob->collider->enable = cond;
			ob->collider->atk.damage = 8;
			ob->collider->atk.hitSound = SDKEY::hit_punch00;
			ob->collider->atk.guardSound = SDKEY::guard00;
			if (ob->collider->enable)
				Attack(ob->collider, GameType::HIT_PLAYER);
		}

		for (int i = 1; ob = GetActor()->Find("AttackCollider2_" + to_string(i)); i++)
		{
			bool cond = actor->anim->PlayingIdx() == AniAction::RUSH &&
				actor->anim->GetPlayTime() >= 51.0f / 118.0f &&
				actor->anim->GetPlayTime() <= 94.0f / 118.0f;
			ob->collider->enable = cond;
			ob->collider->atk.damage = 18;
			ob->collider->atk.kb_velocity = { 15.0f, GetJump(5.0f,0.5f) };
			ob->collider->atk.hitSound = SDKEY::hit_punch01;
			ob->collider->atk.guardSound = SDKEY::guard01;
			if (ob->collider->enable)
				Attack(ob->collider, GameType::HIT_PLAYER);
		}
	}

	// 모션으로 인한 사운드 생성
	switch (actor->anim->PlayingIdx())
	{
	case AniAction::APPEAR:
		if (actor->anim->GetPlayTime() >= 4.0f / 67.0f)
		{
			SOUND->Play(SDKEY::tremble00, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.5f, pos);
		}
		break;
	case AniAction::WALK:
	{
		if (actor->anim->GetPlayTime() >= 12.0f / 35.0f)
			SOUND->Play(SDKEY_FootstepA(), soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.30f, pos);
		if (actor->anim->GetPlayTime() >= 29.5f / 35.0f)
			SOUND->Play(SDKEY_FootstepA(), soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.30f, pos);

		if (actor->anim->IsLooped())
		{
			soundBoard[SoundCondition::SE_SPACE0] = false;
			soundBoard[SoundCondition::SE_SPACE1] = false;
		}
		break;
	}
	case AniAction::RUN:
	{
		if (actor->anim->GetPlayTime() >= 8.0f / 21.0f)
			SOUND->Play(SDKEY_FootstepA(), soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.35f, pos);
		if (actor->anim->GetPlayTime() >= 18.5f / 21.0f)
			SOUND->Play(SDKEY_FootstepA(), soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.35f, pos);

		if (actor->anim->IsLooped())
		{
			soundBoard[SoundCondition::SE_SPACE0] = false;
			soundBoard[SoundCondition::SE_SPACE1] = false;
		}
		break;
	}
	case AniAction::RECOGNIZE:
		if (actor->anim->GetPlayTime() >= 10.0f / 55.0f)
			SOUND->Play((SDKEY)((int)SDKEY::goblin00 + RANDOM->Int(0, 2)), soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		break;
	case AniAction::BATTLE_IDLE:
		if (soundBoard[SoundCondition::SE_SPACE0] == false)
		{
			if (breathing <= 0)
			{
				SOUND->Play(SDKEY::goblin03, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
				breathing = RANDOM->Int(0, max_breathing);
			}
			else
			{
				soundBoard[SoundCondition::SE_SPACE0] = true;
				breathing--;
			}
		}

		if (actor->anim->IsLooped())
		{
			soundBoard[SoundCondition::SE_SPACE0] = false;
			breathing--;
		}
		break;
	case AniAction::LTURN:
		if (soundBoard[SoundCondition::SE_SPACE0] == false)
		{
			if (breathing <= 0)
			{
				SOUND->Play(SDKEY::goblin03, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
				breathing = RANDOM->Int(0, max_breathing);
			}
			else
			{
				soundBoard[SoundCondition::SE_SPACE0] = true;
				breathing--;
			}
		}
		if (actor->anim->GetPlayTime() >= 14.0f / 31.0f)
			SOUND->Play(SDKEY_FootstepA(), soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.30f, pos);
		if (actor->anim->GetPlayTime() >= 29.0f / 31.0f)
			SOUND->Play(SDKEY_FootstepA(), soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 0.30f, pos);
		break;
	case AniAction::RTURN:
		if (soundBoard[SoundCondition::SE_SPACE0] == false)
		{
			if (breathing <= 0)
			{
				SOUND->Play(SDKEY::goblin03, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
				breathing = RANDOM->Int(0, max_breathing);
			}
			else
			{
				soundBoard[SoundCondition::SE_SPACE0] = true;
				breathing--;
			}
		}
		if (actor->anim->GetPlayTime() >= 17.0f / 31.0f)
			SOUND->Play(SDKEY_FootstepA(), soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.30f, pos);
		if (actor->anim->GetPlayTime() >= 28.0f / 31.0f)
			SOUND->Play(SDKEY_FootstepA(), soundBoard, SoundCondition::SE_SPACE2, SDTYPE::SE, 0.30f, pos);
		break;
	case AniAction::ATTACK:
		if (actor->anim->GetPlayTime() >= 5.0f / 68.0f)
			SOUND->Play((SDKEY)((int)SDKEY::goblin04 + RANDOM->Int(0, 2)), soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 24.0f / 68.0f)
			SOUND->Play(SDKEY::attack_punch00, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 1.0f, actor);
		break;
	case AniAction::RUSH:
	{
		float maxFrame = 118.0f;
		float steps[5] = { 55.0f, 66.0f, 76.0f, 87.0f, 97.0f };
		if (actor->anim->GetPlayTime() >= 10.0f / 118.0f)
			SOUND->Play((SDKEY)((int)SDKEY::goblin07 + RANDOM->Int(0, 1)), soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		for (int i = 0; i < sizeof(steps) / sizeof(float); i++)
		{
			bool cond = (i % 2 == 0);
			float beginTime = steps[i] / maxFrame;
			float endTime = (i < (sizeof(steps) / sizeof(float)) - 1) ? (steps[i + 1] / maxFrame) : 1.0f;
			int curSpace = cond ? SoundCondition::SE_SPACE1 : SoundCondition::SE_SPACE2;
			int nextSpace = cond ? SoundCondition::SE_SPACE2 : SoundCondition::SE_SPACE1;

			if (actor->anim->GetPlayTime() >= beginTime && actor->anim->GetPlayTime() < endTime)
			{
				SOUND->Play(SDKEY_FootstepB(), soundBoard, curSpace, SDTYPE::SE, 0.75f, pos);
				soundBoard[nextSpace] = false;
			}
		}
		break;
	}
	case AniAction::HIT1:
	case AniAction::HIT2:
		SOUND->Play((SDKEY)((int)SDKEY::goblin09 + RANDOM->Int(0, 2)), soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		break;
	case AniAction::JUMP:
		SOUND->Play((SDKEY)((int)SDKEY::goblin04 + RANDOM->Int(0, 2)), soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 16.0f / 25.0f)
			SOUND->Play(SDKEY::evade01, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.3f, pos);
		break;
	case AniAction::DEATH:
		SOUND->Play((SDKEY)((int)SDKEY::goblin12 + RANDOM->Int(0, 2)), soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		if (actor->anim->GetPlayTime() >= 63.0f / 109.0f)
			SOUND->Play(SDKEY::down02, soundBoard, SoundCondition::SE_SPACE1, SDTYPE::SE, 0.5f, actor);
		break;
	}

	// 엔딩상태라면 자동 리타이어
	if (PLAYER_UI->IsGameEnding() && hp > 0.0f)
	{
		hp = 0.0f;
		ChangeAnim(AnimationState::ONCE, AniAction::DEATH, 0.05f);
	}

	// 리타이어 시
	if (Check_Death())
	{
		if (!actor->anim->IsPlaying()) corpseTime += DELTA;
		if (corpseTime >= maxCorpseTime)
		{
			corpseTime = 0.0f;
			Enable = false;
			return;
		}
	}
	else if (!isAir && (!actor->anim->IsPlaying() ||
		(!isMoved && actor->anim->PlayingIdx() == AniAction::RUN)))
	{
		// 지상상태에서 이동 외 특별한 조작이 없으면 통상모션으로 변경
		ChangeAnim(AnimationState::LOOP, beware ? AniAction::BATTLE_IDLE : AniAction::IDLE);
	}

	// 피격으로 인해 경직이 발생하여 동작이 취소되면 현재 실행중인 패턴을 종료상태로 하고 time을 초기화한다.
	if (damageInfo.size() > 0)
	{
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
			SOUND->Play(damageInfo[n].hitSound, SDTYPE::SE, 1.0f, actor);

			if (!Check_Unbreakable())
			{
				hp -= damageInfo[n].damage;
				if (!Check_Hit()) st -= damageInfo[n].knockdown;
			}
			damageInfo.erase(damageInfo.begin() + n);
		}

		if (!Check_Unbreakable())
		{
			if (hp <= 0.0f)
			{
				pattern[currentPattern].playing = 3;	// 현재 실행중인 공격패턴이 있으면 종료
				PLAYER_UI->PushMessage(InfoMsg::Subject::Goblin, InfoMsg::MSG::Kill);
				ChangeAnim(AnimationState::ONCE, AniAction::DEATH, 0.05f);
			}
			else if (st <= 0.0f)
			{
				pattern[currentPattern].playing = 3;	// 현재 실행중인 공격패턴이 있으면 종료
				UINT randHit = (RANDOM->Int(0, 1) == 0) ? AniAction::HIT1 : AniAction::HIT2;
				ChangeAnim(AnimationState::ONCE, randHit, 0.05f);
				if (randHit == AniAction::HIT2) actor->anim->aniScale = 1.5f;
				st = st_max;
			}
		}
		damageInfo.clear();
	}

	// 고스트판정
	if (Check_Death())
		ghost = true;
	else
		ghost = false;

	// 무적판정
	if (false)
		Invincible(true);
	else
		Invincible(false);

	//cout << "(" << destPos.x << ", " << destPos.y << ", " << destPos.z << ")" << endl;

	// 점프
	if (!isAir && actor->anim->PlayingIdx() == AniAction::JUMP && actor->anim->GetPlayTime() >= 16.0f / 25.0f)
	{
		YSpeed = GetJump(5.0f, 0.5f);
		isAir = true;
	}

	// 자유낙하
	if (isAir)
	{
		YSpeed -= SCENE->gravity * DELTA;
		pos.y += YSpeed * DELTA;
		if (YSpeed < YminSpeed) YSpeed = YminSpeed;
	}

	// 공중상태에서 특별한 조작이 없으면 낙하모션으로 변경
	if (isAir && !actor->anim->isChanging && actor->anim->currentAnimator.animState == AnimationState::STOP)
		ChangeAnim(AnimationState::LOOP, AniAction::FALL, actor->anim->PlayingIdx() == AniAction::JUMP ? 0.35f : 0.2f);

	pos += Vector3::Transform(actor->animOffset, actor->R);
	MoveOnTerrain(pos);

	if (!lastAir && isAir && !Check_Hit() &&
		actor->anim->PlayingIdx() != AniAction::JUMP)
	{
		// 고저차로 인한 자유낙하시
		pattern[currentPattern].playing = 3;	// 현재 실행중인 공격패턴이 있으면 종료
		ChangeAnim(AnimationState::LOOP,AniAction::FALL);
	}
	else if (lastAir && !isAir && !Check_Hit())
	{
		// 땅에 착지할 시
		movable = false;
		destJump = false;
		ChangeAnim(AnimationState::ONCE, AniAction::FALL_LAND, 0.05f);
		SOUND->Play(SDKEY_FootstepA(), SDTYPE::SE, 0.7f, pos);
	}

	// 모션에 따른 상태변경
	if (hp <= 0.0f || st <= 0.0f || Check_Hit() || 
		actor->anim->PlayingIdx() == AniAction::JUMP ||
		actor->anim->PlayingIdx() == AniAction::FALL_LAND ||
		actor->anim->PlayingIdx() == AniAction::APPEAR)
	{
		if (hp <= 0.0f || st <= 0.0f)
			movable = false;
		else if (actor->anim->PlayingIdx() == AniAction::JUMP && actor->anim->GetPlayTime() >= 16.0f / 25.0f)
			movable = true;
		else
			movable = false;
	}
	else
		movable = true;
}

void Monster_Goblin::Render()
{
	if (!Enable) return;
	actor->Render();
}

void Monster_Goblin::CubeMapRender()
{
	if (!Enable) return;
	actor->CubeMapRender();
}

void Monster_Goblin::ShadowMapRender()
{
	if (!Enable) return;
	actor->ShadowMapRender();
}

void Monster_Goblin::Init()
{
	hp_max = hp = 3500;
	st_max = st = 550.0f;
	weight = 1;
	isAir = false;
	movable = false;
	ghost = false;
	YSpeed = 0.0f;
	targetTime = 0.0f;
	Enable = false;
	beware = false;
	corpseTime = 0.0f;
	currentPattern = PatternType::PT_NONE;
	breathing = 0;
	PatternInit();
	ChangeAnim(AnimationState::STOP, AniAction::APPEAR, 0.0f);
}

void Monster_Goblin::GeneratePatterns()
{
	// 패턴 NONE은 다른 모든 패턴들이 사용불가능 할때 사용되는 패턴이다.
	pattern[PatternType::PT_NONE]
		= PatternData({ AniAction::BATTLE_IDLE }, 1.0f, 0.0f, 3.5f, []() { return false; });
	pattern[PatternType::PT_RECOGNIZE]
		= PatternData({ AniAction::RECOGNIZE }, FLT_MAX, 0.0f, FLT_MAX, []() {return false; });
	pattern[PatternType::PT_ATTACK]
		= PatternData({ AniAction::ATTACK }, FLT_MAX, 0.3f, 3.5f,
			[]()
			{
				return true;
			}
	);
	//pattern[PatternType::PT_ATTACK].aniScale = 1.5f;
	
	pattern[PatternType::PT_RUSH]
		= PatternData({ AniAction::RUSH }, FLT_MAX, 5.0f, 20.0f, [] {return true; });
	pattern[PatternType::PT_RUSH].first_cooldown = 5.0f;	// 이 패턴은 전투 시작으로부터 5초의 쿨타임을 가짐.
}

bool Monster_Goblin::Move(Vector3& pos, Vector3 dir, Vector3 toPlayerDir)
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
		float aniTime = actor->anim->GetPlayTime();
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

		bool isMoving = actor->anim->PlayingIdx() == AniAction::RUN ||
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

		if (jump)
		{
			ChangeAnim(AnimationState::ONCE, AniAction::JUMP);
		}
		else
		{
			float speed = 20.0f;
			pos += -actor->GetForward() * speed * DELTA;
			if (!isAir)
				ChangeAnim(AnimationState::LOOP, AniAction::RUN);
		}
	}
	return false;
}

void Monster_Goblin::Appear(bool instant, bool activate_beware)
{
	Enable = true;
	breathing = RANDOM->Int(0, max_breathing);
	if (instant)
	{
		ChangeAnim(AnimationState::LOOP, AniAction::IDLE, 0.0f);
	}
	else
	{
		ChangeAnim(AnimationState::ONCE, AniAction::APPEAR, 0.0f, true);
		movable = false;
	}

	if (activate_beware)
	{
		ChangePattern(PatternType::PT_RECOGNIZE);
		pattern[PatternType::PT_RECOGNIZE].mustUse = true;
		beware = true;
	}
	Update();
}
