#include "stdafx.h"

Object_Trail::Object_Trail(Object* trailStart, Object* trailEnd)
	: trailStart(trailStart), trailEnd(trailEnd)
{
	actor = Actor::Create();
	Init();
	
	//material = RESOURCE->materials.Load();

	//----
	void* vertices = nullptr;
	UINT* indices = nullptr;

	//----
	VertexPT* vertex;
	vertexCount = trailMaxCount * 2;
	vertex = new VertexPT[vertexCount];
	indices = new UINT[vertexCount];

	for (int i = 0; i < vertexCount; i++)
	{
		vertex[i].position = { 0,0,0 };
		vertex[i].uv = { 0,0 };
		indices[i] = i;
	}

	vertices = (void*)vertex;
	//----

	actor->mesh = make_shared<Mesh>(vertices, vertexCount, indices, vertexCount,
		VertexType::PT, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	actor->shader = RESOURCE->shaders.Load("6.Trail.hlsl");
	actor->material = RESOURCE->materials.Load("Claris/Claris_Trail.mtl");
}

Object_Trail::~Object_Trail()
{
	SafeRelease(actor);
}

void Object_Trail::Calculate()
{
	RESOURCE->AddTotalCapacity(RESOURCE->shaders.Calculate("6.Trail.hlsl"));
	RESOURCE->AddTotalCapacity(RESOURCE->materials.Calculate("Claris/Claris_Trail.mtl"));
}

void Object_Trail::Update()
{
	if (trailAddTime > 0.0f) trailAddTime -= DELTA;
	if (!isAdd && !startPos.empty())
		RemoveTrail();

	isAdd = false;
	actor->Update();
}

void Object_Trail::Render()
{
	if (startPos.size() <= 1) return;
	GAMEOBJ->q_trails.push(this);
}

void Object_Trail::Init()
{
	startPos.clear();
	endPos.clear();
	trailAddTime = 0.0f;
	isAdd = false;
}

void Object_Trail::UpdateTrail()
{
	VertexPT* vertices = (VertexPT*)actor->mesh->vertices;
	int len = startPos.size() - 1;
	
	for (int i = 0; i <= len; i++)
	{
		float uvX = ((float)i / (float)len);
		vertices[i * 2].position = endPos[i];	// Ä®³¡
		vertices[i * 2].uv = { uvX,1 };
		vertices[i * 2 + 1].position = startPos[i];	// Ä®¼ÕÀâÀÌ
		vertices[i * 2 + 1].uv = { uvX,0 };
	}
	for (int i = len + 1; i < trailMaxCount; i++)
	{
		vertices[i * 2].position = endPos.empty() ? Vector3(0, 0, 0) : endPos.back();
		vertices[i * 2].uv = { 1,1 };
		vertices[i * 2 + 1].position = endPos.empty() ? Vector3(0, 0, 0) : endPos.back();
		vertices[i * 2 + 1].uv = { 1,0 };
	}
	
	actor->mesh->UpdateMesh();
}

void Object_Trail::AddTrail()
{
	if (trailAddTime <= 0.0f)
	{
		startPos.push_front(trailStart->GetWorldPos());
		endPos.push_front(trailEnd->GetWorldPos());
		if (startPos.size() > trailMaxCount)
			RemoveTrail();
		trailAddTime = trailAddTimeMax;
		isAdd = true;
		UpdateTrail();
	}
}

void Object_Trail::RemoveTrail()
{
	startPos.pop_back();
	endPos.pop_back();

	UpdateTrail();
}
