#include "stdafx.h"

shared_ptr<Texture> DmgNumber::numTex[10];
float DmgNumber::imgHalfWidth = 0.0f;
void DmgNumber::CreateStaticMember()
{
	for (int i = 0; i < 10; i++)
	{
		numTex[i] = RESOURCE->textures.Load("UI/num_" + to_string(i) + ".png");
	}

	if(numTex[0])
		imgHalfWidth = numTex[0]->image.GetMetadata().width / DEFAULT_RESOLUTION.x;
}

void DmgNumber::DeleteStaticMember()
{
	for (int i = 0; i < 10; i++)
	{
		numTex[i] = nullptr;
	}
	imgHalfWidth = 0.0f;
}

void DmgNumber::CalculateStaticMember()
{
	for (int i = 0; i < 10; i++)
		RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/num_" + to_string(i) + ".png"));
}

DmgNumber::DmgNumber()
{
	enable = false;
	
	root = Actor::Create("dmg_root");
	for (int i = 0; i < 6; i++)
	{
		string name = "num" + to_string(i);
		UI* temp = UI::Create(name);
		temp->CreateMesh(numTex[0]);
		temp->SetLocalPos({ 0,0,0 });
		number[i] = temp;
		root->AddChild(number[i]);
	}
}

DmgNumber::~DmgNumber()
{
	root->Release();
}

void DmgNumber::Update()
{
	if (!enable) return;
	Vector3 ndcPos;
	Vector3 sc = { 1,1,1 };
	Color diff = { 1,1,1,1 };
	float op = 1.0f;	// 투명도

	Matrix W = Matrix::CreateTranslation(worldPos);
	Matrix Matrixtemp = W * Camera::GetFollowCam()->view * Camera::GetFollowCam()->proj;
	Matrixtemp /= Matrixtemp._44;
	
	ndcPos = { (Matrixtemp._41),(Matrixtemp._42),0.0f };
	root->visible = Matrixtemp._43 < 1.0f;

	time += DELTA;
	switch (dmgType)
	{
	case DmgType::NORMAL:
	case DmgType::ENDURE:
	case DmgType::HEAL:
		switch (dmgType)
		{
		case DmgType::NORMAL: diff = { 1,1,1,1 }; break;
		case DmgType::ENDURE: sc = { 0.7f,0.7f,0.7f }; diff = { 0.5f,0.5f,0.5f,1 }; break;
		case DmgType::HEAL: diff = { 0,1,0,1 }; break;
		}
		if (time < 0.15f)
		{
			float t = time / 0.15f;
			ndcPos.y = ndcPos.y - Util::Lerp(imgHalfWidth, 0.0f, t);
			op = Util::Lerp(0.0f, 1.0f, t);
		}
		else if (time < 1.0f)
		{
			op = 1.0f;
		}
		else if (time < 1.15f)
		{
			float t = (time - 1.0f) / 0.15f;
			ndcPos.y = ndcPos.y + Util::Lerp(0.0f, imgHalfWidth, t);
			op = Util::Lerp(1.0f, 0.0f, t);
		}
		else
		{
			sc = { 1,1,1 };
			op = 0.0f;
			enable = false;
		}
		break;
	case DmgType::CRITICAL:
		if (time < 0.1f)
		{
			float t = time / 0.1f;
			sc = Util::Lerp(Vector3(2, 2, 2), Vector3(1, 1, 1), t);
			diff = { 1,1,0,1 };
			
			op = Util::Lerp(0.0f, 1.0f, t);
		}
		else if (time < 0.25f)
		{
			float t = (time - 0.1f) / 0.15f;
			ndcPos.y = ndcPos.y + Util::Quadratic(0.0f, imgHalfWidth / 2.0f, 0.0f, t);
			sc = { 1,1,1 };
			diff = Util::Quadratic(Color(1, 1, 0, 1), Color(1, 1, 1, 1), Color(1, 1, 0, 1), t);

			op = 1.0f;
		}
		else if (time < 1.25f)
		{
			diff = { 1,1,0,1 };
			sc = { 1,1,1 };
			op = 1.0f;
		}
		else if (time < 1.4f)
		{
			float t = (time - 1.25f) / 0.15f;
			diff = { 1,1,0,1 };
			ndcPos.y = ndcPos.y + Util::Lerp(0.0f, imgHalfWidth, t);
			sc = { 1,1,1 };
			op = Util::Lerp(1.0f, 0.0f, t);
		}
		else
		{
			sc = { 1,1,1 };
			op = 0.0f;
			enable = false;
		}
		break;
	}
	for (auto n : number)
	{
		n->material->diffuse = diff;
		n->material->opacity = op;
	}
	root->scale = sc;
	root->SetWorldPos(ndcPos);
	root->Update();
}

void DmgNumber::Render()
{
	if (!enable) return;
	root->Render();
}

void DmgNumber::Play(Vector3 pos, int value, DmgType type)
{
	worldPos = pos;
	time = 0.0f;
	dmgType = type;

	value = abs(value);
	string valStr = to_string(value);
	int digit = valStr.length();
	float localX = -imgHalfWidth * (float)(digit - 1);

	// 왼쪽부터 오른쪽으로 배치시작
	for (int i = 0; i < 6; i++)
	{
		if (i < digit)
		{
			int n = stoi(valStr.substr(i, 1));
			number[i]->SetLocalPosX(localX);
			number[i]->material->diffuseMap = numTex[n];
			number[i]->visible = true;
			localX += imgHalfWidth * 2.0f;
		}
		else
		{
			number[i]->visible = false;
		}
	}
	enable = true;
	Update();
}
