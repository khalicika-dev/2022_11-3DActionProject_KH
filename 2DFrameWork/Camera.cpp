#include "framework.h"

Camera::Camera()
{
	fov = 60.0f * TORADIAN;
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	width= App.GetWidth();
	height = App.GetHeight();
	viewport.width = App.GetWidth();
	viewport.height = App.GetHeight();
	nearZ = 0.1f;
	farZ = 500.0f;
	type = ObType::Camera;
	ortho = false;
}
Camera::~Camera()
{
}

Camera* Camera::Create(string name)
{
	Camera* temp = new Camera();
	temp->name = name;
	return temp;
}

void Camera::Update()
{
    Object::Update();
}
void Camera::Set()
{
	{
		view = (S.Invert() * W).Invert();
		if (ortho)
			proj = Matrix::CreateOrthographic(width, height, nearZ, farZ);
		else
			proj = Matrix::CreatePerspectiveFieldOfView(fov, width / height, nearZ, farZ);
		Matrix TV = view.Transpose();
		Matrix TP = proj.Transpose();
		Matrix TVP = view * proj;
		TVP = TVP.Transpose();
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3D->GetDC()->Map(VPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy_s(mappedResource.pData, sizeof(Matrix), &TVP, sizeof(Matrix));
		D3D->GetDC()->Unmap(VPBuffer, 0);

		D3D11_MAPPED_SUBRESOURCE mappedResource2;
		D3D->GetDC()->Map(PBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource2);
		memcpy_s(mappedResource2.pData, sizeof(Matrix), &TP, sizeof(Matrix));
		D3D->GetDC()->Unmap(PBuffer, 0);

		D3D11_MAPPED_SUBRESOURCE mappedResource3;
		D3D->GetDC()->Map(VBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource3);
		memcpy_s(mappedResource3.pData, sizeof(Matrix), &TV, sizeof(Matrix));
		D3D->GetDC()->Unmap(VBuffer, 0);

		D3D->GetDC()->VSSetConstantBuffers(3, 1, &VBuffer);
		D3D->GetDC()->VSSetConstantBuffers(4, 1, &PBuffer);
		D3D->GetDC()->VSSetConstantBuffers(1, 1, &VPBuffer);
		D3D->GetDC()->GSSetConstantBuffers(0, 1, &VPBuffer);
	}
	{
		Vector4 viewPos = Vector4(GetWorldPos().x, GetWorldPos().y, GetWorldPos().z,1.0f);
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3D->GetDC()->Map(viewPosBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy_s(mappedResource.pData, sizeof(Vector4), &viewPos, sizeof(Vector4));
		D3D->GetDC()->Unmap(viewPosBuffer, 0);
		D3D->GetDC()->PSSetConstantBuffers(0, 1, &viewPosBuffer);
		D3D->GetDC()->GSSetConstantBuffers(1, 1, &viewPosBuffer);
	}
	{
		Vector3 viewUp = GetUp();
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3D->GetDC()->Map(viewUpBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy_s(mappedResource.pData, sizeof(Vector4), &viewUp, sizeof(Vector4));
		D3D->GetDC()->Unmap(viewUpBuffer, 0);
		D3D->GetDC()->GSSetConstantBuffers(2, 1, &viewUpBuffer);
	}
	{
		Vector3 viewForward = GetForward();
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3D->GetDC()->Map(viewForwardBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy_s(mappedResource.pData, sizeof(Vector4), &viewForward, sizeof(Vector4));
		D3D->GetDC()->Unmap(viewForwardBuffer, 0);
		D3D->GetDC()->GSSetConstantBuffers(3, 1, &viewForwardBuffer);
	}
    D3D->GetDC()->RSSetViewports(1, viewport.Get11());
}

void Camera::SetShadow()
{
	{
		Matrix TV = view.Transpose();
		Matrix TP = proj.Transpose();
		Matrix TVP = view * proj;
		TVP = TVP.Transpose();
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3D->GetDC()->Map(VPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy_s(mappedResource.pData, sizeof(Matrix), &TVP, sizeof(Matrix));
		D3D->GetDC()->Unmap(VPBuffer, 0);

		D3D11_MAPPED_SUBRESOURCE mappedResource2;
		D3D->GetDC()->Map(PBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource2);
		memcpy_s(mappedResource2.pData, sizeof(Matrix), &TP, sizeof(Matrix));
		D3D->GetDC()->Unmap(PBuffer, 0);

		D3D11_MAPPED_SUBRESOURCE mappedResource3;
		D3D->GetDC()->Map(VBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource3);
		memcpy_s(mappedResource3.pData, sizeof(Matrix), &TV, sizeof(Matrix));
		D3D->GetDC()->Unmap(VBuffer, 0);

		D3D->GetDC()->VSSetConstantBuffers(3, 1, &VBuffer);
		D3D->GetDC()->VSSetConstantBuffers(4, 1, &PBuffer);
		D3D->GetDC()->VSSetConstantBuffers(1, 1, &VPBuffer);
		D3D->GetDC()->GSSetConstantBuffers(0, 1, &VPBuffer);
	}
	{
		Vector4 viewPos = Vector4(GetWorldPos().x, GetWorldPos().y, GetWorldPos().z, 1.0f);
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3D->GetDC()->Map(viewPosBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy_s(mappedResource.pData, sizeof(Vector4), &viewPos, sizeof(Vector4));
		D3D->GetDC()->Unmap(viewPosBuffer, 0);
		D3D->GetDC()->PSSetConstantBuffers(0, 1, &viewPosBuffer);
		D3D->GetDC()->GSSetConstantBuffers(1, 1, &viewPosBuffer);
	}
	{
		Vector3 viewUp = GetUp();
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3D->GetDC()->Map(viewUpBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy_s(mappedResource.pData, sizeof(Vector4), &viewUp, sizeof(Vector4));
		D3D->GetDC()->Unmap(viewUpBuffer, 0);
		D3D->GetDC()->GSSetConstantBuffers(2, 1, &viewUpBuffer);
	}
	{
		Vector3 viewForward = GetForward();
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3D->GetDC()->Map(viewForwardBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy_s(mappedResource.pData, sizeof(Vector4), &viewForward, sizeof(Vector4));
		D3D->GetDC()->Unmap(viewForwardBuffer, 0);
		D3D->GetDC()->GSSetConstantBuffers(3, 1, &viewForwardBuffer);
	}
	D3D->GetDC()->RSSetViewports(1, viewport.Get11());


}

ID3D11Buffer* Camera::VPBuffer = nullptr;
ID3D11Buffer* Camera::VBuffer = nullptr;
ID3D11Buffer* Camera::PBuffer = nullptr;
ID3D11Buffer* Camera::viewPosBuffer = nullptr;
ID3D11Buffer* Camera::viewUpBuffer = nullptr;
ID3D11Buffer* Camera::viewForwardBuffer = nullptr;
Camera* Camera::main = nullptr;
Camera* Camera::follow = nullptr;
Actor*	Camera::followPivot = nullptr;
Camera::CameraState Camera::camState = Camera::CameraState::NOMRAL;
void Camera::CreateStaticMember()
{
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = sizeof(Matrix);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//상수버퍼
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &VPBuffer);
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
		HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &VBuffer);
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
		HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &PBuffer);
		assert(SUCCEEDED(hr));

	}
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = sizeof(Vector4);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//상수버퍼
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &viewPosBuffer);
		assert(SUCCEEDED(hr));
		
	}
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = sizeof(Vector4);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//상수버퍼
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &viewUpBuffer);
		assert(SUCCEEDED(hr));

	}
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = sizeof(Vector4);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//상수버퍼
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &viewForwardBuffer);
		assert(SUCCEEDED(hr));

	}
}

