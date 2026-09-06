#include "Framework.h"

ID3D11Buffer* Material::materialBuffer = nullptr;

Material::Material()
{
    file = "defalut.mtl";
    ambient = Color(0.3f, 0.3f, 0.3f, 0.0f);
    diffuse = Color(1.0f, 1.0f, 1.0f, 0.0f);
    specular = Color(1.0f, 1.0f, 1.0f, 0.0f);
    emissive = Color(0.2f, 0.2f, 0.2f, 0.0f);
    shininess = 30.0f;
    opacity = 1.0f;
    environment = 0.0f;
    shadow = 0.0f;
    normalMap = nullptr;
    diffuseMap = nullptr;
    emissiveMap = nullptr;
    specularMap = nullptr;
    nNormal = nDiffuse = nSpecular = nEmissive = 0;
}


Material::~Material()
{
    SafeReset(normalMap);
    SafeReset(diffuseMap);
    SafeReset(specularMap);
    SafeReset(emissiveMap);
}

void Material::CreateStaticMember()
{
    D3D11_BUFFER_DESC desc = { 0 };
    desc.ByteWidth = sizeof(MaterialBuffer);
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//상수버퍼
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &materialBuffer);
    assert(SUCCEEDED(hr));
    D3D->GetDC()->PSSetConstantBuffers(1, 1, &materialBuffer);
}

void Material::DeleteStaticMember()
{
    SafeRelease(materialBuffer);
}

uint64_t Material::CalculateFile(string file)
{
    return Util::CalculateFile("Contents/Material/" + file);
}

void Material::Set()
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    D3D->GetDC()->Map(materialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy_s(mappedResource.pData, sizeof(MaterialBuffer), (MaterialBuffer*)this, sizeof(MaterialBuffer));
    D3D->GetDC()->Unmap(materialBuffer, 0);

  
    if (normalMap)normalMap->Set(0);
    if (diffuseMap)diffuseMap->Set(1);
    if (specularMap)specularMap->Set(2);
    if (emissiveMap)emissiveMap->Set(3);
}

