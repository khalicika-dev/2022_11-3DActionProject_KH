#include "stdafx.h"
#include "Main.h"


Main::Main()
{

}

Main::~Main()
{

}

void Main::Init()
{
	Transform::CreateStaticMember();
	Camera::CreateStaticMember();
	Material::CreateStaticMember();
	Object::CreateStaticMember();
	Skeleton::CreateStaticMember();
	Terrain::CreateStaticMember();
	Rain::CreateStaticMember();
	Pop::CreateStaticMember();
	//App.background = Color(0.7f, 0.7f, 0.7f, 1.0f);
	VAR->isDebug = true;
	VAR->showColliderComponents = true;

	curTarget = nullptr;
	bVertexRefresh = false;
	bShowVertexID = false;

    Cam = Camera::Create();
    Cam->LoadFile("Cam.xml");
    Camera::SetMainCam(Cam);

    Grid = Actor::Create();
    Grid->LoadFile("Grid.xml");

	sphere = Actor::Create();
	sphere->LoadFile("Sphere.xml");

	popEffect = Pop::Create();
	popEffect->material = make_shared<Material>();
	popEffect->material->diffuseMap = RESOURCE->textures.Load("ptc_circle.png");
	popEffect->material->diffuse.w = 1.0f;

	/////////////////////////////////
	//popEffect->rotRange = [=]() {
	//	float rz = popEffect->DefaultRot();
	//	float ry = RANDOM->Float(-PI_DIV4, PI_DIV4);
	//	return Vector3(0, ry, rz);
	//};
	/////////////////////////////////
	popEffect->rotRange = [=]() {
		float ry = popEffect->DefaultRot();
		return Vector3(0, ry, 0);
	};
	/////////////////////////////////

	ResizeScreen();
}

void Main::Release()
{
    RESOURCE->ReleaseAll();
    Cam->Release();
    Grid->Release();
	sphere->Release();
}


