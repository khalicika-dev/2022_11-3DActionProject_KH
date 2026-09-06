#include "framework.h"

Object::Object()
{
	type = ObType::Object;
	shown = false;
	root = nullptr;
	visible = true;
	mesh = nullptr;
	shader = nullptr;
	material = nullptr;
	collider = nullptr;
}
Actor::Actor()
{
	type = ObType::Actor;
	root = this;
	skeleton = nullptr;
	anim = nullptr;
}
Object::~Object()
{
	SafeReset(mesh);
	SafeReset(shader);
	SafeReset(material);
	SafeDelete(collider);
}
Actor::~Actor()
{
	SafeDelete(skeleton);
	SafeDelete(anim);
}





Object* Object::Create(string name)
{
	Object* temp = new Object();
	temp->name = name;
	return temp;
}

void Object::Release()
{
	for (auto it = children.begin(); it != children.end(); it++)
	{
		SafeRelease(it->second);
	}
	delete this;
}


void Actor::Release()
{
	for (auto it = children.begin(); it != children.end(); it++)
	{
		SafeRelease(it->second);
	}
	delete this;
}

void Actor::ReleaseMember()
{
	for (auto it = children.begin(); it != children.end(); it++)
	{
		it->second->Release();
	}
	if (type < ObType::Rain)
	{
		SafeReset(mesh);
	}
	
	SafeReset(shader);
	SafeReset(material);
	SafeDelete(collider);
	SafeDelete(skeleton);
	SafeDelete(anim);
	obList.clear();
	children.clear();
}

Actor* Actor::Create(string name)
{
	Actor* temp = new Actor();
	temp->name = name;
	return temp;
}


void Object::Update()
{
	//스켈레톤 구조인가?
	if (boneIndex != -1)
	{
		if (root->anim)
		{
			Transform::UpdateAnim(root->anim->GetFrameBone(boneIndex, root->rootBoneIndex, root->animCurrentOffset));
		}
		else
		{
			Transform::Update();
		}
		Matrix temp = root->skeleton->bonesOffset[boneIndex] * W;
		//행우선->열우선
		root->skeleton->bones[boneIndex] = temp.Transpose();
	}
	else
	{
		Transform::Update();
	}
	if (collider)
		collider->Update(this);

	for (auto it = children.begin(); it != children.end(); it++)
		it->second->Update();
}

void Object::Render()
{
	if (visible)
	{
		bool bSeen;
		if (SCENE->currentCam && collider)
		{
			//Vector3 Dir = cube1->GetWorldPos() - Camera::main->GetWorldPos();
			//float Distance = Dir.Length();
			//Dir.Normalize();
			//Vector3 reflect = Vector3::Reflect(Dir, cube1->GetUp());
			//refract(ViewDir, Normal, RefractIndex);
			//Vector3::Refract();
			
			bSeen = SCENE->currentCam->Intersect(collider);
		}
		else
			bSeen = true;
		if (mesh && shader && bSeen)
		{
			Transform::Set();
			//prerender같은곳에서만 다른 쉐이더 사용할경우
			shader->Set();
			mesh->Set();

			if (material)
				material->Set();
			else
				defalutMaterial->Set();

			shown = true;
			D3D->GetDC()->DrawIndexed(mesh->indexCount, 0, 0);
		}
		else
			shown = false;

		for (auto it = children.begin(); it != children.end(); it++)
		{
			it->second->Render();
		}
	}

	if (collider)
		collider->Render();

	if (GUI->target == this && axis)
	{
		if (!parent)
		{
			if (rotationAxis)
				axis->W = axis->S * RT;
			else
				axis->W = axis->S * T;
		}
		else if (worldPos && !rotationAxis)
			axis->W = axis->S * Matrix::CreateTranslation(GetWorldPos());
		else
		{
			if (rotationAxis)
				axis->W = axis->S * RT * parent->S;
			else
				axis->W = axis->S * T * parent->W;
		}

		axis->Render();
	}

}