void Material::RenderDetail()
{
    ImGui::ColorEdit3("ambient", (float*)&ambient, ImGuiColorEditFlags_PickerHueWheel);
    ImGui::ColorEdit3("diffuse", (float*)&diffuse, ImGuiColorEditFlags_PickerHueWheel);
    ImGui::ColorEdit3("specular", (float*)&specular, ImGuiColorEditFlags_PickerHueWheel);
    ImGui::ColorEdit3("emissive", (float*)&emissive, ImGuiColorEditFlags_PickerHueWheel);
    ImGui::DragFloat("shiness", &shininess, 0.05f);
    ImGui::DragFloat("opacity", &opacity, 0.05f);
    ImGui::DragFloat("environment", &environment, 0.05f, 0.0f, 1.0f);
    ImGui::DragFloat("shadow", &shadow, 0.05f, 0.0f, 1.0f);
    ImGui::Separator();

    if (normalMap)
        normalMap->RenderDetail(nNormal);

    if (GUI->FileImGui("Load NormalMap", "Load NormalMap",
        ".dds,.jpg,.tga,.png,.bmp", "Contents/Texture"))
    {
        string path = ImGuiFileDialog::Instance()->GetCurrentPath();
        Util::Replace(&path, "\\", "/");
        if (path.find("/Texture/") != -1)
        {
            size_t tok = path.find("/Texture/") + 9;
            path = path.substr(tok, path.length())
                + "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        else
        {
            path = ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        //SafeReset(normalMap);
        if (normalMap)
            normalMap->LoadFile(path, nNormal, false);
        else
            normalMap = RESOURCE->textures.Load(path);
        ambient.w = 1.0f;
    }
    RenderNum("normal", nNormal, normalMap);
    ImGui::Separator();

    if (diffuseMap)
        diffuseMap->RenderDetail(nDiffuse);

    if (GUI->FileImGui("Load DiffuseMap", "Load DiffuseMap",
        ".dds,.jpg,.tga,.png,.bmp", "Contents/Texture"))
    {
        string path = ImGuiFileDialog::Instance()->GetCurrentPath();
        Util::Replace(&path, "\\", "/");
        if (path.find("/Texture/") != -1)
        {
            size_t tok = path.find("/Texture/") + 9;
            path = path.substr(tok, path.length())
                + "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        else
        {
            path = ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        //SafeReset(diffuseMap);
        if (diffuseMap)
            diffuseMap->LoadFile(path, nDiffuse, false);
        else
            diffuseMap = RESOURCE->textures.Load(path);
        diffuse.w = 1.0f;
    }
    RenderNum("diffuse", nDiffuse, diffuseMap);
    ImGui::Separator();

    if (specularMap)
        specularMap->RenderDetail(nSpecular);

    if (GUI->FileImGui("Load SpecularMap", "Load SpecularMap",
        ".dds,.jpg,.tga,.png,.bmp", "Contents/Texture"))
    {
        string path = ImGuiFileDialog::Instance()->GetCurrentPath();
        Util::Replace(&path, "\\", "/");
        if (path.find("/Texture/") != -1)
        {
            size_t tok = path.find("/Texture/") + 9;
            path = path.substr(tok, path.length())
                + "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        else
        {
            path = ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        //SafeReset(specularMap);
        if (specularMap)
            specularMap->LoadFile(path, nSpecular, false);
        else
            specularMap = RESOURCE->textures.Load(path);
        specular.w = 1.0f;
    }
    RenderNum("specular", nSpecular, specularMap);
    ImGui::Separator();

    if (emissiveMap)
        emissiveMap->RenderDetail(nEmissive);

    if (GUI->FileImGui("Load emissiveMap", "Load emissiveMap",
        ".dds,.jpg,.tga,.png,.bmp", "Contents/Texture"))
    {
        string path = ImGuiFileDialog::Instance()->GetCurrentPath();
        Util::Replace(&path, "\\", "/");
        if (path.find("/Texture/") != -1)
        {
            size_t tok = path.find("/Texture/") + 9;
            path = path.substr(tok, path.length())
                + "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        else
        {
            path = ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        //SafeReset(emissiveMap);
        if (emissiveMap)
            emissiveMap->LoadFile(path, nEmissive, false);
        else
            emissiveMap = RESOURCE->textures.Load(path);
        emissive.w = 1.0f;
    }
    RenderNum("emissive", nEmissive, emissiveMap);
}

void Material::LoadFile(string file, bool b_AddCapacity)
{
    this->file = file;
    BinaryReader in;
    wstring path = L"Contents/Material/" + Util::ToWString(file);
    in.Open(path);

    ambient = in.color4f(b_AddCapacity);
    diffuse = in.color4f(b_AddCapacity);
    specular = in.color4f(b_AddCapacity);
    emissive = in.color4f(b_AddCapacity);

    int cnt;
    cnt = in.Int(b_AddCapacity);
    if (cnt > 0)
    {
        normalMap = RESOURCE->textures.Load(in.String(b_AddCapacity));
        for (int i = 1; i < cnt; i++)
        {
            normalMap->AddTexture();
            normalMap->LoadFile(in.String(b_AddCapacity), i, false);
        }
        normalMap->SamplerDesc.Filter = (D3D11_FILTER)in.Int(b_AddCapacity);
        normalMap->SamplerDesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)in.Int(b_AddCapacity);
        normalMap->SamplerDesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)in.Int(b_AddCapacity);
        normalMap->SamplerDesc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)in.Int(b_AddCapacity);
        normalMap->CreateSampler();
    }
    cnt = in.Int(b_AddCapacity);
    if (cnt > 0)
    {
        diffuseMap = RESOURCE->textures.Load(in.String(b_AddCapacity));
        for (int i = 1; i < cnt; i++)
        {
            diffuseMap->AddTexture();
            diffuseMap->LoadFile(in.String(b_AddCapacity), i, false);
        }
        diffuseMap->SamplerDesc.Filter = (D3D11_FILTER)in.Int(b_AddCapacity);
        diffuseMap->SamplerDesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)in.Int(b_AddCapacity);
        diffuseMap->SamplerDesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)in.Int(b_AddCapacity);
        diffuseMap->SamplerDesc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)in.Int(b_AddCapacity);
        diffuseMap->CreateSampler();
    }
    cnt = in.Int(b_AddCapacity);
    if (cnt > 0)
    {
        specularMap = RESOURCE->textures.Load(in.String(b_AddCapacity));
        for (int i = 1; i < cnt; i++)
        {
            specularMap->AddTexture();
            specularMap->LoadFile(in.String(b_AddCapacity), i, false);
        }
        specularMap->SamplerDesc.Filter = (D3D11_FILTER)in.Int(b_AddCapacity);
        specularMap->SamplerDesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)in.Int(b_AddCapacity);
        specularMap->SamplerDesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)in.Int(b_AddCapacity);
        specularMap->SamplerDesc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)in.Int(b_AddCapacity);
        specularMap->CreateSampler();
    }
    cnt = in.Int(b_AddCapacity);
    if (cnt > 0)
    {
        emissiveMap = RESOURCE->textures.Load(in.String(b_AddCapacity));
        for (int i = 1; i < cnt; i++)
        {
            emissiveMap->AddTexture();
            emissiveMap->LoadFile(in.String(b_AddCapacity), i, false);
        }
        emissiveMap->SamplerDesc.Filter = (D3D11_FILTER)in.Int(b_AddCapacity);
        emissiveMap->SamplerDesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)in.Int(b_AddCapacity);
        emissiveMap->SamplerDesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)in.Int(b_AddCapacity);
        emissiveMap->SamplerDesc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)in.Int(b_AddCapacity);
        emissiveMap->CreateSampler();
    }
    shininess = in.Float(b_AddCapacity);
    opacity = in.Float(b_AddCapacity);
    environment = in.Float(b_AddCapacity);
    shadow = in.Float(b_AddCapacity);

    in.Close();

}

