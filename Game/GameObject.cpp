#include "stdafx.h"

bool GameObject::MoveOnTerrain(Vector3 pos)
{
	bool cont;	// 이동거리가 길면 true(여러차례 계산을 하겠다), 아니면 false(이 계산을 마지막으로 하겠다)
	float len = actor->Find("MoveCollider")->collider->scale.x; // 이동거리의 긺을 판정하는 기준
	Vector3 curPos = GetActor()->GetWorldPos();	// 현 계산에서의 자신의 위치
	float totalLen = Vector3::Distance(curPos, pos);	// 목적지까지의 길이
	Vector3 pathDir = pos - curPos;	// 이동방향
	pathDir.Normalize();
	while (1)
	{
		//이동시작 전 준비
		if (totalLen > len)
		{
			cont = true;
			pos = curPos + (pathDir * len);
			totalLen -= len;
		}
		else
		{
			cont = false;
			pos = curPos + (pathDir * totalLen);
		}
		//----

		int idx = -1;
		Vector3 Localorigin = GetActor()->Find("MoveCollider")->collider->GetLocalPos();

		idx = Util::GetMyStandMap(pos);
		if (idx < 0) return false;

		GetActor()->Find("MoveCollider")->collider->SetWorldPos(pos + Localorigin);
		GetActor()->Find("MoveCollider")->collider->Update();

		// 오브젝트 충돌 계산
		if (!ghost)
		{
			for (auto obj : GAMEOBJ->obj_pool)
			{
				if (!obj.second->Enable || obj.second->ghost || obj.second == this || (weight > obj.second->weight))
					continue;

				Object* tg;
				//!obj.second->GetActor()->Find("MoveCollider"))
				for (int i = 1; tg = obj.second->GetActor()->Find("BlockCollider" + to_string(i)); i++)
				{
					if (GetActor()->Find("MoveCollider")->collider->Intersect(tg->collider))
					{
						Vector3 pushDir = pos - tg->collider->GetWorldPos();
						pushDir.y = 0.0f;
						pushDir.Normalize();

						Vector3 moved = pushDir * (GetActor()->Find("MoveCollider")->collider->S._11 + tg->collider->S._11);
						moved.y = 0.0f;
						pos.x = tg->collider->GetWorldPos().x + moved.x;
						pos.z = tg->collider->GetWorldPos().z + moved.z;
						GetActor()->Find("MoveCollider")->collider->SetWorldPos(pos + Localorigin);
						GetActor()->Find("MoveCollider")->collider->Update();
					}
				}
			}

		}

		// 벽면충돌 계산
		{
			//GetActor()->Find("MoveCollider")->collider->SetLocalPos(Localorigin);
			//GetActor()->Find("MoveCollider")->collider->Update();

			Object* ob;
			Collider* col;
			Vector3 moved = { 0,0,0 };
			int col_Idx = 0;
			while (true)
			{
				col_Idx++;
				if (ob = (*SCENE->Map)[idx]->Find("Col" + to_string(col_Idx)))
				{
					if ((ob->collider->gameType & (int)GameType::ONLY_PLAYER) && GET_PLAYER != this)	// 플레이어전용 col은 플레이어만이 충돌될 수 있다
					{
						continue;
					}
					col = ob->collider;
					if (GetActor()->Find("MoveCollider")->collider->Intersect(col))
					{
						int i;
						//계산식 (지형의 벽collider는 y축회전만 했음을 가정한다)
						{
							// case1. 4방향 레이캐스팅 적중
							Ray wallRay;
							Vector3 rayDir[4] = {
								-col->GetForward(),	// 전 (캐릭터가 벽을 향해 레이캐스팅할때 벽이 캐릭터로 향하는 방향)
								 col->GetForward(),	// 후
								 col->GetRight(),		// 좌
								-col->GetRight()		// 우
							};
							wallRay.position = pos;
							for (i = 0; i < 4; i++)
							{
								wallRay.direction = rayDir[i];
								wallRay.direction.Normalize();
								if (col->Intersect(wallRay, moved))
								{
									moved += -fabs(GetActor()->Find("MoveCollider")->collider->scale.x) * rayDir[i];
									pos = moved;
									GetActor()->Find("MoveCollider")->collider->SetWorldPos(pos + Localorigin);
									GetActor()->Find("MoveCollider")->collider->Update();
									break;
								}
							}
						}
						if (i >= 4)
						{
							// case2. 4모서리 거리재기
							Vector3 rectPos[4] = {
								 col->GetForward() * col->S._33 + col->GetRight() * col->S._11,	// 오른쪽 앞 (사각형의 중심이 원점)
								 col->GetForward() * col->S._33 - col->GetRight() * col->S._11,	// 왼쪽 앞
								-col->GetForward() * col->S._33 + col->GetRight() * col->S._11,	// 오른쪽 뒤
								-col->GetForward() * col->S._33 + -col->GetRight() * col->S._11		// 왼쪽 뒤
							};
							Vector3 wallPos;
							float min_dist = FLT_MAX;
							for (i = 0; i < 4; i++)
							{
								Vector3 tempWall;
								tempWall = col->GetWorldPos() + rectPos[i];
								tempWall.y = pos.y;
								if (Vector3::Distance(pos, tempWall) < min_dist)
								{
									wallPos = tempWall;
									moved = pos - wallPos;
									min_dist = moved.Length();
								}
							}
							moved.Normalize();	// 밀리는 방향
							pos = wallPos + fabs(GetActor()->Find("MoveCollider")->collider->scale.x + 0.1f) * moved;
							GetActor()->Find("MoveCollider")->collider->SetWorldPos(pos + Localorigin);
							GetActor()->Find("MoveCollider")->collider->Update();
						}
					}
				}
				else
					break;
			}
		}
		GetActor()->Find("MoveCollider")->collider->SetLocalPos(Localorigin);
		//GetActor()->Find("MoveCollider")->collider->Update();

		idx = Util::GetMyStandMap(pos);
		if (idx < 0) return false;

		// 지면충돌 계산
		{
			Ray ray = Ray(pos, { 0,-1,0 });
			ray.position.y += 100.0f;

			//bool oneHit = false;
			Vector3 Hit;

			if (Util::RayIntersectMap(ray, (*SCENE->Map)[idx], Hit))
			{
				//if (oneHit)
				//{
				//	if (Vector3::Distance(result, ray.position) > Vector3::Distance(Hit, ray.position))
				//		result = Hit;
				//}
				//else
				//{
				//	result = Hit;
				//	oneHit = true;
				//}
				if (isAir)
				{
					if (pos.y - Hit.y < 0.0f)
					{
						curPos = Hit;
						isAir = false;
					}
					else
					{
						curPos = { Hit.x, pos.y, Hit.z };
					}
				}
				else
				{
					if (pos.y - Hit.y > 1.0f)
					{
						curPos = { Hit.x, pos.y, Hit.z };
						YSpeed = 0.0f;
						isAir = true;
					}
					else
					{
						curPos = Hit;
					}
				}
				if(cont)
					continue;
				else
				{
					actor->SetWorldPos(curPos);
					return true;
				}
			}
		}
		return false;
	}
	return false;
}

