#include "stdafx.h"

Bullet_Red::Bullet_Red()
{
	actor = Actor::Create();
	actor->LoadFile("RedBullet.xml");
	if (App.GetAppQuit())
	{
		return;
	}
	col = actor->Find("AttackCollider1_1")->collider;
	Init();
	RESOURCE->DecreaseXmlList("RedBullet.xml");
}

Bullet_Red::~Bullet_Red()
{
	actor->Release();
}

void Bullet_Red::Calculate()
{
	RESOURCE->IncreaseXmlList_AddTotalCapacity("RedBullet.xml");
}

void Bullet_Red::Update()
{
	actor->Update();
}

void Bullet_Red::LateUpdate()
{
	Vector3 pos = actor->GetWorldPos();

	if (Attack(col, GameType::HIT_PLAYER))
	{
		{
			Vector3 spawnPos = pos;
			Pop* temp = dynamic_cast<Pop*>(GAMEOBJ->Generate(PTCTYPE::Effect_Hit, spawnPos)->GetActor());
			temp->scale = { 0.3f,0.3f,0.3f };
			temp->material->diffuse = Color(0.5f, 0.0f, 0.0f, 1.0f);
			temp->duration = 0.75f;
			temp->desc.gravity = 0.0f;
			temp->velocityScalar = 10.0f;
			temp->particleCount = 20;
			temp->Play();
		}
		Enable = false;
	}
	else
	{
		bool check = false;
		// 몬스터와 충돌 시
		for (auto it = GAMEOBJ->HitList.begin(); it != GAMEOBJ->HitList.end(); it++)
		{
			if (col->Intersect(it->collider) &&
				it->owner != owner &&							// 탄환의 소유주가 본인이 아님
				(it->collider->gameType & (int)GameType::HIT_MONSTER) &&	// 히트콜라이더의 속성이 지정한 것과 일치
				!(it->collider->gameType & (int)GameType::IGNORE_BULLETS))	// 해당 히트콜라이더가 탄환을 무시하지 않음
			{
				check = true;
				break;
			}
		}

		// 데미지를 받아 파괴될 시
		if (damageInfo.size() > 0)
		{
			damageInfo.clear();
			check = true;
		}

		Vector3 hit;
		int idx;
		Vector3 dir = velocity;
		dir.Normalize();
		Ray ray = Ray(pos, dir);
		float dist = FLT_MAX;

		if (firstUpdate)
			firstUpdate = false;
		else
		{
			pos += velocity;
			range -= velocity.Length();
		}

		idx = Util::GetMyStandMap(pos);
		// 지형과 충돌시
		if (Util::RayIntersectMap(ray,(*SCENE->Map)[idx],hit))
			dist = Vector3::Distance(pos, hit);

		// TERRAIN_COL옵션이 달린 맵콜라이더와 충돌시
		{
			int col_Idx = 0;
			Object* ob;
			while (true)
			{
				col_Idx++;
				if (ob = (*SCENE->Map)[idx]->Find("Col" + to_string(col_Idx)))
				{
					if ((ob->collider->gameType & (int)GameType::TERRAIN_COL) && ob->collider->Intersect(col))
					{
						check = true;
						break;
					}
				}
				else
					break;
			}
		}

		if (check || range <= 0.0f || dist <= actor->scale.x)
		{
			{
				Vector3 spawnPos = pos;
				Pop* temp = dynamic_cast<Pop*>(GAMEOBJ->Generate(PTCTYPE::Effect_Hit, spawnPos)->GetActor());
				temp->scale = { 0.25f,0.25f,0.25f };
				temp->material->diffuse = Color(0.5f, 0.0f, 0.0f, 1.0f);
				temp->duration = 0.5f;
				temp->desc.gravity = 0.0f;
				temp->velocityScalar = 10.0f;
				temp->particleCount = 20;
				temp->Play();
			}
			SOUND->Play(SDKEY::hit_shot00, SDTYPE::SE, 1.0f, pos);
			Enable = false;
		}
	}
	actor->SetWorldPos(pos);
}

void Bullet_Red::Render()
{
	actor->Render();
}

void Bullet_Red::CubeMapRender()
{
	actor->CubeMapRender();
}

void Bullet_Red::ShadowMapRender()
{
	actor->ShadowMapRender();
}

void Bullet_Red::Init()
{
	owner = nullptr;
	actor->rotation = { 0.0f, 0.0f, 0.0f };
	actor->scale = { 1.0f,1.0f,1.0f };
	col->atk.damage = 0;
	col->atk.kb_velocity = { 0,0 };
	col->atk.knockdown = 0;
	col->atk.hitSound = SDKEY::NONE;
	col->atk.guardSound = SDKEY::NONE;
	col->touched.clear();
	range = 0.0f;
	velocity = { 0.0f,0.0f,0.0f };
	firstUpdate = true;
}
