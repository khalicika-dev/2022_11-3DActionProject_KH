#pragma once
class CubeRenderTarget
{
protected:
    int width, height;

    ID3D11RenderTargetView* rtv;
    ID3D11DepthStencilView* dsv;


    //쉐이더자원으로 쓸때 인터페이스
    ID3D11ShaderResourceView* rtvSrv;

    //자원
    ID3D11Texture2D* rtvTexture;
    ID3D11Texture2D* dsvTexture;
    //창사이즈 조절
    void CreateBackBuffer(float width, float height);
    void DeleteBackBuffer();
public:

    CubeRenderTarget(int width = 256, int height = 256);
    ~CubeRenderTarget();

    void Set(Color clear = Color(0, 0, 0, 1));

    ID3D11ShaderResourceView* GetSRV() { return rtvSrv; }

    //창사이즈 조절
    virtual void ResizeScreen(float width, float height);
};