void Material::SaveFile(string file)
{
    this->file = file;
    BinaryWriter out;
    wstring path = L"Contents/Material/" + Util::ToWString(file);
    out.Open(path);

    out.color4f(ambient);
    out.color4f(diffuse);
    out.color4f(specular);
    out.color4f(emissive);

    if (normalMap)
    {
        //out.Int(1);
        int cnt = normalMap->GetTextureCount();
        out.Int(cnt);
        for (int i = 0; i < cnt; i++)
            out.String(normalMap->GetTextureName(i));
        //out.String(normalMap->file);
        out.Int((D3D11_FILTER)normalMap->SamplerDesc.Filter);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)normalMap->SamplerDesc.AddressU);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)normalMap->SamplerDesc.AddressV);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)normalMap->SamplerDesc.AddressW);
    }
    else
        out.Int(0);

    if (diffuseMap)
    {
        //out.Int(1);
        int cnt = diffuseMap->GetTextureCount();
        out.Int(cnt);
        for (int i = 0; i < cnt; i++)
            out.String(diffuseMap->GetTextureName(i));
        //out.String(diffuseMap->file);
        out.Int((D3D11_FILTER)diffuseMap->SamplerDesc.Filter);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)diffuseMap->SamplerDesc.AddressU);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)diffuseMap->SamplerDesc.AddressV);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)diffuseMap->SamplerDesc.AddressW);
    }
    else
        out.Int(0);

    if (specularMap)
    {
        //out.Int(1);
        int cnt = specularMap->GetTextureCount();
        out.Int(cnt);
        for (int i = 0; i < cnt; i++)
            out.String(specularMap->GetTextureName(i));
        //out.String(specularMap->file);
        out.Int((D3D11_FILTER)specularMap->SamplerDesc.Filter);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)specularMap->SamplerDesc.AddressU);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)specularMap->SamplerDesc.AddressV);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)specularMap->SamplerDesc.AddressW);
    }
    else
        out.Int(0);

    if (emissiveMap)
    {
        //out.Int(1);
        int cnt = emissiveMap->GetTextureCount();
        out.Int(cnt);
        for (int i = 0; i < cnt; i++)
            out.String(emissiveMap->GetTextureName(i));
        //out.String(emissiveMap->file);
        out.Int((D3D11_FILTER)emissiveMap->SamplerDesc.Filter);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)emissiveMap->SamplerDesc.AddressU);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)emissiveMap->SamplerDesc.AddressV);
        out.Int((D3D11_TEXTURE_ADDRESS_MODE)emissiveMap->SamplerDesc.AddressW);
    }
    else
        out.Int(0);



    out.Float(shininess);
    out.Float(opacity);
    out.Float(environment);
    out.Float(shadow);

    out.Close();
}

void Material::RenderNum(string type, UINT& nTex, shared_ptr<Texture> target)
{
    UINT size = target ? target->GetTextureCount() : 0;
    bool block_left = (nTex <= 0);
    bool block_right = (target == nullptr || (nTex >= (target->GetTextureCount() - 1)));
    bool block_add = (target == nullptr || (target->GetTextureCount() >= 9));
    bool block_remove = (target == nullptr || (target->GetTextureCount() <= 1));

    if(block_left) ImGui::BeginDisabled();
    ImGui::SameLine(); if (ImGui::ArrowButton((type + "left").c_str(), ImGuiDir_Left)) { nTex--; };
    if(block_left) ImGui::EndDisabled();

    ImGui::SameLine(); ImGui::Text("%02d", nTex);

    if (block_right) ImGui::BeginDisabled();
    ImGui::SameLine(); if (ImGui::ArrowButton((type + "right").c_str(), ImGuiDir_Right)) { nTex++; };
    if (block_right) ImGui::EndDisabled();

    ImGui::SameLine(); ImGui::Text("(Size: %d", size);
    
    if (block_remove)ImGui::BeginDisabled();
    ImGui::SameLine(); if (ImGui::Button(("-##"+type).c_str())) 
    { 
        target->PopTexture();
        if (nTex >= target->GetTextureCount()) nTex = (target->GetTextureCount() - 1);
    }
    if (block_remove)ImGui::EndDisabled();

    if (block_add)ImGui::BeginDisabled();
    ImGui::SameLine(); if (ImGui::Button(("+##"+type).c_str())) 
    {
        target->AddTexture();
        nTex = target->GetTextureCount() - 1;
    }
    if (block_add)ImGui::EndDisabled();

    ImGui::SameLine(); ImGui::Text(")");
}