void Camera::DeleteStaticMember()
{
	SafeRelease(followPivot);
    SafeRelease(VPBuffer);
    SafeRelease(VBuffer);
    SafeRelease(PBuffer);
    SafeRelease(viewPosBuffer);
    SafeRelease(viewUpBuffer);
    SafeRelease(viewForwardBuffer);
}

void Camera::ControlMainCam(float scalar)
{
    if (not main) return;
	if (INPUT->KeyPress(VK_LSHIFT))
	{
		scalar = 10.0f;
	}

	if (INPUT->KeyPress('W'))
	{
		//                                  초당100움직임 xyz/s
		main->MoveWorldPos(main->GetForward() * DELTA * scalar);
	}
	if (INPUT->KeyPress('S'))
	{
		main->MoveWorldPos(-main->GetForward() * DELTA * scalar);
	}
	if (INPUT->KeyPress('A'))
	{
		main->MoveWorldPos(-main->GetRight() * DELTA * scalar);
	}
	if (INPUT->KeyPress('D'))
	{
		main->MoveWorldPos(main->GetRight() * DELTA * scalar);
	}
	if (INPUT->KeyPress('Q'))
	{
		main->MoveWorldPos(-main->GetUp() * DELTA * scalar);
	}
	if (INPUT->KeyPress('E'))
	{
		main->MoveWorldPos(main->GetUp() * DELTA * scalar);
	}

	//마우스 우클릭시
	if (INPUT->KeyPress(VK_RBUTTON))
	{
		Vector3 Rot;
		Rot.x = INPUT->movePosition.y * 0.001f;
		Rot.y = INPUT->movePosition.x * 0.001f;
		main->rotation += Rot;
	}
	//휠키로 카메라 앞뒤조절
	//main->MoveWorldPos(main->GetForward() * INPUT->wheelMoveValue.z * DELTA);
}