void Main::Update()
{
	if (vDrag <= 0 || !isDraging)
		Camera::ControlMainCam(50.0f);
	ImGui::SliderFloat3("dirLight", (float*)(&LIGHT->dirLight.direction), -1, 1);

    ImGui::Text("FPS: %d", TIMER->GetFramePerSecond());
	ImGui::Checkbox("Show Collider", &VAR->showColliderComponents);
	ImGui::Checkbox("Show VertexID", &bShowVertexID);
	if (ImGui::Button("Refresh"))
	{
		bVertexRefresh = true;
	}
	ImGui::PushItemWidth(-400);
	ImGui::InputInt("VertexID", &select_vid);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("Check"))
	{
		bShowingInfo = true;
		showing_vid = select_vid;
	}
	if (bShowingInfo)
	{
		ShowVertexInfo(showing_vid);
	}
	ImGui::Text("Select Vertex: ");
	ImGui::RadioButton("Off", &vDrag, 0); ImGui::SameLine();
	ImGui::RadioButton("Add", &vDrag, 1); ImGui::SameLine();
	ImGui::RadioButton("subtract", &vDrag, 2);
	if (ImGui::Button("Clear"))
	{
		vText = "";
		for (int i = 0; i < vertexList.size(); i++)
		{
			vertexList[i].bSelected = false;
			vertexList[i].bSelected_temp = false;
		}
	}
	ImGui::SameLine();
	if(ImGui::Button("Search"))
	{
		vText = "";
		for (int i = 0; i < vertexList.size(); i++)
		{
			if (vertexList[i].bSelected)
			{
				for (int j = 0; j < vertexList[i].nums.size(); j++)
				{
					vText.append(to_string(vertexList[i].nums[j]) + " ");
				}
			}
		}
	}
	ImGui::PushTextWrapPos(600.0f);
	ImGui::Text(vText.c_str());
	ImGui::PopTextWrapPos();
	if (vText != "" && GUI->target && GUI->target->mesh && GUI->target->shader)
	{
		if (GUI->FileImGui("Save", "Save VertexData",
			".vData", "../Contents/Mesh"))
		{
			string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			Util::Replace(&filePath, "\\", "/");
			if (filePath.find("/Mesh/") != -1)
			{
				size_t tok = filePath.find("/Mesh/") + 6;
				filePath = filePath.substr(tok, filePath.length())
					+ "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
			}
			else
			{
				filePath = ImGuiFileDialog::Instance()->GetCurrentFileName();
			}
			BinaryWriter out;
			wstring path = L"../Contents/Mesh/" + Util::ToWString(filePath);
			out.Open(path);
			
			vector<int> vlist;
			for (int i = 0; i < vertexList.size(); i++)
			{
				if (vertexList[i].bSelected)
				{
					for (int j = 0; j < vertexList[i].nums.size(); j++)
					{
						vlist.push_back(vertexList[i].nums[j]);
					}
				}
			}
			out.UInt(vlist.size());
			for (int i = 0; i < vlist.size(); i++)
			{
				out.Int(vlist[i]);
			}
			out.Close();
		}
	}
    ImGui::Begin("Hierarchy");
    Cam->RenderHierarchy();
    Grid->RenderHierarchy();
	sphere->RenderHierarchy();
	popEffect->RenderHierarchy();
    for (size_t i = 0; i < List.size(); i++)
    {
        List[i]->RenderHierarchy();
    }

    //Cam->RenderHierarchy();
    ImGui::End();

    ImGui::Begin("AssimpImporter");

    if (GUI->FileImGui("ModelImporter", "ModelImporter",
        ".fbx,.obj,.x,.dae", "../Assets"))
    {
		file = ImGuiFileDialog::Instance()->GetCurrentFileName();
        string path = "../Assets/" + file;
        
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
        scene = importer.ReadFile
        (
			path,
            aiProcess_ConvertToLeftHanded
            | aiProcess_Triangulate
            | aiProcess_GenUVCoords
            | aiProcess_GenNormals
            | aiProcess_CalcTangentSpace
        );
        assert(scene != NULL and "Import Error");

        actor = Actor::Create(scene->mRootNode->mName.C_Str());
		actor->shader = RESOURCE->shaders.Load("4.Model.hlsl");
		actor->skeleton = new Skeleton();
		actor->boneIndex = 0;

		Object* offset = Object::Create("Offset");
		offset->shader = RESOURCE->shaders.Load("4.Model.hlsl");
		actor->AddBone(offset);

		ReadMaterial();

        ReadNode(offset, scene->mRootNode);
        
		{
			int tok = file.find_last_of(".");
			string checkPath = "../Contents/Skeleton/" + file.substr(0, tok);
			if (!PathFileExistsA(checkPath.c_str()))
			{
				CreateDirectoryA(checkPath.c_str(), NULL);
			}

			string filePath = file.substr(0, tok) + "/";
			actor->skeleton->file = filePath + file.substr(0, tok) + ".skel";
			actor->skeleton->SaveFile(actor->skeleton->file);
		}
		ReadMesh(offset, scene->mRootNode);
		
		
		List.push_back(actor);

		importer.FreeScene();
    }
	if (GUI->FileImGui("AnimationImporter", "AnimationImporter",
		".fbx,.obj,.x", "../Assets"))
	{
		file = ImGuiFileDialog::Instance()->GetCurrentFileName();
		string path = "../Assets/" + file;
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
		scene = importer.ReadFile
		(
			path,
			aiProcess_ConvertToLeftHanded
			| aiProcess_Triangulate
			| aiProcess_GenUVCoords
			| aiProcess_GenNormals
			| aiProcess_CalcTangentSpace
		);
		assert(scene != NULL and "Import Error");

		
		if (!actor->anim)actor->anim = new Animations();

		//애니메이션 갯수
		for (UINT i = 0; i < scene->mNumAnimations; i++)
		{
			shared_ptr<Animation> Anim = make_shared<Animation>();
			aiAnimation* srcAnim = scene->mAnimations[i];
			size_t tok2 = file.find_last_of(".");
			Anim->file = file.substr(0, tok2) + to_string(i);
			Anim->frameMax = (int)srcAnim->mDuration + 1;
			Anim->tickPerSecond = srcAnim->mTicksPerSecond != 0.0 ? (float)srcAnim->mTicksPerSecond : 25.0f;
			Anim->boneMax = actor->boneIndexCount;
			Anim->arrFrameBone = new Matrix * [Anim->frameMax];
			for (UINT j = 0; j < Anim->frameMax; j++)
			{
				Anim->arrFrameBone[j] = new Matrix[actor->boneIndexCount];
			}

			//채널갯수 -> 본에 대응
			for (UINT j = 0; j < srcAnim->mNumChannels; j++)
			{
				AnimNode* animNode = new AnimNode();
				aiNodeAnim* srcAnimNode = srcAnim->mChannels[j];

				animNode->name = srcAnimNode->mNodeName.C_Str();
				//Scale
				for (UINT k = 0; k < srcAnimNode->mNumScalingKeys; k++)
				{
					AnimScale srcScale;
					srcScale.time = (float)srcAnimNode->mScalingKeys[k].mTime;
					srcScale.scale.x = (float)srcAnimNode->mScalingKeys[k].mValue.x;
					srcScale.scale.y = (float)srcAnimNode->mScalingKeys[k].mValue.y;
					srcScale.scale.z = (float)srcAnimNode->mScalingKeys[k].mValue.z;
					animNode->scale.push_back(srcScale);
				}
				//Position
				for (UINT k = 0; k < srcAnimNode->mNumPositionKeys; k++)
				{
					AnimPosition srcPosition;
					srcPosition.time = (float)srcAnimNode->mPositionKeys[k].mTime;
					srcPosition.pos.x = (float)srcAnimNode->mPositionKeys[k].mValue.x;
					srcPosition.pos.y = (float)srcAnimNode->mPositionKeys[k].mValue.y;
					srcPosition.pos.z = (float)srcAnimNode->mPositionKeys[k].mValue.z;
					animNode->position.push_back(srcPosition);
				}
				//Rotation
				for (UINT k = 0; k < srcAnimNode->mNumRotationKeys; k++)
				{
					AnimRotation srcRotation;
					srcRotation.time = (float)srcAnimNode->mRotationKeys[k].mTime;
					srcRotation.quater.x = (float)srcAnimNode->mRotationKeys[k].mValue.x;
					srcRotation.quater.y = (float)srcAnimNode->mRotationKeys[k].mValue.y;
					srcRotation.quater.z = (float)srcAnimNode->mRotationKeys[k].mValue.z;
					srcRotation.quater.w = (float)srcAnimNode->mRotationKeys[k].mValue.w;
					animNode->rotation.push_back(srcRotation);
				}

				Object* chanel = actor->Find(animNode->name);
				if (chanel)
				{
					Matrix S, R, T;
					Quaternion quter;
					Vector3 pos, scale;
					for (UINT k = 0; k < Anim->frameMax; k++)
					{
						pos = Interpolated::CalcInterpolatedPosition(animNode, (float)k, Anim->frameMax);
						scale = Interpolated::CalcInterpolatedScaling(animNode, (float)k, Anim->frameMax);
						quter = Interpolated::CalcInterpolatedRotation(animNode, (float)k, Anim->frameMax);

						S = Matrix::CreateScale(scale);
						R = Matrix::CreateFromQuaternion(quter);
						T = Matrix::CreateTranslation(pos);
						Matrix W = S * R * T;
						Anim->arrFrameBone[k][chanel->boneIndex] = chanel->GetLocalInverse()*W;
					}
				}
				//여기서 채널끝(본)

			}
			//여기서 애님끝
			actor->anim->playList.push_back(Anim);

			{
				size_t tok = file.find_last_of(".");
				string checkPath = "../Contents/Animation/" + file.substr(0, tok);
				if (!PathFileExistsA(checkPath.c_str()))
				{
					CreateDirectoryA(checkPath.c_str(), NULL);
				}

				string filePath = file.substr(0, tok) + "/";
				Anim->file = filePath + Anim->file + ".anim";
				Anim->SaveFile(Anim->file);
			}

		}

		importer.FreeScene();
	}


    ImGui::End();

    Cam->Update();
    Grid->Update();
	sphere->Update();
	popEffect->Update();
    for (size_t i = 0; i < List.size(); i++)
    {
        List[i]->Update();
    }
}

