#pragma once
class MaterialBuffer
{
public:
	Color	ambient;
	Color	diffuse;
	Color	specular;
	Color	emissive;
	float	shininess;
	float	opacity;
	float   environment;
	float	shadow;
};
class Material : public MaterialBuffer
{
	static ID3D11Buffer* materialBuffer;
public:
	static void CreateStaticMember();
	static void DeleteStaticMember();
	static uint64_t CalculateFile(string file);
public:
	shared_ptr<Texture>			normalMap;
	shared_ptr<Texture>			diffuseMap;
	shared_ptr<Texture>			specularMap;
	shared_ptr<Texture>			emissiveMap;
	string						file;
public:
	Material();
	~Material();

	void RenderDetail();
	virtual void Set();
	void LoadFile(string file, bool b_AddCapacity);
	void SaveFile(string file);
private:
	UINT nNormal;
	UINT nDiffuse;
	UINT nSpecular;
	UINT nEmissive;
	void RenderNum(string type, UINT& nTex, shared_ptr<Texture> target);
};

