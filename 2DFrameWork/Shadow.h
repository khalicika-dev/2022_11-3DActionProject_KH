#pragma once
struct ShadowDesc
{
	int		ShadowQuality = 0;
	float	ShadowBias = 0.00325f;
	Vector2 Size = Vector2(0.1f, 0.1f);
};

class Shadow : public RenderTarget
{
	ShadowDesc			desc;
	static ID3D11Buffer* shadowPSBuffer;
	static ID3D11Buffer* shadowVSBuffer;
	Camera* cam;
	float				range;
	float				textureSize;
public:
	static void CreateStaticMember();
	static void DeleteStaticMember();

	Shadow(float textureSize = 1000.0f, float range = 500.0f, float ShadowBias = -0.001f, bool ShadowQuality = false);
	~Shadow();

	void SetCapture(Vector3 position);
	void SetTexture();
	void ResizeScreen(float textureSize);
	void RenderDetail();
	void SetProperty(float textureSize, float range, bool ShadowQuality);
};