void Main::LateUpdate()
{
	if (vDrag > 0)
	{
		if (INPUT->KeyDown(VK_LBUTTON))
		{
			isDraging = true;
			vText = "";
			
			selectRect[0] = INPUT->position;
		}
		if (INPUT->KeyPress(VK_LBUTTON))
		{
			selectRect[1] = INPUT->position;
			cout << INPUT->position.x << ", " << INPUT->position.y << endl;
		}
		if (INPUT->KeyUp(VK_LBUTTON))
		{
			isDraging = false;
			for (int i = 0; i < vertexList.size(); i++)
			{
				if (vertexList[i].bSelected_temp)
				{
					switch (vDrag)
					{
					case 1: vertexList[i].bSelected = true; break;
					case 2: vertexList[i].bSelected = false; break;
					}
				}
				vertexList[i].bSelected_temp = false;
			}
		}

		if (isDraging && bShowVertexID && GUI->target && GUI->target->mesh && GUI->target->shader)
		{
			for (int i = 0; i < vertexList.size(); i++)
			{
				Matrix M = Matrix::CreateTranslation(vertexList[i].pos) * GUI->target->W * Camera::GetMainCam()->view * Camera::GetMainCam()->proj;
				if (M._44 != 0.0f)
					M /= M._44;
				
				Vector3 pos = {
					(M._41 + 1.0f) * App.GetHalfWidth(),
					(M._42 - 1.0f) * -App.GetHalfHeight(),
					M._43
				};
				if (pos.z < 1.0f)
				{
					float rectX[2] = { min(selectRect[0].x, selectRect[1].x), max(selectRect[0].x, selectRect[1].x) };
					float rectY[2] = { min(selectRect[0].y, selectRect[1].y), max(selectRect[0].y, selectRect[1].y) };
					if (pos.x >= rectX[0] && pos.x <= rectX[1] &&
						pos.y >= rectY[0] && pos.y <= rectY[1])
					{
						vertexList[i].bSelected_temp = true;
					}
					else
					{
						vertexList[i].bSelected_temp = false;
					}
				}
			}
		}
	}
}