void Object::CubeMapRender()
{
	if (visible)
	{
		bool bSeen;
		if (SCENE->currentCam && collider)
		{
			if (Environment::GetCurrentCapture().mapType == 0)
			{
				//Vector3 Dir = Environment::GetCurrentCapture().camPos - SCENE->currentCam->GetWorldPos();
				//float Distance = Dir.Length();
				//Dir.Normalize();
				//Vector3 reflect = Vector3::Reflect(Dir, cube1->GetUp());
				//refract(ViewDir, Normal, RefractIndex);
				//Vector3::Refract();
				
				//Plane plane = Plane(Environment::GetCurrentCapture().reflectNormal, 0.0f);
				//Matrix reflectR = SCENE->currentCam->R.CreateReflection(plane);
				
				
				//BoundingFrustum frustum;
				//frustum.Origin = Environment::GetCurrentCapture().camPos;
				//frustum.Orientation = Quaternion::CreateFromRotationMatrix(reflectR);
				//frustum.RightSlope = tanf(SCENE->currentCam->fov / 2.0f) * 2.0f;
				//frustum.LeftSlope = -frustum.RightSlope;
				//frustum.TopSlope = frustum.RightSlope * App.GetHeight() / App.GetWidth();
				//frustum.BottomSlope = -frustum.TopSlope;
				//frustum.Near = SCENE->currentCam->nearZ;
				//frustum.Far = SCENE->currentCam->farZ;

				//bSeen = SCENE->currentCam->Intersect(frustum, collider);
				bSeen = SCENE->currentCam->Intersect(collider);
			}
			else
			{
				bSeen = SCENE->currentCam->Intersect(collider);
			}
		}
		else
			bSeen = true;
		
		if (mesh && !(material && material->environment > 0.0f) && bSeen)
		{
			Transform::Set();

			if (cubeMapShader[(int)mesh->vertexType])
			{
				cubeMapShader[(int)mesh->vertexType]->Set();
			}
			else
			{
				return;
			}


			mesh->Set();

			if (material)
				material->Set();
			else
				defalutMaterial->Set();

			D3D->GetDC()->DrawIndexed(mesh->indexCount, 0, 0);
		}
		for (auto it = children.begin(); it != children.end(); it++)
		{
			it->second->CubeMapRender();
		}
	}
}

void Object::ShadowMapRender()
{
	if (visible)
	{
		bool bSeen;
		if (SCENE->currentCam && collider)
		{
			//Vector3 Dir = cube1->GetWorldPos() - Camera::main->GetWorldPos();
			//float Distance = Dir.Length();
			//Dir.Normalize();
			//Vector3 reflect = Vector3::Reflect(Dir, cube1->GetUp());
			//refract(ViewDir, Normal, RefractIndex);
			//Vector3::Refract();

			bSeen = SCENE->currentCam->Intersect(collider);
		}
		else
			bSeen = true;
		if (mesh && bSeen)
		{
			Transform::Set();

			if (shadowMapShader[(int)mesh->vertexType])
			{
				shadowMapShader[(int)mesh->vertexType]->Set();
			}
			else
			{
				return;
			}
			mesh->Set();

			if (material)
				material->Set();
			else
				defalutMaterial->Set();

			D3D->GetDC()->DrawIndexed(mesh->indexCount, 0, 0);
		}
		for (auto it = children.begin(); it != children.end(); it++)
		{
			it->second->ShadowMapRender();
		}
	}
}

Object* Object::axis = nullptr;
Material* Object::defalutMaterial = nullptr;
Shader** Object::cubeMapShader = nullptr;
Shader** Object::shadowMapShader = nullptr;
void Object::CreateStaticMember()
{
	axis = new Object();
	axis->mesh = RESOURCE->meshes.Load("1.Transform.mesh");
	axis->shader  = RESOURCE->shaders.Load("1.Transform.hlsl");
	axis->S = Matrix::CreateScale(Vector3(500.0f, 500.0f, 500.0f));
	defalutMaterial = new Material();
	cubeMapShader = new Shader * [10];
	shadowMapShader = new Shader * [10];
	for (int i = 0; i < 10; i++)
	{
		cubeMapShader[i] = nullptr;
		shadowMapShader[i] = nullptr;
	}
	for (int i = 0; i < 6; i++)
	{
		cubeMapShader[i] = new Shader();
		cubeMapShader[i]->LoadFile(to_string(i) + ".CubeMap.hlsl", true);
		cubeMapShader[i]->LoadGeometry();
	}
	for (int i = 1; i < 6; i++)
	{
		shadowMapShader[i] = new Shader();
		shadowMapShader[i]->LoadFile(to_string(i) + ".ShadowMap.hlsl", true);
	}
}

void Object::DeleteStaticMember()
{
	SafeDelete(axis);
	SafeDelete(defalutMaterial);
	SafeDeleteArray(cubeMapShader);
	SafeDeleteArray(shadowMapShader);
}

