#include "stdafx.h"

Monster_Drone::Monster_Drone()
{
	actor = Actor::Create();
	actor->LoadFile("Drone.xml");
	if (App.GetAppQuit())
	{
		return;
	}
	actor->rootBoneIndex = actor->Find("Root")->boneIndex;
	actor->SetStandardMatrix({
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, 1, 0, 0,
		0, 0, 0, 1 }
	);

	for (int i = 0; i < SoundCondition::SE_ALL_COUNTS; i++)
		soundBoard.push_back(false);

	GeneratePatterns();
	Init();
	RESOURCE->DecreaseXmlList("Drone.xml");
}

Monster_Drone::~Monster_Drone()
{
	actor->Release();
}

void Monster_Drone::Calculate()
{
	RESOURCE->IncreaseXmlList_AddTotalCapacity("Drone.xml");
}

void Monster_Drone::Update()
{
	if (!Enable) return;
	actor->Update();
}

void Monster_Drone::LateUpdate()
{
	if (!Enable) return;

	Vector3 pos = actor->GetWorldPos();
	Vector3 dir = -actor->GetForward();
	bool lastAir = isAir;
	targetRot = { 0.0f,0.0f,0.0f };

	if (hp > hp_max)
		hp = hp_max;

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
		if (Vector3::Distance(pos, playerPos) <= 75.0f || !damageInfo.empty())
		{
			float _startRot = mRot;
			if (fabs((pRot + PI_2) - mRot) < fabs(pRot - mRot))
			{
				_startRot -= PI_2;
			}
			else if (fabs((pRot - PI_2) - mRot) < fabs(pRot - mRot))
			{
				_startRot += PI_2;
			}
			if (fabs(pRot - _startRot) <= PI_DIV2 || !damageInfo.empty())
			{
				destPos = playerPos;
				ChangePattern(PatternType::PT_RECOGNIZE);
				pattern[PatternType::PT_RECOGNIZE].mustUse = true;
				beware = true;
			}
			// 벽에 가로막혔으면 인식하지 않는것도 추가?
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
						pattern[PatternType::PT_NONE].duration = RANDOM->Float(0.25f, 2.0f);
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
				pattern[PatternType::PT_NONE].duration = RANDOM->Float(0.25f, 2.0f);
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
		case PatternType::PT_BURST:
			if (pattern[currentPattern].playing == 2)
			{
				//if (actor->anim->PlayingIdx() == pattern[currentPattern].actionNum && !actor->anim->IsPlaying())
				if (actor->anim->PlayingIdx() == pattern[currentPattern].actionNums.back() && !actor->anim->IsPlaying())
					pattern[currentPattern].playing = 3;
				//else if (actor->anim->PlayingIdx() != pattern[currentPattern].actionNum)
				else if (find(pattern[currentPattern].actionNums.begin(), pattern[currentPattern].actionNums.end(), actor->anim->PlayingIdx()) == pattern[currentPattern].actionNums.end())
				{
					actor->ClearAttackTouchedList();
					Swing = 0;
					ChangeAnim(AnimationState::ONCE, pattern[currentPattern].actionNums.front());
					actor->anim->aniScale = pattern[currentPattern].aniScale;
				}

				if (currentPattern == PT_BURST && actor->anim->GetPlayTime() > 30.0f / 135.0f)
				{
					float lastRot, curRot;

					lastRot = actor->rotation.y;
					Rotate(toPlayerDir, 90.0f * TORADIAN);
					curRot = actor->rotation.y;

					if (fabs((lastRot + PI_2) - curRot) < fabs(lastRot - curRot))
					{
						curRot -= PI_2;
					}
					else if (fabs((lastRot - PI_2) - curRot) < fabs(lastRot - curRot))
					{
						curRot += PI_2;
					}
					if (curRot - lastRot > TORADIAN)
						targetRot.z = PI_DIV4;
					else if (curRot - lastRot < -TORADIAN)
						targetRot.z = -PI_DIV4;
					else
						targetRot.z = 0.0f;
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

	// Pitch조준
	if (actor->anim->PlayingIdx() == AniAction::ATTACK ||
		actor->anim->PlayingIdx() == AniAction::BURST ||
		actor->anim->PlayingIdx() == AniAction::IDLE ||
		actor->anim->PlayingIdx() == AniAction::MOVE)
	{
		if (actor->anim->PlayingIdx() == AniAction::BURST && actor->anim->GetPlayTime() <= 30.0f / 135.0f)
		{
			targetRot.x = 0.0f;
		}
		else if (actor->anim->PlayingIdx() == AniAction::MOVE)
		{
			targetRot.x = -30.0f * TORADIAN;
		}
		else
		{
			float VecY = playerPos.y - pos.y;
			float VecZ = Vector2::Distance(Vector2(playerPos.x, playerPos.z), Vector2(pos.x, pos.z));
			targetRot.x = atan2(VecY, VecZ);
			Util::Saturate(targetRot.x, -45.0f * TORADIAN, 45.0f * TORADIAN);
		}
	}

	// 모션으로 인한 공격판정 생성
	//{
	//	Object* ob;
	//	for (int i = 1; ob = GetActor()->Find("AttackCollider1_" + to_string(i)); i++)
	//	{
	//		bool cond = actor->anim->PlayingIdx() == AniAction::ATTACK &&
	//			actor->anim->GetPlayTime() >= 22.0f / 68.0f &&
	//			actor->anim->GetPlayTime() <= 28.0f / 68.0f;
	//		ob->collider->enable = cond;
	//		ob->collider->atk.damage = 8;
	//		if (ob->collider->enable)
	//			Attack(ob->collider, GameType::HIT_PLAYER);
	//	}
	//
	//	for (int i = 1; ob = GetActor()->Find("AttackCollider2_" + to_string(i)); i++)
	//	{
	//		bool cond = actor->anim->PlayingIdx() == AniAction::RUSH &&
	//			actor->anim->GetPlayTime() >= 51.0f / 118.0f &&
	//			actor->anim->GetPlayTime() <= 94.0f / 118.0f;
	//		ob->collider->enable = cond;
	//		ob->collider->atk.damage = 18;
	//		ob->collider->atk.kb_velocity = { 15.0f, GetJump(5.0f,0.5f) };
	//		if (ob->collider->enable)
	//			Attack(ob->collider, GameType::HIT_PLAYER);
	//	}
	//}

	// 생존하는 동안 모터사운드 재생
	{
		const float maxVol = 0.5f;
		if (hp > 0.0f)
		{
			if (!motorChannel)
				motorChannel = SOUND->Play(SDKEY::motor00, SDTYPE::SE, 0.0f, actor);
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
	case AniAction::APPEAR:
		if (actor->anim->GetPlayTime() >= 10.0f / 36.0f)
			SOUND->Play(SDKEY::drone00, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		break;
	case AniAction::RECOGNIZE:
		SOUND->Play(SDKEY::drone01, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		break;
	case AniAction::MOVE:
		SOUND->Play(SDKEY::drone03, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		break;
	case AniAction::ATTACK:
		if (actor->anim->GetPlayTime() > 15.0f / 35.0f)
			SOUND->Play((SDKEY)((int)SDKEY::attack_shot00 + RANDOM->Int(0, 5)), soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		break;
	case AniAction::BURST:
	{
		float maxFrame = 135.0f;
		float steps[3] = { 60.0f, 85.0f, 110.0f };
		SOUND->Play(SDKEY::drone02, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		for (int i = 0; i < sizeof(steps) / sizeof(float); i++)
		{
			bool cond = (i % 2 == 0);
			float beginTime = steps[i] / maxFrame;
			float endTime = (i < (sizeof(steps) / sizeof(float)) - 1) ? (steps[i + 1] / maxFrame) : 1.0f;
			int curSpace = cond ? SoundCondition::SE_SPACE1 : SoundCondition::SE_SPACE2;
			int nextSpace = cond ? SoundCondition::SE_SPACE2 : SoundCondition::SE_SPACE1;

			if (actor->anim->GetPlayTime() >= beginTime && actor->anim->GetPlayTime() < endTime)
			{
				SOUND->Play((SDKEY)((int)SDKEY::attack_shot00 + RANDOM->Int(0, 5)), soundBoard, curSpace, SDTYPE::SE, 1.0f, actor);
				soundBoard[nextSpace] = false;
			}
		}
		break;
	}
	case AniAction::HIT:
	case AniAction::FALL:
		SOUND->Play((SDKEY)((int)SDKEY::spark00 + RANDOM->Int(0, 1)), soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 0.5f, actor);
		break;
	case AniAction::DEATH:
		SOUND->Play(SDKEY::crash00, soundBoard, SoundCondition::SE_SPACE0, SDTYPE::SE, 1.0f, actor);
		break;
	}

	// 모션으로 인한 이벤트 생성
	{
		Vector3 aimDir = -actor->Find("Root_Rot")->GetForward();
		//Vector3 spawnPos = pos + actor->scale * (actor->Find("Root_Rot")->GetUp() * 1.5f + aimDir * 1.2f);
		Vector3 spawnPos = actor->Find("Root_Rot")->GetWorldPos() + actor->scale * (aimDir * 1.2f);
		switch (actor->anim->PlayingIdx())
		{
		case AniAction::ATTACK:
			{
				if (Swing == 0 && actor->anim->GetPlayTime() > 15.0f / 35.0f)
				{
					Swing = 1;

					shared_ptr<Bullet_Red> temp = dynamic_pointer_cast<Bullet_Red>(GAMEOBJ->Generate(PTCTYPE::Bullet_Red, spawnPos));
					temp->owner = this;
					temp->GetActor()->rotation = actor->Find("Root_Rot")->rotation;
					temp->GetActor()->scale = { 0.75f,0.75f,0.75f };
					temp->velocity = aimDir * (10.0f * DELTA);
					temp->range = 100.0f;
					temp->col->atk.damage = 5;
					temp->col->atk.hitSound = SDKEY::hit_punch02;
					temp->col->atk.guardSound = SDKEY::guard00;
					temp->Update();
				}
			break;
			}
		case AniAction::BURST:
			{
				if ((Swing == 0 && actor->anim->GetPlayTime() > 60.0f / 135.0f) ||
					(Swing == 1 && actor->anim->GetPlayTime() > 85.0f / 135.0f) ||
					(Swing == 2 && actor->anim->GetPlayTime() > 110.0f / 135.0f))
				{
					Swing++;

					shared_ptr<Bullet_Red> temp = dynamic_pointer_cast<Bullet_Red>(GAMEOBJ->Generate(PTCTYPE::Bullet_Red, spawnPos));
					temp->owner = this;
					temp->GetActor()->rotation = actor->Find("Root_Rot")->rotation;
					temp->GetActor()->scale = { 0.75f,0.75f,0.75f };
					temp->velocity = aimDir * (10.0f * DELTA);
					temp->range = 100.0f;
					temp->col->atk.damage = 5;
					temp->col->atk.hitSound = SDKEY::hit_punch02;
					temp->col->atk.guardSound = SDKEY::guard00;
					temp->Update();
				}
			break;
			}
		}
	}

	// 엔딩상태라면 자동 리타이어
	if (PLAYER_UI->IsGameEnding() && hp > 0.0f)
	{
		hp = 0.0f;
		ChangeAnim(AnimationState::ONCE, AniAction::FALL, 0.05f);
	}

	// 리타이어 시
	if (Check_Death() ||
		(hp <= 0.0f && actor->anim->PlayingIdx() == AniAction::FALL))
	{
		if (!actor->anim->IsPlaying()) corpseTime += DELTA;
		if (corpseTime >= maxCorpseTime)
		{
			corpseTime = 0.0f;
			Enable = false;
			return;
		}
	}
	else if (actor->anim->PlayingIdx() != AniAction::FALL && (!actor->anim->IsPlaying() ||
		(!isMoved && actor->anim->PlayingIdx() == AniAction::MOVE)))
	{
		// 이동 외 특별한 조작이 없으면 통상모션으로 변경
		ChangeAnim(AnimationState::LOOP, AniAction::IDLE);
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
			if(damageInfo[n].dmgType == DmgNumber::DmgType::ENDURE)
				SOUND->Play(damageInfo[n].guardSound, SDTYPE::SE, 1.0f, actor);
			else
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
				PLAYER_UI->PushMessage(InfoMsg::Subject::Drone, InfoMsg::MSG::Kill);
				YSpeed = GetJump(3.0f, 0.5f);
				ChangeAnim(AnimationState::ONCE, AniAction::FALL, 0.05f);
			}
			else if (st <= 0.0f)
			{
				pattern[currentPattern].playing = 3;	// 현재 실행중인 공격패턴이 있으면 종료
				ChangeAnim(AnimationState::ONCE, AniAction::HIT, 0.05f);
				st = st_max;
			}
		}
		damageInfo.clear();
	}

	// 고스트판정
	if (Check_Death() || 
		(actor->anim->PlayingIdx() == AniAction::APPEAR && actor->anim->GetPlayTime() >= 15.0f / 36.0f) ||
		(hp <= 0.0f && actor->anim->PlayingIdx() == AniAction::FALL))
		ghost = true;
	else
		ghost = false;

	// 무적판정
	if ((actor->anim->PlayingIdx() == AniAction::APPEAR && actor->anim->GetPlayTime() >= 15.0f / 36.0f) ||
		(hp <= 0.0f && actor->anim->PlayingIdx() == AniAction::FALL))
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

	if (isAir)
	{
		if (actor->anim->PlayingIdx() == AniAction::FALL || actor->anim->PlayingIdx() == AniAction::DEATH)
		{
			// 자유낙하
			YSpeed -= SCENE->gravity * DELTA;
			pos.y += YSpeed * DELTA;
			if (YSpeed < YminSpeed) YSpeed = YminSpeed;
		}
		else if(actor->anim->PlayingIdx() != AniAction::HIT && actor->anim->PlayingIdx() != AniAction::ATTACK)
		{
			// 고도조절
			int idx = Util::GetMyStandMap(pos);
			Ray ray = Ray(pos, { 0,-1,0 });
			ray.position.y += 100.0f;

			Vector3 Hit;

			if (Util::RayIntersectMap(ray, (*SCENE->Map)[idx], Hit))
			{
				float demandFloating = dist_needFloating;	// 최종적으로 올려야하는 최대고도
				float moveDistY;
				float speed = 6.0f;
				if (pos.y - Hit.y < 0.0f)
				{
					dist_lastFloating = 0.0f;
				}
				else
				{
					dist_lastFloating = pos.y - Hit.y;
				}

				if (jumpingFly)
				{
					demandFloating += (fly_climbY - Hit.y) + 0.5f;
					speed = 8.0f;
				}
				else if (actor->anim->PlayingIdx() == AniAction::MOVE)
				{
					speed = 3.0f;
				}
				else if (actor->anim->PlayingIdx() == AniAction::BURST)
				{
					float addHeight = 6.0f;
					demandFloating += addHeight;
					speed = addHeight * (App.fixFrame / 60.0f);
				}

				if (dist_lastFloating - demandFloating < -fabs(FLT_EPSILON))
					YSpeed = speed;
				else if (dist_lastFloating - demandFloating > fabs(FLT_EPSILON))
					YSpeed = -speed;
				else
					YSpeed = 0.0f;
				
				jumpingFly = false;
				moveDistY = YSpeed * DELTA;
				dist_lastFloating += moveDistY;
				pos.y += moveDistY;
				if ((YSpeed > 0.0f && dist_lastFloating > demandFloating) ||
					(YSpeed < 0.0f && dist_lastFloating < demandFloating))
					pos.y = Hit.y + demandFloating;
			}
		}
	}

	{
		Vector3* rRot = &actor->Find("Root_Rot")->rotation;
		if ((hp <= 0.0f && actor->anim->PlayingIdx() == AniAction::FALL) || Check_Death())
		{
			*rRot = { 0,0,0 };
		}
		else
		{
			if (rRot->z > targetRot.z)
			{
				rRot->z -= ((targetRot.z == 0.0f) ? PI_DIV8 : PI_DIV2) * DELTA;
				if (rRot->z < targetRot.z) rRot->z = targetRot.z;
			}
			else if (rRot->z < targetRot.z)
			{
				rRot->z += ((targetRot.z == 0.0f) ? PI_DIV8 : PI_DIV2) * DELTA;
				if (rRot->z > targetRot.z) rRot->z = targetRot.z;
			}

			if (rRot->x > targetRot.x)
			{
				rRot->x -= PI_DIV2 * DELTA;
				if (rRot->x < targetRot.x) rRot->x = targetRot.x;
			}
			else if (rRot->x < targetRot.x)
			{
				rRot->x += PI_DIV2 * DELTA;
				if (rRot->x > targetRot.x) rRot->x = targetRot.x;
			}
		}
	}

	pos += Vector3::Transform(actor->animOffset, actor->R);
	MoveOnTerrain(pos);
	if (actor->anim->PlayingIdx() != AniAction::FALL && !Check_Death())
		isAir = true;

	if (!lastAir && isAir && !Check_Hit())
	{
		// 고저차로 인한 자유낙하시
	}
	else if (lastAir && !isAir && !Check_Hit())
	{
		// 땅에 착지할 시
		//movable = false;
		destJump = false;
		//ChangeAnim(AnimationState::ONCE, AniAction::FALL_LAND, 0.05f);
	}

	if (!isAir && actor->anim->PlayingIdx() == AniAction::FALL)
		ChangeAnim(AnimationState::ONCE, AniAction::DEATH, 0.05f);

	// 모션에 따른 상태변경
	if (hp <= 0.0f || st <= 0.0f || Check_Hit() ||
		actor->anim->PlayingIdx() == AniAction::APPEAR)
	{
		if (hp <= 0.0f || st <= 0.0f)
			movable = false;
		else if (false)
			movable = true;
		else
			movable = false;
	}
	else
		movable = true;
}

void Monster_Drone::Render()
{
	if (!Enable) return;
	actor->Render();
}

void Monster_Drone::ShadowMapRender()
{
	if (!Enable) return;
	actor->ShadowMapRender();
}

void Monster_Drone::CubeMapRender()
{
	if (!Enable) return;
	actor->CubeMapRender();
}

void Monster_Drone::Init()
{
	hp_max = hp = 2800;
	st_max = st = 300.0f;
	weight = 1;
	isAir = true;
	movable = false;
	ghost = false;
	YSpeed = 0.0f;
	targetTime = 0.0f;
	Enable = false;
	beware = false;
	corpseTime = 0.0f;
	dist_needFloating = 7.0f;
	jumpingFly = false;
	currentPattern = PatternType::PT_NONE;
	actor->Find("Root_Rot")->rotation = { 0,0,0 };
	targetRot = { 0,0,0 };
	Swing = 0;
	motorChannel = nullptr;
	PatternInit();
	ChangeAnim(AnimationState::STOP, AniAction::IDLE, 0.0f);
}

void Monster_Drone::GeneratePatterns()
{
	// 패턴 NONE은 다른 모든 패턴들이 사용불가능 할때 사용되는 패턴이다.
	pattern[PatternType::PT_NONE]
		= PatternData({ AniAction::IDLE }, 0.25f, 0.0f, 20.0f, []() { return false; });
	pattern[PatternType::PT_RECOGNIZE]
		= PatternData({ AniAction::RECOGNIZE }, FLT_MAX, 0.0f, FLT_MAX, []() {return false; });
	pattern[PatternType::PT_ATTACK]
		= PatternData({ AniAction::ATTACK }, FLT_MAX, 0.2f, 20.0f,
			[&]()
			{
				return dist_lastFloating < dist_needFloating + 0.5f;
			}
	);

	pattern[PatternType::PT_BURST]
		= PatternData({ AniAction::BURST }, FLT_MAX, 7.0f, 25.0f,
			[&]()
			{
				return dist_lastFloating < dist_needFloating + 0.5f;
			}
	);
	pattern[PatternType::PT_BURST].first_cooldown = 10.0f;
}

bool Monster_Drone::Move(Vector3& pos, Vector3 dir, Vector3 toPlayerDir)
{
	bool can_move = false;
	float mRot = atan2(-dir.x, -dir.z);
	float pRot = atan2(-toPlayerDir.x, -toPlayerDir.z);

	if (Way.empty() && !destJump && pattern[currentPattern].playing == 0 && Vector2::Distance({ destPos.x,destPos.z }, { pos.x,pos.z }) < pattern[currentPattern].distance)
	{
		pattern[currentPattern].playing = 1;
	}

	/*
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
	else*/
	{
		Vector3 toDestDir = destPos - pos;
		toDestDir.Normalize();
		startRot = mRot;
		destRot = atan2(-toDestDir.x, -toDestDir.z);

		bool isMoving = actor->anim->PlayingIdx() == AniAction::MOVE;
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
			if (fabs(startRot - destRot) > (isMoving ? PI_DIV4 : 5.0f * TORADIAN))
			{
				//ChangeAnim(AnimationState::ONCE, AniAction::LTURN);
				mRot -= PI * DELTA;
				if (mRot < destRot)
					mRot = destRot;
				targetRot.z = -PI_DIV4;
			}
			else if (fabs(startRot - destRot) > FLT_EPSILON)
			{
				can_move = true;
				mRot -= PI_DIV2 * DELTA;
				if (mRot < destRot)
					mRot = destRot;
				targetRot.z = -10.0f * DELTA;
			}
			else
			{
				can_move = true;
			}
		}
		else if (startRot < destRot)
		{
			if (fabs(startRot - destRot) > (isMoving ? PI_DIV4 : 5.0f * TORADIAN))
			{
				//ChangeAnim(AnimationState::ONCE, AniAction::RTURN);
				mRot += PI * DELTA;
				if (mRot > destRot)
					mRot = destRot;
				targetRot.z = PI_DIV4;
			}
			else if (fabs(startRot - destRot) > FLT_EPSILON)
			{
				can_move = true;
				mRot += PI_DIV2 * DELTA;
				if (mRot > destRot)
					mRot = destRot;
				targetRot.z = 10.0f * DELTA;
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
		if (Way.empty() && !destJump && Vector2::Distance({ destPos.x,destPos.z }, { pos.x,pos.z }) < pattern[currentPattern].distance)
		{
			pattern[currentPattern].playing = 2;
			return true;
		}
		else if (!Way.empty() && Vector2::Distance({ destPos.x,destPos.z }, { pos.x,pos.z }) < 1.0f)
		{
			destPos = Way.back();
			Way.pop_back();
		}

		bool bStop = false;
		float jump_needDist = 10.0f;
		float jump_minDist = 3.0f;

		{
			int idx = Util::GetMyStandMap(pos);
			if (idx >= 0)
			{
				Ray ray = Ray(pos, dir);
				ray.position.y -= dist_lastFloating;
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
								if (hitDist < jump_minDist) bStop = true;
								jumpingFly = true;
								fly_climbY = ob->collider->GetWorldPos().y + ob->collider->S._22;
								break;
							}
						}
					}
					else
						break;
				}
			}
		}

		if(!bStop)
		{
			float speed = 25.0f;
			pos += -actor->GetForward() * speed * DELTA;
			ChangeAnim(AnimationState::LOOP, AniAction::MOVE);
		}
	}
	return false;
}

void Monster_Drone::Appear(bool instant, bool activate_beware)
{
	Enable = true;
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

void Monster_Drone::Spawn(Object* point)
{
	GameObject::Spawn(point);
	actor->MoveWorldPos({ 0.0f,dist_needFloating,0.0f });
}