void GameObject::CameraOnTerrain(Vector3 pos, float z)
{
	Vector3 dir = camera->GetWorldPos() - pos;
	dir.Normalize();
	Ray ray = Ray(pos, dir);
	float zMin = FLT_MAX;
	Vector3 Hit;

	for (auto m : *SCENE->Map)
	{
		if (m->ComPutePicking(ray, Hit))
		{
			if (zMin > Vector3::Distance(Hit, ray.position))
				zMin = Vector3::Distance(Hit, ray.position) - 1.0f;
		}
	}
	if (zMin > z) zMin = z;
	camera->SetLocalPosZ(zMin);
}

bool GameObject::ChangeAnim(AnimationState state, UINT idx, float blendtime, bool force)
{
	//if ((!actor->anim->isChanging && actor->anim->currentAnimator.animIdx != idx) ||
	//	(actor->anim->isChanging && actor->anim->nextAnimator.animIdx != idx))
	if (actor->anim->PlayingIdx() != idx || !actor->anim->IsPlaying() || force)
	{
		actor->anim->PlayAnimation(state, idx, blendtime);
		for (int i = 0; i < soundBoard.size(); i++)
			soundBoard[i] = false;
		return true;
	}
	return false;
}

float GameObject::GetJump(float height, float t)
{
	return (height / t) + (SCENE->gravity * t) / 2.0f;
}

