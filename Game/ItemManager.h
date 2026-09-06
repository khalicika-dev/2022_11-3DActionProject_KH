#pragma once
class ItemManager
{
public:
	static void CreateStaticMember();
	static void DeleteStaticMember();
	static void CalculateStaticMember();

	enum class ItemType
	{
		RED,
		PURPLE,
		DUMMY1,
		DUMMY2
	};
	struct Item
	{
		shared_ptr<Texture> image;
		shared_ptr<Texture> text;
		Color color;
		function<void()> effect;	// 포션효과
	};
	static map<ItemType, Item> itemInfo;
};