void Main::Render()
{
	LIGHT->Set();
    Cam->Set();
    Grid->Render();
	popEffect->Render();

	if (bShowVertexID) RASTER->Set(D3D11_CULL_BACK, D3D11_FILL_WIREFRAME);
	sphere->Render();
    for (size_t i = 0; i < List.size(); i++)
    {
        List[i]->Render();
    }
	RASTER->Set();

	if (!bVertexRefresh && bShowVertexID && GUI->target != nullptr && GUI->target == curTarget)
	{
		if (GUI->target->mesh && GUI->target->shader)
		{
			Matrix Matrixtemp;
			for (int i = 0; i < vertexList.size(); i++)
			{
				Vector3 pos = vertexList[i].pos;
				Matrixtemp = Matrix::CreateTranslation(pos) * GUI->target->W * Camera::GetMainCam()->view * Camera::GetMainCam()->proj;
				if (Matrixtemp._44 != 0.0f)
					Matrixtemp /= Matrixtemp._44;

				Vector3 strPos = {
					(Matrixtemp._41 + 1.0f) * App.GetHalfWidth(),
					(Matrixtemp._42 - 1.0f) * -App.GetHalfHeight(),
					Matrixtemp._43
				};
				if (strPos.z < 1.0f)
				{
					for (int j = 0; j < vertexList[i].nums.size(); j++)
					{
						wstring str = to_wstring(vertexList[i].nums[j]);
						float h = 15.0f;
						RECT rc{ strPos.x - 50.0f, strPos.y + h * j,strPos.x + 50.0f,strPos.y + h * j };
						//                    출력할 문자열,텍스트박스 크기위치
						Color color;
						
						if ((vDrag == 0 && vertexList[i].bSelected) ||
							(vDrag == 1 && (vertexList[i].bSelected || vertexList[i].bSelected_temp)) ||
							(vDrag == 2 && (vertexList[i].bSelected && !vertexList[i].bSelected_temp)))
							color = Color(0, 0, 1, 1);
						else
							color = Color(1, 0, 0, 1);
						DWRITE->RenderText(str, rc, h, L"Verdana", color);
					}
				}
			}
			//Matrix Matrixtemp;
			//UINT cnt = GUI->target->mesh->GetVertexCount();
			////cout << mesh->GetVertexCount() << endl;
			//for (int i = 0; i < cnt; i++)
			//{
			//	Vector3 pos = GUI->target->mesh->GetVertexPosition(i);
			//	Matrixtemp = Matrix::CreateTranslation(pos) * GUI->target->W * Camera::GetMainCam()->view * Camera::GetMainCam()->proj;
			//	Matrixtemp /= Matrixtemp._44;
			//
			//	wstring str = to_wstring(i);
			//	Vector3 strPos = {
			//		(Matrixtemp._41 + 1.0f) * App.GetHalfWidth(),
			//		(Matrixtemp._42 - 1.0f) * -App.GetHalfHeight(),
			//		Matrixtemp._43
			//	};
			//	//float ShoptextHeight = Matrixtemp._22 * 300.0f;
			//	//float ShoptextHeight = App.GetHeight() / Vector3::Distance(GetWorldPos(), Camera::GetMainCam()->GetWorldPos());
			//	//float ShoptextWidth = ShoptextHeight * str.length();
			//	//cout << "size: " << ShoptextHeight << ", AppHeight: " << App.GetHeight() << endl;
			//	if (strPos.z < 1.0f)
			//	{
			//		RECT rc{ strPos.x - 50.0f, strPos.y,strPos.x + 50.0f,strPos.y };
			//		//                    출력할 문자열,텍스트박스 크기위치
			//		DWRITE->RenderText(str, rc, 15.0f, L"Verdana", Color(1, 0, 0, 1));
			//	}
			//}
		}
	}
	else if(GUI->target != nullptr && GUI->target->mesh && GUI->target->shader && (GUI->target != curTarget || bVertexRefresh))
	{
		bVertexRefresh = false;
		bShowingInfo = false;
		select_vid = 0;
		curTarget = GUI->target;
		UINT cnt = GUI->target->mesh->GetVertexCount();
		vertexList.clear();
		for (int i = 0; i < cnt; i++)
		{
			Vector3 pos = GUI->target->mesh->GetVertexPosition(i, false);
			bool isNew = true;
			for (int j = 0; j < vertexList.size(); j++)
			{
				if (pos == vertexList[j].pos)
				{
					vertexList[j].nums.push_back(i);
					isNew = false;
					break;
				}
			}
			if (isNew)
			{
				vertexList.push_back(VertexInfo());
				vertexList.back().pos = pos;
				vertexList.back().nums.push_back(i);
				vertexList.back().bSelected = false;
				vertexList.back().bSelected_temp = false;
			}
		}
	}
}

void Main::ResizeScreen()
{
    Cam->width = App.GetWidth();
    Cam->height = App.GetHeight();
    Cam->viewport.width = App.GetWidth();
    Cam->viewport.height = App.GetHeight();
}

