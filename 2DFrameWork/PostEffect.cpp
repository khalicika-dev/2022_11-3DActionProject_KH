#include "framework.h"
ID3D11Buffer* PostEffect::blurBuffer = nullptr;
void PostEffect::CreateStaticMember()
{
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = sizeof(Blur);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//상수버퍼
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &blurBuffer);
		assert(SUCCEEDED(hr));

	}
}

void PostEffect::DeleteStaticMember()
{
	SafeRelease(blurBuffer);
}

PostEffect::PostEffect()
{
	actor = UI::Create();
	actor->LoadFile("Window.xml");
}

PostEffect::~PostEffect()
{
	SafeRelease(actor);
}

void PostEffect::SetCapture()
{
	Set(Color(0, 0, 0, 1));
}

void PostEffect::Render()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D->GetDC()->Map(blurBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy_s(mappedResource.pData, sizeof(Blur), &blur, sizeof(Blur));
	D3D->GetDC()->Unmap(blurBuffer, 0);
	D3D->GetDC()->PSSetConstantBuffers(10, 1, &blurBuffer);

	actor->Update();
	//actor->material->diffuseMap->srv = rtvSrv;
	actor->material->diffuseMap->SetTextureSRV(0, rtvSrv);
	actor->Render();
}

void PostEffect::RenderDetail()
{
	ImGui::SliderInt("Filter", &blur._Filter, 0, 5);
	ImGui::SliderInt("Select", &blur._Select, 0, 7);
	ImGui::SliderInt("Count", &blur._Count, 1, 1000);
	ImGui::DragFloat("Width", &blur._Width, 0.05f);
	ImGui::DragFloat("Height", &blur._Height, 0.05f);
	ImGui::ColorEdit3("Color", (float*)&blur._Color, ImGuiColorEditFlags_PickerHueWheel);
	ImGui::DragFloat("Radius", &blur._Radius, 0.05f);
	ImGui::DragFloat2("ScreenPos", (float*)&blur._Screen, 0.05f);
	actor->RenderDetail();
}