void Object::CalculateStaticMember()
{
	RESOURCE->AddTotalCapacity(RESOURCE->meshes.Calculate("1.Transform.mesh"));
	RESOURCE->AddTotalCapacity(RESOURCE->shaders.Calculate("1.Transform.hlsl"));
	for (int i = 0; i < 6; i++)
		RESOURCE->AddTotalCapacity(Shader::CalculateFile(to_string(i) + ".CubeMap.hlsl"));
	for (int i = 1; i < 6; i++)
		RESOURCE->AddTotalCapacity(Shader::CalculateFile(to_string(i) + ".ShadowMap.hlsl"));
}

void Object::AddChild(Object* child)
{
	if (root->Find(child->name))
		return;
	root->obList[child->name] = child;
	children[child->name] = child;
	child->parent = this;
	child->root = root;
}
void Object::AddBone(Object* child)
{
	if (root->Find(child->name))
		return;
	child->boneIndex = root->boneIndexCount;
	root->boneIndexCount++;

	root->obList[child->name] = child;
	children[child->name] = child;
	child->parent = this;
	child->root = root;
}


Object* Actor::Find(string name)
{
	auto it = obList.find(name);
	//찾았다
	if (it != obList.end())
	{
		return it->second;
	}
	return nullptr;
}

bool Actor::DeleteObject(string Name)
{
	if (Name == name) return false;

	auto temp = obList.find(Name);

	if (temp == obList.end())  return false;

	//지워질대상,지워질대상의 부모 포인터
	Object* Target = temp->second;
	Object* Parent = temp->second->parent;
	//부모리스트에서 타겟 비우기
	Parent->children.erase(Parent->children.find(Name));

	for (auto it = Target->children.begin();
		it != Target->children.end(); it++)
	{
		Parent->children[it->second->name] = it->second;
		it->second->parent = Parent;
		//SafeRelease(it->second);
	}
	SafeDelete(Target);
	obList.erase(temp);
	return true;
}

void Actor::Update()
{
	if (anim)
	{
		if (anim->isChanging && anim->nextAnimator.currentFrame == 0)
		{
			animCurrentOffset = { 0.0f,0.0f,0.0f };
			animLastOffset = { 0.0f,0.0f,0.0f };
		}
		anim->Update();
		if (!anim->isChanging && anim->currentAnimator.currentFrame == 0)
		{
			animCurrentOffset = { 0.0f,0.0f,0.0f };
			animLastOffset = { 0.0f,0.0f,0.0f };
		}
	}

	animLastOffset = animCurrentOffset;
	Object::Update();
	if (anim)
	{
		//Matrix tempMM = Matrix(1, 0, 0, 0,
		//	0, 0, 1, 0,
		//	0, 1, 0, 0,
		//	0, 0, 0, 1);
		//Matrix tempM = Matrix::CreateTranslation(animCurrentOffset) * standardRootAxis * R;
		Matrix tempM = Matrix::CreateTranslation(animCurrentOffset) * standardRootAxis;
		Vector3 sc = root->Find("Offset")->scale;
		animCurrentOffset = { tempM._41 * sc.x, -tempM._42 * sc.y, tempM._43 * sc.z };
		//cout << "(" << animCurrentOffset.x << ", " << animCurrentOffset.y << ", " << animCurrentOffset.z << ")" << endl;

		if (!anim->isChanging && anim->currentAnimator.animState == AnimationState::STOP)
			animOffset = { 0.0f,0.0f,0.0f };
		else
			animOffset = animCurrentOffset - animLastOffset;
	}
}

void Actor::Render()
{
	if (skeleton)
	{
		//if (anim)anim->Update();
		skeleton->Set();
	}
	Object::Render();
}
void Actor::CubeMapRender()
{
	if (skeleton)
	{
		skeleton->Set();
	}
	Object::CubeMapRender();
}

void Actor::ShadowMapRender()
{
	if (skeleton)
	{
		skeleton->Set();
	}
	Object::ShadowMapRender();
}

void Actor::ClearAttackTouchedList()
{
	Object* ob;
	for (int i = 1; ob = Find("AttackCollider" + to_string(i) + "_1"); i++)
	{
		for (int j = 1; ob = Find("AttackCollider" + to_string(i) + "_" + to_string(j)); j++)
		{
			ob->collider->touched.clear();
		}
	}
}