void Main::ReadMaterial()
{
	for (UINT i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* srcMtl = scene->mMaterials[i];
		Material* destMtl = new Material();
		aiColor3D tempColor;
		//이름 -키값
		destMtl->file = srcMtl->GetName().C_Str();
		//ambient
		srcMtl->Get(AI_MATKEY_COLOR_AMBIENT, tempColor);
		destMtl->ambient.x = tempColor.r;
		destMtl->ambient.y = tempColor.g;
		destMtl->ambient.z = tempColor.b;

		//diffuse
		srcMtl->Get(AI_MATKEY_COLOR_DIFFUSE, tempColor);
		destMtl->diffuse.x = tempColor.r;
		destMtl->diffuse.y = tempColor.g;
		destMtl->diffuse.z = tempColor.b;

		//specular
		srcMtl->Get(AI_MATKEY_COLOR_SPECULAR, tempColor);
		destMtl->specular.x = tempColor.r;
		destMtl->specular.y = tempColor.g;
		destMtl->specular.z = tempColor.b;

		//emissive
		srcMtl->Get(AI_MATKEY_COLOR_EMISSIVE, tempColor);
		destMtl->emissive.x = tempColor.r;
		destMtl->emissive.y = tempColor.g;
		destMtl->emissive.z = tempColor.b;

		//Shininess
		srcMtl->Get(AI_MATKEY_SHININESS, destMtl->shininess);
		//opacity
		srcMtl->Get(AI_MATKEY_OPACITY, destMtl->opacity);

		//Normal
		{
			aiString aifile;
			string TextureFile;
			aiReturn texFound;
			texFound = srcMtl->GetTexture(aiTextureType_NORMALS, 0, &aifile);
			TextureFile = aifile.C_Str();
			size_t index = TextureFile.find_last_of('/');
			TextureFile = TextureFile.substr(index + 1, TextureFile.length());

			//텍스쳐가 있다.
			if (texFound == AI_SUCCESS && file != "")
			{
				destMtl->ambient.w = 1.0f;
				destMtl->normalMap = make_shared<Texture>();

				size_t tok = file.find_last_of(".");
				string checkPath = "../Contents/Texture/" + file.substr(0, tok);
				if (!PathFileExistsA(checkPath.c_str()))
				{
					CreateDirectoryA(checkPath.c_str(), NULL);
				}
				string orgin = "../Assets/" + TextureFile;
				string copy = "../Contents/Texture/" + file.substr(0, tok) + "/" + TextureFile;
				bool isCheck = true;
				CopyFileA(orgin.c_str(), copy.c_str(), isCheck);

				destMtl->normalMap->LoadFile(file.substr(0, tok) + "/" + TextureFile, false);

			}
		}

		//Diffuse
		{
			aiString aifile;
			string TextureFile;
			aiReturn texFound;
			texFound = srcMtl->GetTexture(aiTextureType_DIFFUSE, 0, &aifile);
			TextureFile = aifile.C_Str();
			size_t index = TextureFile.find_last_of('/');
			TextureFile = TextureFile.substr(index + 1, TextureFile.length());

			//텍스쳐가 있다.
			if (texFound == AI_SUCCESS && file != "")
			{
				destMtl->diffuse.w = 1.0f;
				destMtl->diffuseMap = make_shared<Texture>();

				size_t tok = file.find_last_of(".");
				string checkPath = "../Contents/Texture/" + file.substr(0, tok);
				if (!PathFileExistsA(checkPath.c_str()))
				{
					CreateDirectoryA(checkPath.c_str(), NULL);
				}
				string orgin = "../Assets/" + TextureFile;
				string copy = "../Contents/Texture/" + file.substr(0, tok) + "/" + TextureFile;
				bool isCheck = true;
				CopyFileA(orgin.c_str(), copy.c_str(), isCheck);

				destMtl->diffuseMap->LoadFile(file.substr(0, tok) + "/" + TextureFile, false);

			}
		}

		//specular
		{
			aiString aifile;
			string TextureFile;
			aiReturn texFound;
			texFound = srcMtl->GetTexture(aiTextureType_SPECULAR, 0, &aifile);
			TextureFile = aifile.C_Str();
			size_t index = TextureFile.find_last_of('/');
			TextureFile = TextureFile.substr(index + 1, TextureFile.length());

			//텍스쳐가 있다.
			if (texFound == AI_SUCCESS && file != "")
			{
				destMtl->specular.w = 1.0f;
				destMtl->specularMap = make_shared<Texture>();

				size_t tok = file.find_last_of(".");
				string checkPath = "../Contents/Texture/" + file.substr(0, tok);
				if (!PathFileExistsA(checkPath.c_str()))
				{
					CreateDirectoryA(checkPath.c_str(), NULL);
				}
				string orgin = "../Assets/" + TextureFile;
				string copy = "../Contents/Texture/" + file.substr(0, tok) + "/" + TextureFile;
				bool isCheck = true;
				CopyFileA(orgin.c_str(), copy.c_str(), isCheck);

				destMtl->specularMap->LoadFile(file.substr(0, tok) + "/" + TextureFile, false);

			}
		}

		//emissive
		{
			aiString aifile;
			string TextureFile;
			aiReturn texFound;
			texFound = srcMtl->GetTexture(aiTextureType_EMISSIVE, 0, &aifile);
			TextureFile = aifile.C_Str();
			size_t index = TextureFile.find_last_of('/');
			TextureFile = TextureFile.substr(index + 1, TextureFile.length());

			//텍스쳐가 있다.
			if (texFound == AI_SUCCESS && file != "")
			{
				destMtl->emissive.w = 1.0f;
				destMtl->emissiveMap = make_shared<Texture>();

				size_t tok = file.find_last_of(".");
				string checkPath = "../Contents/Texture/" + file.substr(0, tok);
				if (!PathFileExistsA(checkPath.c_str()))
				{
					CreateDirectoryA(checkPath.c_str(), NULL);
				}
				string orgin = "../Assets/" + TextureFile;
				string copy = "../Contents/Texture/" + file.substr(0, tok) + "/" + TextureFile;
				bool isCheck = true;
				CopyFileA(orgin.c_str(), copy.c_str(), isCheck);

				destMtl->emissiveMap->LoadFile(file.substr(0, tok) + "/" + TextureFile, false);

			}
		}

		size_t tok = file.find_last_of(".");
		string checkPath = "../Contents/Material/" + file.substr(0, tok);
		if (!PathFileExistsA(checkPath.c_str()))
		{
			CreateDirectoryA(checkPath.c_str(), NULL);
		}

		string filePath = file.substr(0, tok) + "/";
		destMtl->file = filePath + destMtl->file + ".mtl";
		destMtl->SaveFile(destMtl->file);
	}
}

void Main::ReadNode(Object* dest, aiNode* src)
{
    Matrix tempMat = ToMatrix(src->mTransformation);
    Vector3 s, r, t; Quaternion q;
    tempMat.Decompose(s, q, t);
    r = Util::QuaternionToYawPtichRoll(q);
    dest->scale = s; dest->rotation = r; dest->SetLocalPos(t);

	actor->Update();
	//로컬로 향하는 행렬값 넣기
	actor->skeleton->bonesOffset[dest->boneIndex] = dest->W.Invert();

    //자식노드들로 재귀호출
    for (UINT i = 0; i < src->mNumChildren; i++)
    {
        Object* temp =
            Object::Create(src->mChildren[i]->mName.C_Str());
        dest->AddBone(temp);
        //temp->mesh = RESOURCE->meshes.Load("2.Sphere.mesh");
        temp->shader = RESOURCE->shaders.Load("4.Model.hlsl");
        ReadNode(temp, src->mChildren[i]);
    }

}

