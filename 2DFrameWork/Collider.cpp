#include "Framework.h"

Collider::Collider(ColliderType type)
{
	this->type = type;
	enable = true;
	visible = true;
	gameType = (int)GameType::NONE;
	switch (type)
	{
	case ColliderType::SPHERE:
		mesh = RESOURCE->meshes.Load("1.SphereCollider.mesh");
		break;
	case ColliderType::BOX:
		mesh = RESOURCE->meshes.Load("1.BoxCollider.mesh");
		break;
	case ColliderType::OBOX:
		mesh = RESOURCE->meshes.Load("1.BoxCollider.mesh");
		//mesh = RESOURCE->meshes.Load("2.Cube.mesh");
		break;
	}
	shader = RESOURCE->shaders.Load("1.Collider.hlsl");
	//shader = RESOURCE->shaders.Load("2.Shapes.hlsl");
	
}

Collider::~Collider()
{
	SafeReset(mesh);
	SafeReset(shader);
}

uint64_t Collider::CalculateType(ColliderType type)
{
	uint64_t total = 0;
	switch (type)
	{
	case ColliderType::SPHERE:
		total += RESOURCE->meshes.Calculate("1.SphereCollider.mesh");
		break;
	case ColliderType::BOX:
		total += RESOURCE->meshes.Calculate("1.BoxCollider.mesh");
		break;
	case ColliderType::OBOX:
		total += RESOURCE->meshes.Calculate("1.BoxCollider.mesh");
		//total += RESOURCE->meshes.Calculate("2.Cube.mesh");
		break;
	}
	total += RESOURCE->shaders.Calculate("1.Collider.hlsl");
	//total += RESOURCE->shaders.Calculate("2.Shapes.hlsl");
	return total;
}

void Collider::Update(Object* ob)
{
	parent = ob;
	this->Update();
}

void Collider::Update()
{
	if (type == ColliderType::SPHERE)
	{
		scale.z = scale.y = scale.x;
	}
	Transform::Update();
	if (type == ColliderType::BOX)
	{
		T = Matrix::CreateTranslation(GetWorldPos());
		W = S * T;
	}

	//if (!enable && !touched.empty())
	//	touched.clear();
}

void Collider::Render()
{
	if (visible && enable && VAR->showColliderComponents)
	{
		Transform::Set();
		mesh->Set();
		shader->Set();
		D3D->GetDC()->DrawIndexed(mesh->GetIndexCount(), 0, 0);
	}
}

void Collider::RenderDetail()
{
	switch (type)
	{
	case ColliderType::SPHERE:
	{
		ImGui::Text("Sphere");
		break;
	}
	case ColliderType::BOX:
	{
		ImGui::Text("Box");
		break;
	}
	case ColliderType::OBOX:
	{
		ImGui::Text("OBox");
		break;
	}
	}
	ImGui::Checkbox("isEnable", &enable);
	ImGui::Checkbox("isVisible", &visible);
	if (ImGui::Button("Flags"))
	{
		ImGui::OpenPopup("colFlags");
	}
	if (ImGui::BeginPopup("colFlags"))
	{
		ImGui::CheckboxFlags("JumpCollider", &gameType, (int)GameType::JUMP);
		ImGui::CheckboxFlags("PlayerOnly", &gameType, (int)GameType::ONLY_PLAYER);

		ImGui::CheckboxFlags("HitCollider(Player)", &gameType, (int)GameType::HIT_PLAYER);
		ImGui::CheckboxFlags("HitCollider(Monster)", &gameType, (int)GameType::HIT_MONSTER);
		ImGui::CheckboxFlags("LockOn(Monster)", &gameType, (int)GameType::LOCKON_MON);
		ImGui::CheckboxFlags("LockOn(MonsterParts)", &gameType, (int)GameType::LOCKON_MON_PARTS);
		ImGui::CheckboxFlags("Terrain", &gameType, (int)GameType::TERRAIN_COL);
		ImGui::CheckboxFlags("ColCenter", &gameType, (int)GameType::COL_CENTER);
		/*
		# note: it also allows to use multiple flags at once
		clicked, flags = imgui.checkbox_flags(
			"No resize & no move", flags,
			imgui.WINDOW_NO_RESIZE | imgui.WINDOW_NO_MOVE
		)
		*/
		ImGui::EndPopup();
	}
	ImGui::Text("Current flags value: %d", gameType);
	Transform::RenderDetail();
}

