#include "framework.h"

UI* UI::Create(string name)
{
	UI* temp = new UI();
	temp->name = name;
	temp->type = ObType::UI;

	temp->mesh = RESOURCE->meshes.Load("6.UI.mesh");
	temp->shader = RESOURCE->shaders.Load("6.UI.hlsl");
	return temp;
}

void UI::Update()
{
	float left = GetWorldPos().x - S._11 * 0.5f;
	float right = GetWorldPos().x + S._11 * 0.5f;
	float top = GetWorldPos().y + S._22 * 0.5f;
	float bottom = GetWorldPos().y - S._22 * 0.5f;

	if (left < INPUT->NDCPosition.x and INPUT->NDCPosition.x < right
		and bottom < INPUT->NDCPosition.y and
		INPUT->NDCPosition.y < top)
	{
		//함수포인터가 할당 되었다면 실행
		if (mouseOver) mouseOver();

		if (INPUT->KeyDown(VK_LBUTTON))
		{
			Press = true;
			if (mouseDown) mouseDown();
		}

	}

	if (Press)
	{
		if (INPUT->KeyPress(VK_LBUTTON))
		{
			if (mousePress) mousePress();
		}
		if (INPUT->KeyUp(VK_LBUTTON))
		{
			Press = false;

			if (mouseUp) mouseUp();
		}
	}

	Actor::Update();
}

bool UI::MouseOver()
{
	float left = GetWorldPos().x - S._11 * 0.5f;
	float right = GetWorldPos().x + S._11 * 0.5f;
	float top = GetWorldPos().y + S._22 * 0.5f;
	float bottom = GetWorldPos().y - S._22 * 0.5f;

	if (left < INPUT->NDCPosition.x and INPUT->NDCPosition.x < right
		and bottom < INPUT->NDCPosition.y and
		INPUT->NDCPosition.y < top)
	{
		return true;
	}
	return false;
}

Vector2 UI::NDCLength(Vector2 worldLength)
{
	float x = worldLength.x / (DEFAULT_RESOLUTION.x * 0.5f);
	float y = worldLength.y / -(DEFAULT_RESOLUTION.y * 0.5f);
	return Vector2(x, y);
}

float UI::NDCLengthX(float worldLengthX) 
{
	return worldLengthX / (DEFAULT_RESOLUTION.x * 0.5f);
}
float UI::NDCLengthY(float worldLengthY)
{
	return worldLengthY / -(DEFAULT_RESOLUTION.y * 0.5f);
}

void UI::SetNDCPos(Vector2 pos)
{
	//0~ 800  ->   0 ~ 2  -> -1 ~ 1
	float x = pos.x / (DEFAULT_RESOLUTION.x * 0.5f) - 1.0f;
	float y = pos.y / -(DEFAULT_RESOLUTION.y * 0.5f) + 1.0f;
	SetLocalPos({ x, y, 0.0f });
}

void UI::UpdateMesh(shared_ptr<Texture> texture, AlignX alignX, AlignY alignY)
{
	if (!texture) return;

	material->diffuseMap = texture;
	material->diffuse.w = 1.0f;

	//----
	VertexPT* vertices = (VertexPT*)mesh->vertices;

	float half_w = static_cast<float>(texture->image.GetMetadata().width) / DEFAULT_RESOLUTION.x;
	float half_h = static_cast<float>(texture->image.GetMetadata().height) / DEFAULT_RESOLUTION.y;

	float x1 = 0.0f;
	float x2 = 0.0f;
	float y1 = 0.0f;
	float y2 = 0.0f;
	switch (alignX)
	{
	case AlignX::LEFT:		x1 = half_w * (+0.0f);	x2 = half_w * (+2.0f);	break;
	case AlignX::CENTER:	x1 = half_w * (-1.0f);	x2 = half_w * (+1.0f);	break;
	case AlignX::RIGHT:		x1 = half_w * (-2.0f);	x2 = half_w * (+0.0f);	break;
	}

	switch (alignY)
	{
	case AlignY::TOP:		y1 = half_h * (-2.0f);	y2 = half_h * (+0.0f);	break;
	case AlignY::CENTER:	y1 = half_h * (-1.0f);	y2 = half_h * (+1.0f);	break;
	case AlignY::BOTTOM:	y1 = half_h * (+0.0f);	y2 = half_h * (+2.0f);	break;
	}

	vertices[0].position = Vector3(x1, y1, 0.0f);	// 좌하
	vertices[1].position = Vector3(x1, y2, 0.0f);	// 좌상
	vertices[2].position = Vector3(x2, y1, 0.0f);	// 우하
	vertices[3].position = Vector3(x2, y2, 0.0f);	// 우상

	mesh->UpdateMesh();
}

void UI::CreateMesh(string texturePath, AlignX alignX, AlignY alignY)
{
	shared_ptr<Texture> temp = RESOURCE->textures.Load(texturePath);
	CreateMesh(temp, alignX, alignY);
}

void UI::CreateMesh(shared_ptr<Texture> texture, AlignX alignX, AlignY alignY)
{
	material = make_shared<Material>();
	if (!texture) return;

	material->diffuseMap = texture;
	material->diffuse.w = 1.0f;

	//----
	void* vertices;
	UINT* indices;
	UINT vertexCount = 4;
	UINT indexCount = 4;

	float half_w = static_cast<float>(texture->image.GetMetadata().width) / DEFAULT_RESOLUTION.x;
	float half_h = static_cast<float>(texture->image.GetMetadata().height) / DEFAULT_RESOLUTION.y;

	float x1 = 0.0f;
	float x2 = 0.0f;
	float y1 = 0.0f;
	float y2 = 0.0f;
	switch (alignX)
	{
	case AlignX::LEFT:		x1 = half_w * (+0.0f);	x2 = half_w * (+2.0f);	break;
	case AlignX::CENTER:	x1 = half_w * (-1.0f);	x2 = half_w * (+1.0f);	break;
	case AlignX::RIGHT:		x1 = half_w * (-2.0f);	x2 = half_w * (+0.0f);	break;
	}

	switch (alignY)
	{
	case AlignY::TOP:		y1 = half_h * (-2.0f);	y2 = half_h * (+0.0f);	break;
	case AlignY::CENTER:	y1 = half_h * (-1.0f);	y2 = half_h * (+1.0f);	break;
	case AlignY::BOTTOM:	y1 = half_h * (+0.0f);	y2 = half_h * (+2.0f);	break;
	}

	//----
	VertexPT* vertex;
	vertex = new VertexPT[4];
	indices = new UINT[4];

	vertex[0].position = Vector3(x1, y1, 0.0f);	// 좌하
	vertex[0].uv = Vector2(0.0f, 1.0f);
	indices[0] = 0;

	vertex[1].position = Vector3(x1, y2, 0.0f);	// 좌상
	vertex[1].uv = Vector2(0.0f, 0.0f);
	indices[1] = 1;

	vertex[2].position = Vector3(x2, y1, 0.0f);	// 우하
	vertex[2].uv = Vector2(1.0f, 1.0f);
	indices[2] = 2;

	vertex[3].position = Vector3(x2, y2, 0.0f);	// 우상
	vertex[3].uv = Vector2(1.0f, 0.0f);
	indices[3] = 3;

	vertices = (void*)vertex;
	//----

	mesh = make_shared<Mesh>(vertices, vertexCount, indices, indexCount,
		VertexType::PT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}