void Main::ReadMesh(Object* dest, aiNode* src)
{

	for (UINT i = 0; i < src->mNumMeshes; i++)
	{
		UINT index = src->mMeshes[i];
		aiMesh* srcMesh = scene->mMeshes[index];
		vector<VertexModel>		VertexList;
		vector<VertexWeights>	VertexWeights;
		vector<UINT> indexList;
		string mtlFile = scene->mMaterials[srcMesh->mMaterialIndex]->GetName().C_Str();
		string meshFile = src->mName.C_Str();
		int tok = file.find_last_of(".");
		string filePath = file.substr(0, tok) + "/";
		mtlFile = filePath + mtlFile + ".mtl";

		//1:1 대응 배열
		VertexList.resize(srcMesh->mNumVertices);
		VertexWeights.resize(srcMesh->mNumVertices);


		ReadBoneData(srcMesh, VertexWeights);

		for (UINT j = 0; j < VertexList.size(); j++)
		{
			memcpy(&VertexList[j].position, &srcMesh->mVertices[j], sizeof(Vector3));
			if (srcMesh->HasTextureCoords(0))
				memcpy(&VertexList[j].uv, &srcMesh->mTextureCoords[0][j], sizeof(Vector2));
			if (srcMesh->HasNormals())
				memcpy(&VertexList[j].normal, &srcMesh->mNormals[j], sizeof(Vector3));
			if (srcMesh->HasTangentsAndBitangents())
				memcpy(&VertexList[j].tangent, &srcMesh->mTangents[j], sizeof(Vector3));

			//본데이터가 있을때
			if (!VertexWeights.empty())
			{
				VertexWeights[j].Normalize();

				VertexList[j].indices.x = (float)VertexWeights[j].boneIdx[0];
				VertexList[j].indices.y = (float)VertexWeights[j].boneIdx[1];
				VertexList[j].indices.z = (float)VertexWeights[j].boneIdx[2];
				VertexList[j].indices.w = (float)VertexWeights[j].boneIdx[3];

				VertexList[j].weights.x = VertexWeights[j].boneWeights[0];
				VertexList[j].weights.y = VertexWeights[j].boneWeights[1];
				VertexList[j].weights.z = VertexWeights[j].boneWeights[2];
				VertexList[j].weights.w = VertexWeights[j].boneWeights[3];
			}
			
			VertexList[j].tex_weights = 0.0f;

		}//정점 저장 끝
		UINT IndexCount = srcMesh->mNumFaces;
		for (UINT j = 0; j < IndexCount; j++)
		{
			aiFace& face = srcMesh->mFaces[j];
			for (UINT k = 0; k < face.mNumIndices; k++)
			{
				indexList.emplace_back(face.mIndices[k]);
			}
		}
		VertexModel* VertexArray = new VertexModel[VertexList.size()];
		copy(VertexList.begin(), VertexList.end(), stdext::checked_array_iterator<VertexModel*>(VertexArray, VertexList.size()));

		UINT* IndexArray = new UINT[indexList.size()];
		copy(indexList.begin(), indexList.end(), stdext::checked_array_iterator<UINT*>(IndexArray, indexList.size()));

		if (i == 0)
		{
			dest->mesh = make_shared<Mesh>(VertexArray, VertexList.size(),
				IndexArray, indexList.size(), VertexType::MODEL);
			dest->material = RESOURCE->materials.Load(mtlFile);

			{
				int tok = file.find_last_of(".");
				string checkPath = "../Contents/Mesh/" + file.substr(0, tok);
				if (!PathFileExistsA(checkPath.c_str()))
				{
					CreateDirectoryA(checkPath.c_str(), NULL);
				}

				string filePath = file.substr(0, tok) + "/";
				dest->mesh->file = filePath + meshFile + ".mesh";
				dest->mesh->SaveFile(dest->mesh->file);
			}

		}
		else
		{
			string Name = srcMesh->mName.C_Str() + string("MeshObject");
			Object* temp = Object::Create(Name);
			dest->AddBone(temp);
			temp->mesh = make_shared<Mesh>(VertexArray, VertexList.size(),
				IndexArray, indexList.size(), VertexType::MODEL);
			temp->shader = RESOURCE->shaders.Load("4.Model.hlsl");
			temp->material = RESOURCE->materials.Load(mtlFile);

			{
				int tok = file.find_last_of(".");
				string checkPath = "../Contents/Mesh/" + file.substr(0, tok);
				if (!PathFileExistsA(checkPath.c_str()))
				{
					CreateDirectoryA(checkPath.c_str(), NULL);
				}

				string filePath = file.substr(0, tok) + "/";
				temp->mesh->file = filePath + Name + ".mesh";
				temp->mesh->SaveFile(temp->mesh->file);
			}
		}
	}
	for (UINT i = 0; i < src->mNumChildren; i++)
	{
		ReadMesh(dest->children[src->mChildren[i]->mName.C_Str()],
			src->mChildren[i]);
	}
}

void Main::ReadBoneData(aiMesh* mesh, vector<VertexWeights>& vertexWeights)
{
	//메쉬가 가지고 있는 본 개수 만큼
	for (UINT i = 0; i < mesh->mNumBones; i++)
	{
		//현재본이 하이어라이키에서 몇번째 인덱스인가?
		string boneName = mesh->mBones[i]->mName.C_Str();
		int boneIndex = actor->Find(boneName)->boneIndex;
		for (UINT j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			UINT vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
			vertexWeights[vertexID].AddData(boneIndex, mesh->mBones[i]->mWeights[j].mWeight);
		}
	}
}