void Camera::FollowMainCam(float scalarPos, float scalarRot)
{
	// 메인카메라 추적
	Transform* fo = follow;
	Transform* tg = main;

	while (tg)
	{
		fo->SetLocalPos(Util::Lerp(fo->GetLocalPos(), tg->GetLocalPos(), scalarPos * TIMER->GetDeltaTime()));

		float dist[3] = {
			fabs(tg->rotation.x - fo->rotation.x),
			fabs(tg->rotation.y - fo->rotation.y),
			fabs(tg->rotation.z - fo->rotation.z)
		};
		Vector3 startRot = fo->rotation;
		//x
		if (fabs((tg->rotation.x + PI_2) - fo->rotation.x) < dist[0])
		{
			//dist[0] = fabs((tg->rotation.x + PI_2) - fo->rotation.x);
			startRot.x -= PI_2;
		}
		else if (fabs((tg->rotation.x - PI_2) - fo->rotation.x) < dist[0])
		{
			//dist[0] = fabs((tg->rotation.x - PI_2) - fo->rotation.x);
			startRot.x += PI_2;
		}

		//y
		if (fabs((tg->rotation.y + PI_2) - fo->rotation.y) < dist[1])
		{
			//dist[1] = fabs((tg->rotation.y + PI_2) - fo->rotation.y);
			startRot.y -= PI_2;
		}
		else if (fabs((tg->rotation.y - PI_2) - fo->rotation.y) < dist[1])
		{
			//dist[1] = fabs((tg->rotation.y - PI_2) - fo->rotation.y);
			startRot.y += PI_2;
		}

		//z
		if (fabs((tg->rotation.z + PI_2) - fo->rotation.z) < dist[2])
		{
			//dist[2] = fabs((tg->rotation.z + PI_2) - fo->rotation.z);
			startRot.z -= PI_2;
		}
		else if (fabs((tg->rotation.z - PI_2) - fo->rotation.z) < dist[2])
		{
			//dist[2] = fabs((tg->rotation.z - PI_2) - fo->rotation.z);
			startRot.z += PI_2;
		}
		fo->rotation = Util::Lerp(startRot, tg->rotation, scalarRot * TIMER->GetDeltaTime());
		if (!tg->parent) break;
		tg = tg->parent;
		fo = fo->parent;
	}

	// 지형충돌검사
	Transform* pivot = followPivot->Find("CameraPivot");
	if (pivot && follow)
	{
		Vector3 pivotPos = pivot->GetWorldPos();
		float originZ = follow->GetLocalPos().z;

		Vector3 dir = follow->GetWorldPos() - pivotPos;
		dir.Normalize();
		Ray ray = Ray(pivotPos, dir);
		float zMin = FLT_MAX;
		Vector3 Hit;

		vector<Terrain*> checkMap;
		int idx = -1;

		idx = Util::GetMyStandMap(pivotPos);
		if (idx >= 0) checkMap.push_back((*SCENE->Map)[idx]);

		idx = Util::GetMyStandMap(follow->GetWorldPos());
		if (idx >= 0) checkMap.push_back((*SCENE->Map)[idx]);

		for (auto m : checkMap)
		{
			if (m->ComPutePicking(ray, Hit))
			{
				if (zMin > Vector3::Distance(Hit, ray.position))
					zMin = Vector3::Distance(Hit, ray.position) - 1.0f;
			}

			// TERRAIN_COL옵션이 달린 맵콜라이더와의 충돌검사
			int col_Idx = 0;
			Object* ob;
			while (true && ray.direction.Length() > 0.0f)
			{
				col_Idx++;
				if (ob = m->Find("Col" + to_string(col_Idx)))
				{
					if ((ob->collider->gameType & (int)GameType::TERRAIN_COL) && ob->collider->Intersect(ray,Hit))
					{
						if (zMin > Vector3::Distance(Hit, ray.position))
							zMin = Vector3::Distance(Hit, ray.position) - 1.0f;
					}
				}
				else
					break;
			}
		}

		if (zMin > originZ) zMin = originZ;
		follow->SetLocalPosZ(zMin);
		checkMap.clear();
	}
}