bool Collider::Intersect(Collider* target)
{
	if (!enable || !target->enable) return false;

	if (type == ColliderType::BOX)
	{
		BoundingBox box1;
		box1.Center = GetWorldPos();
		box1.Extents = Vector3(fabs(S._11), fabs(S._22), fabs(S._33));
		if (target->type == ColliderType::BOX)
		{
			BoundingBox box2;
			box2.Center = target->GetWorldPos();
			box2.Extents = Vector3(target->S._11, target->S._22, target->S._33);
			return box1.Intersects(box2);
		}
		else if (target->type == ColliderType::OBOX)
		{
			BoundingOrientedBox box2;
			box2.Center = target->GetWorldPos();
			box2.Extents = Vector3(target->S._11, target->S._22, target->S._33);
			box2.Orientation = Quaternion::CreateFromRotationMatrix(target->RT);
			return box1.Intersects(box2);
		}
		else
		{
			BoundingSphere box2;
			box2.Center = target->GetWorldPos();
			box2.Radius = target->S._11 ;
			return box1.Intersects(box2);
		}
	}
	else if (type == ColliderType::OBOX)
	{
		BoundingOrientedBox box1;
		box1.Center = GetWorldPos();
		box1.Extents = Vector3(S._11, S._22, S._33) ;
		box1.Orientation = Quaternion::CreateFromRotationMatrix(RT);
		if (target->type == ColliderType::BOX)
		{
			BoundingBox box2;
			box2.Center = target->GetWorldPos();
			box2.Extents = Vector3(target->S._11, target->S._22, target->S._33);
			return box1.Intersects(box2);
		}
		else if (target->type == ColliderType::OBOX)
		{
			BoundingOrientedBox box2;
			box2.Center = target->GetWorldPos();
			box2.Extents = Vector3(target->S._11, target->S._22, target->S._33);
			box2.Orientation = Quaternion::CreateFromRotationMatrix(target->RT);
			return box1.Intersects(box2);
		}
		else
		{
			BoundingSphere box2;
			box2.Center = target->GetWorldPos();
			box2.Radius = target->S._11;
			return box1.Intersects(box2);
		}
	}
	else
	{
		BoundingSphere box1;
		box1.Center = GetWorldPos();
		box1.Radius = S._11;
		if (target->type == ColliderType::BOX)
		{
			BoundingBox box2;
			box2.Center = target->GetWorldPos();
			box2.Extents = Vector3(target->S._11, target->S._22, target->S._33) ;
			return box1.Intersects(box2);
		}
		else if (target->type == ColliderType::OBOX)
		{
			BoundingOrientedBox box2;
			box2.Center = target->GetWorldPos();
			box2.Extents = Vector3(target->S._11, target->S._22, target->S._33);
			box2.Orientation = Quaternion::CreateFromRotationMatrix(target->RT);
			return box1.Intersects(box2);
		}
		else
		{
			BoundingSphere box2;
			box2.Center = target->GetWorldPos();
			box2.Radius = target->S._11;
			return box1.Intersects(box2);
		}
	}
	return false;
}

bool Collider::Intersect(Ray Ray, Vector3& Hit)
{
	if (!enable) return false;

	Ray.direction.Normalize();
	float Dis;
	bool result = false;
	if (type == ColliderType::BOX)
	{
		BoundingBox box1;
		box1.Center = GetWorldPos();
		box1.Extents = Vector3(fabs(S._11), fabs(S._22), fabs(S._33));
		result =Ray.Intersects(box1, Dis);
		Hit = Ray.position + Ray.direction * Dis;
	}
	else if (type == ColliderType::OBOX)
	{
		BoundingBox box1;
		box1.Center = Vector3(0, 0, 0);
		box1.Extents = Vector3(fabs(S._11), fabs(S._22), fabs(S._33));
		Matrix inverse = W.Invert();
		Ray.position = Vector3::Transform(Ray.position, inverse) * Vector3(S._11, S._22, S._33);
		Ray.direction = Vector3::TransformNormal(Ray.direction, inverse) * Vector3(S._11, S._22, S._33);
		Ray.direction.Normalize();

		result = Ray.Intersects(box1, Dis);
		Hit = (Ray.position + Ray.direction * Dis) / Vector3(S._11, S._22, S._33);
		Hit = Vector3::Transform(Hit, W);
	}
	else
	{
		BoundingSphere box1;
		box1.Center = GetWorldPos();
		box1.Radius = fabs(S._11);
		result = Ray.Intersects(box1, Dis);
		Hit = Ray.position + Ray.direction * Dis;
	}

	return result;
}