void Main::ShowVertexInfo(UINT id)
{
	if (GUI->target == nullptr || !GUI->target->mesh || !GUI->target->shader ||
		id < 0 || id >= GUI->target->mesh->GetVertexCount())
	{
		ImGui::Text("Invalid Vertex");
		return;
	}
	VertexType vertexType = GUI->target->mesh->GetVertexType();
	void* vertices = GUI->target->mesh->vertices;

	ImGui::Text("[ID: %d]", showing_vid);
	if (vertexType == VertexType::PC)
	{
		VertexPC* Vertices = (VertexPC*)vertices;
		ImGui::Text("position: {%f, %f, %f}", Vertices[id].position.x, Vertices[id].position.y, Vertices[id].position.z);
		ImGui::Text("color: {%f, %f, %f, %f}", Vertices[id].color.x, Vertices[id].color.y, Vertices[id].color.z, Vertices[id].color.w);
	}
	else if (vertexType == VertexType::PCN)
	{
		VertexPCN* Vertices = (VertexPCN*)vertices;
		ImGui::Text("position: {%f, %f, %f}", Vertices[id].position.x, Vertices[id].position.y, Vertices[id].position.z);
		ImGui::Text("color: {%f, %f, %f, %f}", Vertices[id].color.x, Vertices[id].color.y, Vertices[id].color.z, Vertices[id].color.w);
		ImGui::Text("normal: {%f, %f, %f}", Vertices[id].normal.x, Vertices[id].normal.y, Vertices[id].normal.z);
	}
	else if (vertexType == VertexType::PTN)
	{
		VertexPTN* Vertices = (VertexPTN*)vertices;
		ImGui::Text("position: {%f, %f, %f}", Vertices[id].position.x, Vertices[id].position.y, Vertices[id].position.z);
		ImGui::Text("uv: {%f, %f}", Vertices[id].uv.x, Vertices[id].uv.y);
		ImGui::Text("normal: {%f, %f, %f}", Vertices[id].normal.x, Vertices[id].normal.y, Vertices[id].normal.z);
	}
	else if (vertexType == VertexType::MODEL)
	{
		VertexModel* Vertices = (VertexModel*)vertices;
		ImGui::Text("position: {%f, %f, %f}", Vertices[id].position.x, Vertices[id].position.y, Vertices[id].position.z);
		ImGui::Text("uv: {%f, %f}", Vertices[id].uv.x, Vertices[id].uv.y);
		ImGui::Text("normal: {%f, %f, %f}", Vertices[id].normal.x, Vertices[id].normal.y, Vertices[id].normal.z);
		ImGui::Text("tangent: {%f, %f, %f}", Vertices[id].tangent.x, Vertices[id].tangent.y, Vertices[id].tangent.z);
		ImGui::Text("indices: {%f, %f, %f, %f}", Vertices[id].indices.x, Vertices[id].indices.y, Vertices[id].indices.z, Vertices[id].indices.w);
		ImGui::Text("weights: {%f, %f, %f, %f}", Vertices[id].weights.x, Vertices[id].weights.y, Vertices[id].weights.z, Vertices[id].weights.w);
	}
	else if (vertexType == VertexType::TERRAIN)
	{
		VertexTerrain* Vertices = (VertexTerrain*)vertices;
		ImGui::Text("position: {%f, %f, %f}", Vertices[id].position.x, Vertices[id].position.y, Vertices[id].position.z);
		ImGui::Text("uv: {%f, %f}", Vertices[id].uv.x, Vertices[id].uv.y);
		ImGui::Text("normal: {%f, %f, %f}", Vertices[id].normal.x, Vertices[id].normal.y, Vertices[id].normal.z);
		ImGui::Text("weights: %f", Vertices[id].weights);
	}
	else if (vertexType == VertexType::PT)
	{
		VertexPT* Vertices = (VertexPT*)vertices;
		ImGui::Text("position: {%f, %f, %f}", Vertices[id].position.x, Vertices[id].position.y, Vertices[id].position.z);
		ImGui::Text("uv: {%f, %f}", Vertices[id].uv.x, Vertices[id].uv.y);
	}
	else if (vertexType == VertexType::PS)
	{
		VertexPS* Vertices = (VertexPS*)vertices;
		ImGui::Text("position: {%f, %f, %f}", Vertices[id].position.x, Vertices[id].position.y, Vertices[id].position.z);
		ImGui::Text("size: {%f, %f}", Vertices[id].size.x, Vertices[id].size.y);
	}
	else if (vertexType == VertexType::PSV)
	{
		VertexPSV* Vertices = (VertexPSV*)vertices;
		ImGui::Text("position: {%f, %f, %f}", Vertices[id].position.x, Vertices[id].position.y, Vertices[id].position.z);
		ImGui::Text("size: {%f, %f}", Vertices[id].size.x, Vertices[id].size.y);
		ImGui::Text("velocity: {%f, %f, %f}", Vertices[id].velocity.x, Vertices[id].velocity.y, Vertices[id].velocity.z);
	}
	else if (vertexType == VertexType::PSP)
	{
		VertexPSP* Vertices = (VertexPSP*)vertices;
		ImGui::Text("position: {%f, %f, %f}", Vertices[id].position.x, Vertices[id].position.y, Vertices[id].position.z);
		ImGui::Text("pivot: {%f, %f}", Vertices[id].pivot.x, Vertices[id].pivot.y);
		ImGui::Text("size: {%f, %f}", Vertices[id].size.x, Vertices[id].size.y);
	}
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR param, int command)
{
    App.SetAppName(L"Game1");
    App.SetInstance(instance);
	App.is_game = false;
    WIN->Create();
    D3D->Create();
    Main* main = new Main();
    main->Init();
    int wParam = (int)WIN->Run(main);
    main->Release();
    SafeDelete(main);
    D3D->DeleteSingleton();
    WIN->DeleteSingleton();

    return wParam;
}


