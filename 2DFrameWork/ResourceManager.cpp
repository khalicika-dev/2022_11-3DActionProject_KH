#include "framework.h"

void ResourceManager::ReleaseAll()
{
	meshes.ReleaseList();
	shaders.ReleaseList();
	textures.ReleaseList(); 
	materials.ReleaseList();
	animations.ReleaseList();
	calculatedXmlList.clear();
}
bool ResourceManager::IsItInXmlList(string file)
{
	// 리스트에 해당 원소가 있고 갯수가 0보다 크면 true
	auto it = calculatedXmlList.find(file);
	return it != calculatedXmlList.end() && it->second > 0;
}
void ResourceManager::AddCurrentCapacity(bool cond, uint64_t value)
{
	if (not cond)
	{
		return;
	}
	App.currentLoadingCapacity += value;
}
void ResourceManager::AddTotalCapacity(uint64_t value)
{
	App.totalLoadingCapacity += value;
}
void ResourceManager::ResetAllCapacity()
{
	App.currentLoadingCapacity = 0;
	App.totalLoadingCapacity = 0;
}

void ResourceManager::IncreaseXmlList_AddTotalCapacity(string file)
{
	auto it = calculatedXmlList.find(file);
	if (it == calculatedXmlList.end())
		calculatedXmlList[file] = 0;
	calculatedXmlList[file]++;
	AddTotalCapacity(Actor::CalculateFile(file));
}

void ResourceManager::DecreaseXmlList(string file)
{
	auto it = calculatedXmlList.find(file);
	if (it == calculatedXmlList.end() || calculatedXmlList[file] <= 0)
		return;
	calculatedXmlList[file]--;
}

//template<typename T>
// const shared_ptr<T>& Resource<T>::Load(string file)
