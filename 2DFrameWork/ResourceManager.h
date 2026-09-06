#pragma once

template<typename T>
class Resource
{
private:
	unordered_map<string, shared_ptr<T>> loadedList;
	set<string> calculatedList;
public:
	const shared_ptr<T>& Load(string file)
	{
		auto it = loadedList.find(file);
		if (it != loadedList.end())
		{
			return it->second;
		}
		shared_ptr<T> temp = make_shared<T>();
		auto it2 = calculatedList.find(file);
		bool b_AddCapacity = it2 != calculatedList.end();
		if (!App.GetAppQuit())
			temp->LoadFile(file, b_AddCapacity);	// 계산리스트에 이미 포함되어있으면 로딩된 용량을 표시한다.
		loadedList[file] = temp;
		calculatedList.insert(file);
		return loadedList[file];
	}

	const uint64_t Calculate(string file)
	{
		auto it = loadedList.find(file);
		auto it2 = calculatedList.find(file);
		if (it != loadedList.end() || it2 != calculatedList.end())
		{
			return 0;
		}
		calculatedList.insert(file);
		return T::CalculateFile(file);
	}

	void ReleaseList()
	{
		for (auto it = loadedList.begin(); it != loadedList.end(); it++)
		{
			SafeReset(it->second);
		}
		loadedList.clear();
		calculatedList.clear();
	}
};


class ResourceManager : public Singleton<ResourceManager>
{
private:
	map<string, int>		calculatedXmlList;	// 용량을 계산한 Xml리스트, 용량계산 시 카운트가 증가하고 로드하면 감소한다.
public:
	Resource<Mesh>			meshes;
	Resource<Shader>		shaders;
	Resource<Texture>		textures;
	Resource<Material>		materials;
	Resource<Animation>		animations;
	//---------------------------------------------------------------------------------
	void ReleaseAll();
	bool IsItInXmlList(string file);
	void AddCurrentCapacity(bool cond, uint64_t value);
	void AddTotalCapacity(uint64_t value);
	void ResetAllCapacity();
	uint64_t GetCurrentCapacity() { return App.currentLoadingCapacity; }
	uint64_t GetTotalCapacity() { return App.totalLoadingCapacity; }
	void IncreaseXmlList_AddTotalCapacity(string file);		// 계산한 직후 총량에 집계하고 계산한 Xml리스트 카운트를 증가시킨다.
	void DecreaseXmlList(string file);
};
