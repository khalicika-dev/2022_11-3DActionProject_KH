#include "stdafx.h"

map<ItemManager::ItemType, ItemManager::Item> ItemManager::itemInfo;
void ItemManager::CreateStaticMember()
{
	itemInfo[ItemType::RED].image = RESOURCE->textures.Load("UI/item_potionRed.png");
	itemInfo[ItemType::RED].text = RESOURCE->textures.Load("UI/text_redPotion.png");
	itemInfo[ItemType::RED].color = { 1, 0.15f, 0.15f, 1 };
	itemInfo[ItemType::RED].effect = []()
	{
		if (!GET_PLAYER)
			return;
		GET_PLAYER->hp += 50.0f;
		if(GET_PLAYER->hp > GET_PLAYER->hp_max)
			GET_PLAYER->hp = GET_PLAYER->hp_max;
	};

	itemInfo[ItemType::PURPLE].image = RESOURCE->textures.Load("UI/item_potionPurple.png");
	itemInfo[ItemType::PURPLE].text = RESOURCE->textures.Load("UI/text_purplePotion.png");
	itemInfo[ItemType::PURPLE].color = { 0.8f, 0, 0.8f, 1 };
	itemInfo[ItemType::PURPLE].effect = []()
	{
		if (!GET_PLAYER)
			return;
		GET_PLAYER->hp = GET_PLAYER->hp_max;
		GET_PLAYER->st = GET_PLAYER->st_max;
	};

	itemInfo[ItemType::DUMMY1].image = RESOURCE->textures.Load("UI/item_ItemDummy1.png");
	itemInfo[ItemType::DUMMY1].text = RESOURCE->textures.Load("UI/text_ItemDummy1.png");
	itemInfo[ItemType::DUMMY1].color = { 0.15f, 0.75f, 0.75f, 1 };
	itemInfo[ItemType::DUMMY1].effect = []()
	{
		if (!GET_PLAYER)
			return;
	};

	itemInfo[ItemType::DUMMY2].image = RESOURCE->textures.Load("UI/item_ItemDummy2.png");
	itemInfo[ItemType::DUMMY2].text = RESOURCE->textures.Load("UI/text_ItemDummy2.png");
	itemInfo[ItemType::DUMMY2].color = { 0.5f, 1, 1, 1 };
	itemInfo[ItemType::DUMMY2].effect = []()
	{
		if (!GET_PLAYER)
			return;
	};
}

void ItemManager::DeleteStaticMember()
{
	for (auto it = itemInfo.begin(); it != itemInfo.end(); it++)
	{
		it->second.image = nullptr;
		it->second.text = nullptr;
	}
}

void ItemManager::CalculateStaticMember()
{
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/item_potionRed.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/text_redPotion.png"));

	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/item_potionPurple.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/text_purplePotion.png"));

	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/item_ItemDummy1.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/text_ItemDummy1.png"));

	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/item_ItemDummy2.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/text_ItemDummy2.png"));
}
