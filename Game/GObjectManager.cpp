#include "stdafx.h"
GObjectManager::GObjectManager()
{
	eventTarget = nullptr;
}

GObjectManager::~GObjectManager()
{
	// 사용된 오브젝트들 제거
	for (auto& obj : obj_pool)
		delete obj.second;
	obj_pool.clear();
}

void GObjectManager::InitPool()
{
	for (int i = 0; i < 50; i++) Generate(ParticleType::Effect_Hit, { 0.0f,0.0f,0.0f }, true);
	for (int i = 0; i < 50; i++) Generate(ParticleType::Bullet_Red, { 0.0f,0.0f,0.0f }, true);
}

void GObjectManager::CalculatePool()
{
	for (int i = 0; i < 50; i++) Effect_Hit::Calculate();
	for (int i = 0; i < 50; i++) Bullet_Red::Calculate();
}

void GObjectManager::Update(GOType type, UINT num)
{
	GameObject* obj = Find(type, num);
	if (!obj || !obj->Enable) return;

	obj->Update();

	// 해당 Actor의 Hit콜라이더 수집
	int i = 1;
	while (true)
	{
		Object* ob;
		if (ob = obj->GetActor()->Find("HitCollider" + to_string(i)))
		{
			if (ob->collider->enable)
				HitList.push_back(HitInfo(ob->collider, obj));
		}
		else
			break;
		i++;
	}
}

void GObjectManager::LateUpdate(GOType type, UINT num)
{
	GameObject* obj = Find(type, num);
	if (!obj || !obj->Enable) return;
	obj->LateUpdate();
}

void GObjectManager::Render(GOType type, UINT num)
{
	GameObject* obj = Find(type, num);
	if (!obj || !obj->Enable) return;
	obj->Render();
}

void GObjectManager::CubeMapRender(GOType type, UINT num)
{
	GameObject* obj = Find(type, num);
	if (!obj || !obj->Enable) return;
	obj->CubeMapRender();
}

void GObjectManager::ShadowMapRender(GOType type, UINT num)
{
	GameObject* obj = Find(type, num);
	if (!obj || !obj->Enable) return;
	obj->ShadowMapRender();
}

void GObjectManager::Prepare(GOType type, UINT num)
{
	GameObject* obj = nullptr;
	string name;
	name = to_string(static_cast<int>(type)) + to_string(num);
	if (obj_pool.count(name) != 0)
	{
		obj_pool[name]->Enable = true;
		obj_pool[name]->Init();
		return;
	}
	switch (type)
	{
	case GOType::PLAYER: obj = new Player(); break;
	case GOType::MONSTER_GOBLIN: obj = new Monster_Goblin(); break;
	case GOType::MONSTER_DRONE: obj = new Monster_Drone(); break;
	case GOType::MONSTER_GIANT: obj = new Monster_Giant(); break;
	default: assert(false);
	}
	obj_pool.emplace(name, obj);
}

void GObjectManager::Calculate(GOType type, UINT num)
{
	string name;
	name = to_string(static_cast<int>(type)) + to_string(num);
	if (obj_pool.count(name) != 0)
		return;
	switch (type)
	{
	case GOType::PLAYER: Player::Calculate(); break;
	case GOType::MONSTER_GOBLIN: Monster_Goblin::Calculate(); break;
	case GOType::MONSTER_DRONE: Monster_Drone::Calculate(); break;
	case GOType::MONSTER_GIANT: Monster_Giant::Calculate(); break;
	default: assert(false);
	}
}

GameObject* GObjectManager::Find(GOType type, UINT num)
{
	GameObject* obj = nullptr;
	string name = Type2Name(type, num);
	if (obj_pool.count(name))
		obj = obj_pool[name];
	return obj;
}

Monster* GObjectManager::Find_Mob(GOType type, UINT num)
{
	return dynamic_cast<Monster*>(Find(type, num));
}

Player* GObjectManager::FindPlayer()
{
	return dynamic_cast<Player*>(Find(GOType::PLAYER, 0));
}

void GObjectManager::Particles_Update()
{
	for (auto& item : m_particles)
	{
		if (item.second->Enable)
		{
			item.second->Update();

			// 해당 Actor의 Hit콜라이더 수집
			int i = 1;
			while (true)
			{
				Object* ob;
				if (ob = item.second->GetActor()->Find("HitCollider" + to_string(i)))
				{
					if (ob->collider->enable)
						HitList.push_back(HitInfo(ob->collider, item.second.get()));
				}
				else
					break;
				i++;
			}
		}
	}
}

void GObjectManager::Particles_LateUpdate()
{
	for (auto& item : m_particles)
	{
		if (item.second->Enable)
			item.second->LateUpdate();
	}
}

void GObjectManager::Particles_Render()
{
	for (auto& item : m_particles)
	{
		if (item.second->Enable)
			item.second->Render();
	}
}

void GObjectManager::All_Particle_ActiveOff()
{
	for (auto& item : m_particles)
		item.second->Enable = false;
}

void GObjectManager::Trails_Render()
{
	BLEND->Set(true);
	RASTER->Set(D3D11_CULL_NONE);
	while (!q_trails.empty())
	{
		q_trails.front()->actor->Render();
		q_trails.pop();
	}
	RASTER->Set(D3D11_CULL_BACK);
	BLEND->Set(false);
}

shared_ptr<GameObject> GObjectManager::Generate(ParticleType type, Vector3 pos, bool isStore)
{
	shared_ptr<GameObject> temp;

	// 기존의 객체를 찾아서 재활용한다
	if (!m_particles.empty() && !isStore)
	{
		auto range = m_particles.equal_range(type);
		for (auto iter = range.first; iter != range.second; iter++)
		{
			if (!iter->second->Enable)
			{
				iter->second->Enable = true;
				iter->second->SetWorldPos(pos);
				iter->second->Init();
				return iter->second;
			}
		}
	}

	// 기존의 객체가 없으면 새로 만든다
	// isStore면 존재유무에 관계없이 새로 만든다. 주로 오브젝트 풀을 미리 담을때 사용
	switch (type)
	{
	case ParticleType::Effect_Hit:	temp = make_shared<Effect_Hit>(); break;
	case ParticleType::Bullet_Red:	temp = make_shared<Bullet_Red>(); break;
	default: return nullptr;
	}
	temp->SetWorldPos(pos);

	temp->Enable = !isStore;
	m_particles.emplace(type, temp);
	return temp;
}