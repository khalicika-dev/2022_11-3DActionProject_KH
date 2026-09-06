#include "framework.h"
ID3D11Buffer* Shadow::shadowPSBuffer = nullptr;
ID3D11Buffer* Shadow::shadowVSBuffer = nullptr;
void Shadow::CreateStaticMember()
{
    {
        D3D11_BUFFER_DESC desc = { 0 };
        desc.ByteWidth = sizeof(ShadowDesc);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//상수버퍼
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;
        HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &shadowPSBuffer);

        assert(SUCCEEDED(hr));
    }
    {
        D3D11_BUFFER_DESC desc = { 0 };
        desc.ByteWidth = sizeof(Matrix);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//상수버퍼
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;
        HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &shadowVSBuffer);
        assert(SUCCEEDED(hr));

    }
}

void Shadow::DeleteStaticMember()
{
    SafeRelease(shadowVSBuffer);
    SafeRelease(shadowPSBuffer);
}

Shadow::Shadow(float textureSize, float range, float ShadowBias, bool ShadowQuality)
    : textureSize(textureSize), range(range), RenderTarget(500, 500)
{
    cam = Camera::Create();
    cam->viewport.width = textureSize;
    cam->viewport.height = textureSize;
    desc.Size.x = textureSize;
    desc.Size.y = textureSize;
    desc.ShadowBias = ShadowBias;
    desc.ShadowQuality = ShadowQuality ? 1 : 0;
    ResizeScreen(textureSize);
}

Shadow::~Shadow()
{
    SafeRelease(cam);
}

void Shadow::SetCapture(Vector3 position)
{
    Vector3 up = Vector3(0, 1, 0);
    Vector3 direction(LIGHT->dirLight.direction.x, LIGHT->dirLight.direction.y, LIGHT->dirLight.direction.z);
    direction.Normalize();
    Vector3 pos = direction * -range;
    pos += position;
    cam->view = Matrix::CreateLookAt(pos, position, up);
    Vector3 origin = Vector3::Transform(position, cam->view);
    float n = origin.z - range * 0.5f;
    float f = origin.z + range * 0.5f;

    cam->proj = Matrix::CreateOrthographic(range, range, n, f);
    cam->viewport.width = textureSize;
    cam->viewport.height = textureSize;
    cam->SetShadow();
    Set(Color(1, 1, 1, 1));
}

void Shadow::SetTexture()
{
    D3D->GetDC()->PSSetShaderResources(43, 1, &rtvSrv);
    {
        Matrix TVP = cam->view * cam->proj;
        TVP = TVP.Transpose();

        D3D11_MAPPED_SUBRESOURCE mappedResource3;
        D3D->GetDC()->Map(shadowVSBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource3);
        memcpy_s(mappedResource3.pData, sizeof(Matrix), &TVP, sizeof(Matrix));
        D3D->GetDC()->Unmap(shadowVSBuffer, 0);

        D3D->GetDC()->VSSetConstantBuffers(5, 1, &shadowVSBuffer);
    }


    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        D3D->GetDC()->Map(shadowPSBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        memcpy_s(mappedResource.pData, sizeof(ShadowDesc), &desc, sizeof(ShadowDesc));
        D3D->GetDC()->Unmap(shadowPSBuffer, 0);
        D3D->GetDC()->PSSetConstantBuffers(5, 1, &shadowPSBuffer);
    }
}

void Shadow::SetProperty(float textureSize, float range, bool ShadowQuality)
{
    desc.Size.x = textureSize;
    desc.Size.y = textureSize;
    desc.ShadowQuality = ShadowQuality ? 1 : 0;
    ResizeScreen(textureSize);
}

void Shadow::ResizeScreen(float textureSize)
{
    RenderTarget::ResizeScreen(textureSize, textureSize);
    this->textureSize = textureSize;
    desc.Size.x = textureSize;
    desc.Size.y = textureSize;
}
void Shadow::RenderDetail()
{
    ImGui::SliderFloat("Range", &range, 1, 1000);
    if (ImGui::SliderFloat("textureSize", &textureSize, 1, 10000))
    {
        ResizeScreen(textureSize);
    }
    ImGui::SliderFloat("ShadowBias", &desc.ShadowBias, -0.1f, 0.1f);
    ImGui::SliderInt("Quality", &desc.ShadowQuality, 0, 1);

    ImVec2 size(400, 400);
    ImGui::Image((void*)rtvSrv, size);
}