Vector3 Interpolated::CalcInterpolatedScaling(AnimNode* iter, float time, int Duration)
{
	//비어있으면 1,1,1반환
	if (iter->scale.empty())
		return Vector3(1.0f, 1.0f, 1.0f);
	//한개만 있는놈은 첫번째 값 반환
	if (iter->scale.size() == 1)
		return iter->scale.front().scale;
	//마지막 놈은 마지막값 반환
	if (time == Duration - 1)
	{
		return iter->scale.back().scale;
	}
	//보간 시작
	int scaling_index = FindScale(iter, time);
	//-1 인덱스가 없으므로 마지막 값으로 반환
	if (scaling_index == -1)
	{
		return iter->scale.back().scale;
	}
	//보간 끝
	UINT next_scaling_index = scaling_index + 1;
	assert(next_scaling_index < iter->scale.size());

	//차이 나는 시간값
	float delta_time = (float)(iter->scale[next_scaling_index].time
		- iter->scale[scaling_index].time);
	//보간값
	float factor = (time - (float)(iter->scale[scaling_index].time)) / delta_time;
	//얘도 하나만 있는애
	if (factor < 0.0f)
	{
		return iter->scale.front().scale;
	}

	auto start = iter->scale[scaling_index].scale;
	auto end = iter->scale[next_scaling_index].scale;

	start = Vector3::Lerp(start, end, factor);
	return start;
}
Quaternion Interpolated::CalcInterpolatedRotation(AnimNode* iter, float time, int Duration)
{
	//auto rot_frames = iter->second->rotation;

	if (iter->rotation.empty())
		return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

	if (iter->rotation.size() == 1)
		return iter->rotation.front().quater;

	if (time == Duration - 1)
	{
		return iter->rotation.back().quater;
	}

	int quter_index = FindRot(iter, time);
	if (quter_index == -1)
	{
		return iter->rotation.back().quater;
	}
	UINT next_quter_index = quter_index + 1;
	assert(next_quter_index < iter->rotation.size());

	float delta_time = static_cast<float>(iter->rotation[next_quter_index].time - iter->rotation[quter_index].time);
	float factor = (time - static_cast<float>(iter->rotation[quter_index].time)) / delta_time;
	if (factor < 0.0f)
	{
		return iter->rotation.front().quater;
	}

	auto start = iter->rotation[quter_index].quater;
	auto end = iter->rotation[next_quter_index].quater;

	start = Quaternion::Slerp(start, end, factor);
	return start;
}
Vector3 Interpolated::CalcInterpolatedPosition(AnimNode* iter, float time, int Duration)
{
	//auto pos_frames = iter->second->position;

	if (iter->position.empty())
		return Vector3(0.0f, 0.0f, 0.0f);

	if (iter->position.size() == 1)
		return iter->position.front().pos;

	if (time == Duration - 1)
	{
		return iter->position.back().pos;
	}

	int position_index = FindPos(iter, time);

	if (position_index == -1)
	{
		return iter->position.back().pos;
	}

	UINT next_position_index = position_index + 1;
	assert(next_position_index < iter->position.size());

	float delta_time = static_cast<float>(iter->position[next_position_index].time - iter->position[position_index].time);
	float factor = (time - static_cast<float>(iter->position[position_index].time)) / delta_time;

	if (factor < 0.0f)
	{
		return iter->position.front().pos;
	}

	auto start = iter->position[position_index].pos;
	auto end = iter->position[next_position_index].pos;

	start = Vector3::Lerp(start, end, factor);
	return start;
}


int Interpolated::FindScale(AnimNode* iter, float time)
{
	//vector<AnimScale> scale_frames = iter->second->scale;

	if (iter->scale.empty())
		return -1;//-1 을 반환하면 오류로 터지게 됨

	for (UINT i = 0; i < iter->scale.size() - 1; i++)
	{
		if (time < (float)(iter->scale[i + 1].time))
			return i;
	}

	return -1;//-1 을 반환하면 오류로 터지게 됨
}
int Interpolated::FindRot(AnimNode* iter, float time)
{
	//auto pos_frames = iter->second->rotation;

	if (iter->rotation.empty())
		return -1;

	for (UINT i = 0; i < iter->rotation.size() - 1; i++)
	{
		if (time < static_cast<float>(iter->rotation[i + 1].time))
			return i;
	}

	return -1;
}
int Interpolated::FindPos(AnimNode* iter, float time)
{
	//auto pos_frames = iter->second->position;

	if (iter->position.empty())
		return -1;

	for (UINT i = 0; i < iter->position.size() - 1; i++)
	{
		if (time < static_cast<float>(iter->position[i + 1].time))
			return i;
	}

	return -1;
}