void GameObject::Rotate(Vector3 targetDir, float speedPerSec, float angleOffset)
{
	//Vector3 dir = -actor->GetForward();
	Vector3 dir = Vector3::Transform({0.0f,0.0f,-1.0f}, Matrix::CreateFromAxisAngle({ 0.0f,1.0f,0.0f }, actor->rotation.y));
	float curRot = atan2(-dir.x, -dir.z);
	float nextRot;
	float targetRot = atan2(-targetDir.x, -targetDir.z);
	if (angleOffset != 0.0f)
	{
		targetRot = Util::NormalizeAngle(targetRot + angleOffset);
	}

	if (fabs((targetRot + PI_2) - curRot) < fabs(targetRot - curRot))
	{
		curRot -= PI_2;
	}
	else if (fabs((targetRot - PI_2) - curRot) < fabs(targetRot - curRot))
	{
		curRot += PI_2;
	}
	nextRot = curRot;

	if (curRot > targetRot)
	{
		if (fabs(curRot - targetRot) > FLT_EPSILON)
		{
			nextRot -= speedPerSec * DELTA;
			if (nextRot < targetRot)
				nextRot = targetRot;
		}
	}
	else if (curRot < targetRot)
	{
		if (fabs(curRot - targetRot) > FLT_EPSILON)
		{
			nextRot += speedPerSec * DELTA;
			if (nextRot > targetRot)
				nextRot = targetRot;
		}
	}
	actor->rotation.y = Util::NormalizeAngle(nextRot);
}

bool GameObject::Attack(Collider* atkCol, GameType targetType)
{
	// 활성화된 모든 게임오브젝트의 HitCollider들을 검색
	bool result = false;
	for (auto it = GAMEOBJ->HitList.begin(); it != GAMEOBJ->HitList.end(); it++)
	{
		if (it->owner != this &&							// 히트콜라이더의 소유주가 본인이 아님
			(it->collider->gameType & (int)targetType))		// 히트콜라이더의 소유주속성이 지정한 것과 일치
		{
			bool check = true;
			for (int i = 0; i < atkCol->touched.size(); i++)
			{
				if (atkCol->touched[i] == it->owner->GetActor())
				{
					check = false;
					break;
				}
			}
			if (check &&							// 공격콜라이더가 공격모션중 한번이라도 해당 히트콜라이더를 공격한 적이 있음.
				atkCol->Intersect(it->collider))	// 충돌검사
			{
				string str = "AttackCollider";
				string n = atkCol->parent->name.substr(str.length(), 1);
				Object* ob;

				// 해당 공격콜라이더와 같은 앞번호를 지닌 공격콜라이더들에게
				// 접촉한 히트콜라이더의 소유주를 기록한다. (중복피격 방지)
				for (int i = 1; ob = actor->Find(str + n + "_" + to_string(i)); i++)
					ob->collider->touched.push_back(it->owner->actor);

				DmgNumber::DmgType dmg_type;
				float dmg_crit = it->collider->hit.crit;
				int dmg = static_cast<int>(ceilf((float)atkCol->atk.damage * dmg_crit));
				int knock = static_cast<int>(ceilf((float)atkCol->atk.knockdown * it->collider->hit.knock_crit));
				it->owner->DmgCor(dmg);	// 데미지 추가 보정

				// 한계선
				if (dmg > 999999) dmg = 999999;
				if (knock > 999999) knock = 999999;

				// 크리티컬에 따른 데미지타입 결정
				if (dmg_crit < 0)
					dmg_type = DmgNumber::DmgType::HEAL;
				else if (dmg_crit >= 2.0f)
					dmg_type = DmgNumber::DmgType::CRITICAL;
				else if (dmg_crit <= 0.25f)
					dmg_type = DmgNumber::DmgType::ENDURE;
				else
					dmg_type = DmgNumber::DmgType::NORMAL;

				// 히트콜라이더의 소유주에게 공격콜라이더의 속성을 넘긴다
				it->owner->damageInfo.push_back(
					DamageInfo(
						it->collider->parent->name,
						(atkCol->gameType & (int)GameType::COL_CENTER) ? atkCol->GetWorldPos() : actor->GetWorldPos(),
						it->collider->GetWorldPos(),
						dmg_type,
						dmg,
						knock,
						atkCol->atk.kb_velocity, 
						atkCol->atk.hitSound,
						atkCol->atk.guardSound
					)
				);

				if (GET_PLAYER == this && !(it->collider->gameType & (int)GameType::DONT_SHOW_DAMAGE))
					PLAYER_UI->ShowDamage(it->collider->GetWorldPos(), dmg, dmg_type);
				result = true;	// 타격에 성공했음을 반환.
			}
		}
	}
	return result;
}

void GameObject::Invincible(bool active)
{
	// 해당 Actor의 Hit콜라이더 수집
	int i = 1;
	while (true)
	{
		Object* ob;
		if (ob = actor->Find("HitCollider" + to_string(i)))
		{
			ob->collider->enable = !active;
		}
		else
			break;
		i++;
	}
}

void GameObject::Spawn(Object* point)
{
	if (!actor || !point)
		return;

	actor->SetWorldPos(point->GetWorldPos());
	actor->rotation.y = point->rotation.y + PI;
	Util::NormalizeAngle(actor->rotation.y);
}