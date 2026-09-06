#include "Framework.h"

Texture::Texture()
{
    Sampler = nullptr;
    img.push_back(ImgInfo());
    //기본 샘플러 값
    //SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.MipLODBias = 0.0f;
    SamplerDesc.MaxAnisotropy = 1;
    SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    SamplerDesc.MinLOD = -FLT_MAX;
    SamplerDesc.MaxLOD = FLT_MAX;
    CreateSampler();
}

Texture::~Texture()
{
    for (int i = 0; i < img.size(); i++)
        SafeRelease(img[i].srv);
    img.clear();
    SafeRelease(Sampler);
}

uint64_t Texture::CalculateFile(string file)
{
    return Util::CalculateFile("Contents/Texture/" + file);
}

void Texture::RenderDetail(UINT idx)
{
    ImGui::Text(img[idx].file.c_str());
    ImVec2 size(400, 400);
    ImGui::Image((void*)img[idx].srv, size);

    ImGui::PushID(this);
    if (ImGui::Button("SamplerButton"))
    {
        ImGui::OpenPopup("Sampler");
    }
    if (ImGui::BeginPopup("Sampler"))
    {
        if (ImGui::Button("Filter POINT"))
        {
            SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            CreateSampler();
        }
        if (ImGui::Button("Filter LINEAR"))
        {
            SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            CreateSampler();
        }
        if (ImGui::Button("AddressU Clamp"))
        {
            SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            CreateSampler();
        }
        if (ImGui::Button("AddressU Wrap"))
        {
            SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            CreateSampler();
        }
        if (ImGui::Button("AddressU Mirror"))
        {
            SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
            CreateSampler();
        }
        if (ImGui::Button("AddressV Clamp"))
        {
            SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            CreateSampler();
        }
        if (ImGui::Button("AddressV Wrap"))
        {
            SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            CreateSampler();
        }
        if (ImGui::Button("AddressV Mirror"))
        {
            SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
            CreateSampler();
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

void Texture::LoadFile(string file, bool b_AddCapacity)
{
    LoadFile(file, 0, b_AddCapacity);
}

void Texture::LoadFile(string file, UINT idx, bool b_AddCapacity)
{
    this->img[idx].file = file;
    size_t index = file.find_last_of('.');
    //확장자 문자열 자르기
    string format = file.substr(index + 1, file.length());
    wstring path = L"Contents/Texture/" + Util::ToWString(file);
    //wstring path = L"D:/Song/DX/Contents/Texture/" + Util::ToWString(file);

    HRESULT hr;
    if (format == "tga")
        hr = LoadFromTGAFile(path.c_str(), nullptr, image);
    else if (format == "dds")
        hr = LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, image);
    else
        hr = LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, nullptr, image);
    Check(hr);

    CreateShaderResourceView(D3D->GetDevice(), image.GetImages(), image.GetImageCount(),
        image.GetMetadata(), &img[idx].srv);

    RESOURCE->AddCurrentCapacity(b_AddCapacity, CalculateFile(file));
}

void Texture::CreateSampler()
{
    SafeRelease(Sampler);
    HRESULT hr;
    hr = D3D->GetDevice()->CreateSamplerState(&SamplerDesc, &Sampler);
    Check(hr);
}

void Texture::Set(int slot)
{
    int tSlot = 0;
    switch (slot)
    {
    case 0: tSlot = 0; break;
    case 1: tSlot = 10; break;
    case 2: tSlot = 20; break;
    case 3: tSlot = 30; break;
    case 4: tSlot = 40; break;
    case 5: tSlot = 41; break;
    case 6: tSlot = 42; break;
    //case 7: tSlot = 43; break;
    }
    vector<ID3D11ShaderResourceView*> srvtemp;
    for (int i = 0; i < img.size(); i++)
        srvtemp.push_back(img[i].srv);

    //D3D->GetDC()->PSSetShaderResources(slot,// 연결될레지스터 번호 0
    //    1,//리소스갯수
    //    &img[idx].srv);
    D3D->GetDC()->PSSetShaderResources(tSlot,// 연결될레지스터 번호 0
        srvtemp.size(),//리소스갯수
        &srvtemp[0]);
    D3D->GetDC()->PSSetSamplers(slot, 1, &Sampler);
    srvtemp.clear();
}
