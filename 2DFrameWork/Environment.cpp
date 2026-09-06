#include "framework.h"

ID3D11Buffer* Environment::camBuffer = nullptr;
ID3D11Buffer* Environment::PSBuffer = nullptr;
struct Environment::currentCapturedEnvironment Environment::currentCapturedEnv;
void Environment::CreateStaticMember()
{
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = sizeof(GSCamBuffer);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//상수버퍼
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &camBuffer);
		assert(SUCCEEDED(hr));

	}
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = sizeof(PSCubeMap);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//상수버퍼
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &PSBuffer);
		assert(SUCCEEDED(hr));

	}
}

void Environment::DeleteStaticMember()
{
	SafeRelease(camBuffer);
	SafeRelease(PSBuffer);
}

void Environment::SetCapture(Vector3 position, float minDist, float fov, float zNear, float zFar, Color clear)
{
	this->position = position;
	//Create Views
	{
		float x = position.x;
		float y = position.y;
		float z = position.z;

		struct LookAt
		{
			Vector3 LookAt;
			Vector3 Up;
		} lookAt[6];

		lookAt[0] = { Vector3(x + 1.0f, y, z), Vector3(0, 1.0f, 0) };
		lookAt[1] = { Vector3(x - 1.0f, y, z), Vector3(0, 1.0f, 0) };
		lookAt[2] = { Vector3(x, y + 1.0f, z), Vector3(0, 0, -1.0f) };
		lookAt[3] = { Vector3(x, y - 1.0f, z), Vector3(0, 0, +1.0f) };
		lookAt[4] = { Vector3(x, y, z + 1.0f), Vector3(0, 1.0f, 0) };
		lookAt[5] = { Vector3(x, y, z - 1.0f), Vector3(0, 1.0f, 0) };

		for (UINT i = 0; i < 6; i++)
		{
			desc.Views[i] =
				Matrix::CreateLookAt(position, lookAt[i].LookAt, lookAt[i].Up);
			desc.Views[i] = desc.Views[i].Transpose();
		}
	}
	desc.Projection =
		Matrix::CreatePerspectiveFieldOfView(fov, width / height, zNear, zFar);

	desc.Projection = desc.Projection.Transpose();

	D3D->GetDC()->RSSetViewports(1, viewport.Get11());

	currentCapturedEnv.camPos = position;
	currentCapturedEnv.mapType = desc2.CubeMapType;
	desc2.Rotation = Matrix::CreateFromYawPitchRoll(rot.y, rot.x, rot.z);
	desc2.Rotation = desc2.Rotation.Transpose();
	desc2.CubeCamPos[0] = position.x;
	desc2.CubeCamPos[1] = position.y;
	desc2.CubeCamPos[2] = position.z;
	desc2.CubeMinDist = minDist;

	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3D->GetDC()->Map(camBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy_s(mappedResource.pData, sizeof(GSCamBuffer), &desc, sizeof(GSCamBuffer));
		D3D->GetDC()->Unmap(camBuffer, 0);
		D3D->GetDC()->GSSetConstantBuffers(4, 1, &camBuffer);
	}

	Set(clear);
}

void Environment::SetTexture()
{
	water->Set(6);
	environment->SetTextureSRV(0, rtvSrv);
	//environment->srv = rtvSrv;
	environment->Set(5);
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3D->GetDC()->Map(PSBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy_s(mappedResource.pData, sizeof(PSCubeMap), &desc2, sizeof(PSCubeMap));
		D3D->GetDC()->Unmap(PSBuffer, 0);
		D3D->GetDC()->PSSetConstantBuffers(4, 1, &PSBuffer);
	}
}

void Environment::ResizeScreen(float width, float height)
{
	viewport.width = (float)width;
	viewport.height = (float)height;
	CubeRenderTarget::ResizeScreen(width, height);
}


Environment::Environment(UINT width, UINT height)
	:CubeRenderTarget(width, height)
{
	water = RESOURCE->textures.Load("Water.dds");
	environment = make_shared<Texture>();
	viewport.width = (float)width;
	viewport.height = (float)height;
}

Environment::~Environment()
{
	SafeReset(environment);
	SafeReset(water);
}

void Environment::RenderDetail()
{
	if (ImGui::SliderInt("TextureWidth", &width, 1, 2000))
	{
		ResizeScreen(width, height);
	}
	if (ImGui::SliderInt("TextureHeight", &height, 1, 2000))
	{
		ResizeScreen(width, height);
	}
	ImGui::SliderInt("MappingType", &desc2.CubeMapType, 0, 2);
	ImGui::SliderFloat("RefractIndex", &desc2.RefractIndex, 0, 2);
	ImGui::SliderFloat("WaterIndex", &desc2.WaterIndex, 0, 1);
	ImGui::SliderAngle("rotationX", &rot.x);
	ImGui::SliderAngle("rotationY", &rot.y);
	ImGui::SliderAngle("rotationZ", &rot.z);


}
