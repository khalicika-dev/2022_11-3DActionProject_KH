#include "framework.h"

void Transform::RenderDetail()
{
	ImGui::Checkbox("WorldPos", &worldPos);
	ImGui::SameLine();
	ImGui::Checkbox("RotationAxis", &rotationAxis);
	if (worldPos)
	{
		//get
		Vector3 wol = GetWorldPos();
		if (ImGui::DragFloat3("WorldPos", (float*)&wol, 0.05f))
		{
			//set
			SetWorldPos(wol);
			rotationAxis = false;
		}
	}
	else
	{
		Vector3 loc = GetLocalPos();
		if (ImGui::DragFloat3("LocalPos", (float*)&loc, 0.05f))
		{
			SetLocalPos(loc);
			rotationAxis = false;
		}
	}

	if(ImGui::SliderAngle("rotationX", &rotation.x)) rotationAxis = true;
	if(ImGui::SliderAngle("rotationY", &rotation.y)) rotationAxis = true;
	if(ImGui::SliderAngle("rotationZ", &rotation.z)) rotationAxis = true;
	if(ImGui::DragFloat3("scale", (float*)&scale, 0.05f)) rotationAxis = true;
}

bool Object::RenderHierarchy()
{
	ImGui::PushID(this);
	if (ImGui::TreeNode(name.c_str()))
	{
		if (ImGui::IsItemToggledOpen() or
			ImGui::IsItemClicked())
		{
			GUI->target = this;
		}
		if (ImGui::Button("addChild"))
		{
			ImGui::OpenPopup("childName");
		}
		if (ImGui::BeginPopup("childName"))
		{
			static char childName[32] = "None";
			ImGui::InputText("childName", childName, 32);
			if (ImGui::Button("Object"))
			{
				AddChild(Object::Create(childName));
			}
			ImGui::SameLine();
			if (ImGui::Button("Actor"))
			{
				AddChild(Actor::Create(childName));
			}
			ImGui::SameLine();
			if (ImGui::Button("Camera"))
			{
				AddChild(Camera::Create(childName));
			}
			ImGui::SameLine();
			if (ImGui::Button("Terrain"))
			{
				AddChild(Terrain::Create(childName));
			}
			ImGui::SameLine();
			if (ImGui::Button("UI"))
			{
				AddChild(UI::Create(childName));
			}
			ImGui::SameLine();
			if (ImGui::Button("Billboard"))
			{
				AddChild(Billboard::Create(childName));
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("delete"))
		{
			root->DeleteObject(name);
			GUI->target = nullptr;
			ImGui::TreePop();
			ImGui::PopID();
			return true; //하위자식까지는 접근하지 않기
		}
		// l->r
		for (auto it = children.begin(); it != children.end(); it++)
		{
			if (it->second->RenderHierarchy())
			{
				ImGui::TreePop();
				ImGui::PopID();
				GUI->target = nullptr;
				return true;
			}
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
	return false;
}

void Object::RenderDetail()
{
	ImGui::Text(name.c_str());
	ImGui::Checkbox("visible", &visible);
	if (ImGui::BeginTabBar("MyTabBar"))
	{
		if (ImGui::BeginTabItem("Transform"))
		{
			Transform::RenderDetail();
			//if (boneIndex != -1 && root->anim && !root->anim->isChanging/* && root->anim->currentAnimator.animState == AnimationState::STOP */ )
			//{
			//	ImGui::Spacing();
			//	ImGui::Text("Bone (can't modify)");
			//	Vector3 unusedM;	// 현재 사용안함
			//	Matrix tempM = root->anim->GetFrameBone(boneIndex, root->rootBoneIndex, unusedM);
			//	Vector3 toff = { tempM._41, tempM._42, tempM._43 };
			//	ImGui::DragFloat3("TotalOffset", (float*)&toff, 0.05f);
			//	Animations* ani = root->anim;
			//	//Matrix aa = root->anim->playList[]->arrFrameBone[root->anim->currentAnimator.nextFrame][boneIndex];
			//	//Matrix tempMM = ani->playList[ani->currentAnimator.animIdx]->arrFrameBone[ani->currentAnimator.nextFrame][boneIndex]
			//	//	* ani->currentAnimator.frameWeight +
			//	//	ani->playList[ani->currentAnimator.animIdx]->arrFrameBone[ani->currentAnimator.currentFrame][boneIndex]
			//	//	* (1.0f - ani->currentAnimator.frameWeight);
			//	//Vector3 ofs = { tempMM._41, tempMM._42, tempMM._43 };
			//	//ImGui::DragFloat3("deltaOffset", (float*)&ofs, 0.05f);
			//}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Mesh"))
		{
			if (mesh)
			{
				ImGui::Text(mesh->file.c_str());
			}
			if (GUI->FileImGui("Save", "Save Mesh",
				".mesh", "Contents/Mesh"))
			{
				string path = ImGuiFileDialog::Instance()->GetCurrentPath();
				Util::Replace(&path, "\\", "/");
				if (path.find("/Mesh/") != -1)
				{
					size_t tok = path.find("/Mesh/") + 6;
					path = path.substr(tok, path.length())
						+ "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				else
				{
					path = ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				mesh->SaveFile(path);
			}
			ImGui::SameLine();
			if (GUI->FileImGui("Load", "Load Mesh",
				".mesh", "Contents/Mesh"))
			{
				string path = ImGuiFileDialog::Instance()->GetCurrentPath();
				Util::Replace(&path, "\\", "/");
				if (path.find("/Mesh/") != -1)
				{
					size_t tok = path.find("/Mesh/") + 6;
					path = path.substr(tok, path.length())
						+ "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				else
				{
					path = ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				SafeReset(mesh);
				mesh = RESOURCE->meshes.Load(path);
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				SafeReset(mesh);
			}
			ImGui::SameLine();
			if (mesh == nullptr) ImGui::BeginDisabled();
			if (ImGui::Button("Optimize"))
			{
				cout << "Optimize Start...";
				mesh->OptimizeMesh();
				cout << "Done!" << endl;
			}
			ImGui::PushItemWidth(-400);
			ImGui::InputInt("start index", &startIndex);
			ImGui::PopItemWidth();
			ImGui::SameLine();
			if (ImGui::Button("Search Indices(99)"))
			{
				if (startIndex < 0 || startIndex >= mesh->GetIndexCount())
				{
					searchLog = "invalid range";
				}
				else
				{
					searchLog = "";
					string nextArrow = " -> ";
					int idx = startIndex;
					int k = startIndex % 3;
					int triCnt = 0;
					if (k != 0)
						searchLog.append("..." + nextArrow);
					for (int i = 0; i < 99; i++)
					{
						if (k == 0) searchLog.append("[");
						searchLog.append(to_string(mesh->indices[idx]));
						if (k >= 2)
						{
							searchLog.append("]");
							triCnt++;
							k = 0;
						}
						else
							k++;
						if (idx < mesh->GetIndexCount() - 1)
						{
							searchLog.append(nextArrow);
							if (i >= 98) searchLog.append("...");
						}
						if (triCnt != 0 && triCnt % 3 == 0)
						{
							searchLog.append("\n");
							triCnt = 0;
						}

						idx++;
						if (idx >= mesh->GetIndexCount())
							break;
					}
					searchLog.substr(0, searchLog.length() - nextArrow.length());
				}
			}
			if (mesh == nullptr) ImGui::EndDisabled();
			//ImGui::PushTextWrapPos(600.0f);
			ImGui::Text(searchLog.c_str());
			//ImGui::PopTextWrapPos();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Shader"))
		{
			if (shader)
			{
				ImGui::Text(shader->file.c_str());
			}
			if (GUI->FileImGui("Load", "Load Shader",
				".hlsl", "Shaders"))
			{
				string path = ImGuiFileDialog::Instance()->GetCurrentPath();
				Util::Replace(&path, "\\", "/");
				if (path.find("/Shaders/") != -1)
				{
					size_t tok = path.find("/Shaders/") + 9;
					path = path.substr(tok, path.length())
						+ "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				else
				{
					path = ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				SafeReset(shader);
				shader = RESOURCE->shaders.Load(path);
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				SafeReset(shader);
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Material"))
		{
			if (material)
			{
				ImGui::Text(material->file.c_str());
				material->RenderDetail();
			}
			if (GUI->FileImGui("Save", "Save Material",
				".mtl", "Contents/Material"))
			{
				string path = ImGuiFileDialog::Instance()->GetCurrentPath();
				Util::Replace(&path, "\\", "/");
				if (path.find("/Material/") != -1)
				{
					size_t tok = path.find("/Material/") + 10;
					path = path.substr(tok, path.length())
						+ "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				else
				{
					path = ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				material->SaveFile(path);
			}
			ImGui::SameLine();
			if (GUI->FileImGui("Load", "Load Material",
				".mtl", "Contents/Material"))
			{
				
				string path = ImGuiFileDialog::Instance()->GetCurrentPath();
				Util::Replace(&path, "\\", "/");
				if (path.find("/Material/") != -1)
				{
					size_t tok = path.find("/Material/") + 10;
					path = path.substr(tok, path.length())
						+ "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				else
				{
					path = ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				SafeReset(material);
				material = RESOURCE->materials.Load(path);
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				SafeReset(material);
				material = nullptr;
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Collider"))
		{
			if (collider)
			{
				collider->RenderDetail();
				if (ImGui::Button("Delete"))
				{
					SafeDelete(collider);
				}
			}
			else
			{
				ImGui::Text("Create");
				if (ImGui::Button("Box"))
				{
					collider = new Collider(ColliderType::BOX);
				}
				ImGui::SameLine();
				if (ImGui::Button("OBox"))
				{
					collider = new Collider(ColliderType::OBOX);
				}
				ImGui::SameLine();
				if (ImGui::Button("Sphere"))
				{
					collider = new Collider(ColliderType::SPHERE);
				}
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}
void Actor::RenderDetail()
{
	Object::RenderDetail();
	if (ImGui::BeginTabBar("MyTabBar2"))
	{
		if (ImGui::BeginTabItem("Actor"))
		{
			if (GUI->FileImGui("Save", "Save Actor",
				".xml", "Contents/GameObject"))
			{
				string path = ImGuiFileDialog::Instance()->GetCurrentPath();
				Util::Replace(&path, "\\", "/");
				if (path.find("/GameObject/") != -1)
				{
					size_t tok = path.find("/GameObject/") + 12;
					path = path.substr(tok, path.length())
						+ "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				else
				{
					path = ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				SaveFile(path);
			}
			ImGui::SameLine();
			if (GUI->FileImGui("Load", "Load Actor",
				".xml", "Contents/GameObject"))
			{
				string path = ImGuiFileDialog::Instance()->GetCurrentPath();
				Util::Replace(&path, "\\", "/");
				if (path.find("/GameObject/") != -1)
				{
					size_t tok = path.find("/GameObject/") + 12;
					path = path.substr(tok, path.length())
						+ "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				else
				{
					path = ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				LoadFile(path);
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Skeleton"))
		{
			if (skeleton)
			{
				ImGui::Text(skeleton->file.c_str());
			}
			if (GUI->FileImGui("Load", "Load Skeleton",
				".skel", "Contents/Skeleton/"))
			{
				string path = ImGuiFileDialog::Instance()->GetCurrentPath();
				Util::Replace(&path, "\\", "/");
				if (path.find("/Skeleton/") != -1)
				{
					size_t tok = path.find("/Skeleton/") + 10;
					path = path.substr(tok, path.length())
						+ "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				else
				{
					path = ImGuiFileDialog::Instance()->GetCurrentFileName();
				}
				SafeDelete(skeleton);
				skeleton = new Skeleton();
				skeleton->LoadFile(path, false);
			}
			/*ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				SafeDelete(skeleton);
			}*/
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Animations"))
		{
			if (anim)
			{
				anim->RenderDetail();
			}
			
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}



void Camera::RenderDetail()
{
	Actor::RenderDetail();
	if (ImGui::BeginTabBar("MyTabBar3"))
	{
		if (ImGui::BeginTabItem("Camera"))
		{
			ImGui::Checkbox("ortho", &ortho);
			ImGui::SliderAngle("fov", &fov,0,180.0f);
			ImGui::DragFloat("nearZ", &nearZ, 0.05f,0.00001f);
			ImGui::DragFloat("farZ", &farZ, 0.05f, 1.0f,2000.0f);
			ImGui::DragFloat("width", &width, 0.05f,1.0f);
			ImGui::DragFloat("height", &height, 0.05f, 1.0f);
			ImGui::DragFloat("x", &viewport.x, 0.05f, 0.0f);
			ImGui::DragFloat("y", &viewport.y, 0.05f, 0.0f);
			ImGui::DragFloat("w", &viewport.width, 0.05f, 1.0f);
			ImGui::DragFloat("h", &viewport.height, 0.05f, 1.0f);

			if (ImGui::Button("CameraToMainSet"))
			{
				SetMainCam(this);
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void Light::RenderDetail()
{
	Actor::RenderDetail();
	if (ImGui::BeginTabBar("MyTabBar3"))
	{
		if (ImGui::BeginTabItem("Light"))
		{
			ImGui::SliderInt("Type", &light->lightType, 0, 1);
			ImGui::ColorEdit3("Color", (float*)(&light->color));
			ImGui::SliderFloat("Range", &light->range,0,200);
			
			if (light->lightType == (int)LightType::SPOT)
			{
				ImGui::SliderFloat3("Dir", (float*)(&light->direction),-1,1);
				ImGui::SliderFloat("Outer", &light->outer,0.0f,90.0f);
				ImGui::SliderFloat("Inner", &light->inner,0.0f, 90.0f);
			}

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}


void Terrain::RenderDetail()
{
	Actor::RenderDetail();
	if (ImGui::BeginTabBar("MyTabBar3"))
	{
		if (ImGui::BeginTabItem("Terrain"))
		{
			ImGui::Checkbox("ShowNode", &showNode);
			if (ImGui::DragFloat("UvScale", &uvScale, 0.05f))
			{
				UpdateMeshUv();
			}
			static int terrainSize = 257;
			ImGui::DragInt("TerrainSize(Need to CreateMesh)", &terrainSize, 0.05f);

			if (ImGui::Button("CreateMeshes"))
			{
				CreateMesh(terrainSize);
			}

			if (ImGui::Button("UpdateNormal"))
			{
				UpdateMeshNormal();
			}
			if (ImGui::Button("ResetBuffer"))
			{
				DeleteStructuredBuffer();
				CreateStructuredBuffer();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}


void Rain::RenderDetail()
{
	Actor::RenderDetail();
	if (ImGui::BeginTabBar("MyTabBar3"))
	{
		if (ImGui::BeginTabItem("Rain"))
		{
			Particle::Gui();
			ImGui::SliderFloat3("Velocity", (float*)&desc.velocity, -1000, 1000);
			ImGui::SliderFloat3("particleRange", (float*)&desc.range, 0, 1000);
			ImGui::SliderFloat2("particleScale", (float*)&particleScale, 0, 100);
			ImGui::SliderInt("particleCount", &particleCount, 1, 100);

			if (ImGui::Button("Reset"))
			{
				Reset();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}


void Pop::RenderDetail()
{
	Actor::RenderDetail();
	if (ImGui::BeginTabBar("MyTabBar3"))
	{
		if (ImGui::BeginTabItem("Rain"))
		{
			Particle::Gui();
			ImGui::SliderFloat("gravity", &desc.gravity, -100.0f, 100.0f);
			ImGui::SliderFloat("velocityScalar", &velocityScalar, 0.0f, 1000.0f);
			ImGui::SliderFloat2("particleScale", (float*)&particleScale, 0, 100);
			ImGui::SliderInt("particleCount", &particleCount, 1, 100);

			if (ImGui::Button("Reset"))
			{
				Reset();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}