void Camera::SetMainCam(Camera* target)
{
	if (target == nullptr)
		return;

	if (followPivot == nullptr)
	{
		followPivot = Actor::Create();
		followPivot->LoadFile("CamWithPivot.xml");
		if (App.GetAppQuit()) return;
		follow = dynamic_cast<Camera*>(followPivot->Find("Camera"));
		followPivot->name = "FollowPivot";
		follow->name = "FollowCamera";
	}

	main = target;
	if (target->parent)
	{
		Transform* fo = follow;
		Transform* tg = target;
		while (tg)
		{
			fo->SetLocalPos(tg->GetLocalPos());
			fo->rotation = tg->rotation;
			if (!tg->parent || !fo->parent) break;
			tg = tg->parent;
			fo = fo->parent;
		}
	}
	else
	{
		Transform* fo = follow;
		while (fo)
		{
			fo->SetLocalPos({ 0.0f,0.0f,0.0f });
			fo->rotation = { 0.0f,0.0f,0.0f };
			if (!fo->parent) break;
			fo = fo->parent;
		}
		follow->SetLocalPos(target->GetLocalPos());
		follow->rotation = target->rotation;
	}

	SCENE->ResizeScreen();
}

bool Camera::Intersect(BoundingFrustum Frustum, Collider* col)
{
	switch (col->type)
	{
	case ColliderType::BOX:
	{
		BoundingBox box;
		box.Center = col->GetWorldPos();
		box.Extents = Vector3(fabs(col->S._11), fabs(col->S._22), fabs(col->S._33));
		if (Frustum.Contains(box) != DISJOINT)
			return true;
		break;
	}
	case ColliderType::OBOX:
	{
		BoundingOrientedBox box;
		box.Center = col->GetWorldPos();
		box.Extents = Vector3(fabs(col->S._11), fabs(col->S._22), fabs(col->S._33));
		box.Orientation = Quaternion::CreateFromRotationMatrix(col->RT);
		if (Frustum.Contains(box) != DISJOINT)
			return true;
		break;
	}
	case ColliderType::SPHERE:
	{
		BoundingSphere box;
		box.Center = col->GetWorldPos();
		box.Radius = fabs(col->S._11);
		if (Frustum.Contains(box) != DISJOINT)
			return true;
		break;
	}
	}

	return false;
}

bool Camera::Intersect(Vector3 coord)
{
	BoundingFrustum Frustum;

	Frustum.Origin = GetWorldPos();
	Frustum.Orientation = Quaternion::CreateFromRotationMatrix(R);
	Frustum.RightSlope = tanf(fov / 2.0f) * 2.0f;
	Frustum.LeftSlope = -Frustum.RightSlope;
	Frustum.TopSlope = Frustum.RightSlope * App.GetHeight() / App.GetWidth();
	Frustum.BottomSlope = -Frustum.TopSlope;
	Frustum.Near = nearZ;
	Frustum.Far = farZ;

	if (Frustum.Contains(coord) != DISJOINT)
	{
		return true;
	}

	return false;
}

bool Camera::Intersect(Collider* col)
{
	BoundingFrustum Frustum;

	Frustum.Origin = GetWorldPos();
	Frustum.Orientation = Quaternion::CreateFromRotationMatrix(R);
	Frustum.RightSlope = tanf(fov / 2.0f) * 2.0f;
	Frustum.LeftSlope = -Frustum.RightSlope;
	Frustum.TopSlope = Frustum.RightSlope * App.GetHeight() / App.GetWidth();
	Frustum.BottomSlope = -Frustum.TopSlope;
	Frustum.Near = nearZ;
	Frustum.Far = farZ;

	return Intersect(Frustum, col);
}
