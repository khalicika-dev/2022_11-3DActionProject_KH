#pragma once
class Texture
{
	ID3D11SamplerState*				Sampler;
private:
	struct ImgInfo
	{
		ID3D11ShaderResourceView* srv;
		string file;
		ImgInfo() :srv(nullptr), file("") {}
	};
	vector<ImgInfo> img;
public:
	D3D11_SAMPLER_DESC			    SamplerDesc;
	ScratchImage image;
	Texture();
	~Texture();
	static uint64_t CalculateFile(string file);
	void LoadFile(string file, bool b_AddCapacity);
	void LoadFile(string file, UINT idx, bool b_AddCapacity);
	UINT GetTextureCount() { return img.size(); }
	void SetTextureSRV(UINT idx, ID3D11ShaderResourceView* value) { img[idx].srv = value; }
	ID3D11ShaderResourceView* GetTextureSRV(UINT idx) { return img[idx].srv; }
	void AddTexture() { img.push_back(ImgInfo()); }
	void PopTexture() { SafeRelease(img[img.size() - 1].srv); img.pop_back(); }
	string GetTextureName(UINT idx) { return img[idx].file; }
	void RenderDetail(UINT idx);
	void CreateSampler();
	void Set(int slot);
};

