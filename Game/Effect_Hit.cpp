#include "stdafx.h"

Effect_Hit::Effect_Hit()
{
	actor = Pop::Create();
	actor->material = make_shared<Material>();

    actor->material->file = "ptc_circle.mtl";
	actor->material->diffuseMap = RESOURCE->textures.Load("ptc_circle.png");
	Init();
}

Effect_Hit::~Effect_Hit()
{
	actor->Release();
}

void Effect_Hit::Calculate()
{
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("ptc_circle.png"));
}

void Effect_Hit::Update()
{
	actor->Update();
}

void Effect_Hit::LateUpdate()
{
	if (!dynamic_cast<Pop*>(actor)->IsPlaying())
		Enable = false;
}

void Effect_Hit::Render()
{
	actor->Render();
}

void Effect_Hit::CubeMapRender()
{
	actor->CubeMapRender();
}

void Effect_Hit::ShadowMapRender()
{
	actor->ShadowMapRender();
}

void Effect_Hit::Init()
{
	Pop* temp = dynamic_cast<Pop*>(actor);
	actor->rotation = { 0.0f,0.0f,0.0f };
	actor->scale = { 0.0f,0.0f,0.0f };
	actor->material->diffuse = Color(0.5f, 0.5f, 0.5f, 1.0f);
	temp->duration = 1.0f;
	temp->desc.gravity = 60.0f;
	temp->particleScale = Vector2(0, 0);
	temp->particleCount = 30;
	temp->velocityScalar = 50.0f;
	temp->rotRange = [=]() { return Vector3(temp->DefaultRot(), temp->DefaultRot(), temp->DefaultRot()); };
}
