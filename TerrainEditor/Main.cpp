#include "stdafx.h"
#include "Main.h"

Main::Main()
{
    D3D11_BUFFER_DESC desc = { 0 };
    desc.ByteWidth = sizeof(Brush);
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//상수버퍼
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, NULL, &brushBuffer);
    assert(SUCCEEDED(hr));
    D3D->GetDC()->PSSetConstantBuffers(10, 1, &brushBuffer);

    Transform::CreateStaticMember();
    Camera::CreateStaticMember();
    Material::CreateStaticMember();
    PostEffect::CreateStaticMember();
    Object::CreateStaticMember();
    Skeleton::CreateStaticMember();
    Terrain::CreateStaticMember();
    Environment::CreateStaticMember();
    Rain::CreateStaticMember();
    Pop::CreateStaticMember();
    Shadow::CreateStaticMember();
}

Main::~Main()
{

}

void Main::Init()
{
    Cam = Camera::Create();
    Cam->LoadFile("Cam.xml");
    Camera::SetMainCam(Cam);
    Grid = Actor::Create();
    Grid->LoadFile("Grid.xml");

    vertexTarget = Actor::Create();

    terrainMap.push_back(Terrain::Create());
    terrainMap.back()->shader = RESOURCE->shaders.Load("5.TerrainEditor.hlsl");
    terrainMap.back()->CreateStructuredBuffer();
    //Map[0]->LoadFile("Terrain0.xml");

    undo_count = 0;

    Sphere = Actor::Create();
    Sphere->LoadFile("Claris.xml");
    Sphere2 = Actor::Create();
    Sphere2->LoadFile("Giant.xml");

    ResizeScreen();
}

void Main::Release()
{
    RESOURCE->ReleaseAll();
    Grid->Release();
    for (auto m : terrainMap) m->Release();
    Sphere->Release();

    //
    if (ResourceManager::isHaveInstance()) RESOURCE->ReleaseAll();

    Transform::DeleteStaticMember();
    Object::DeleteStaticMember();
    Camera::DeleteStaticMember();
    Material::DeleteStaticMember();
    Skeleton::DeleteStaticMember();
    Terrain::DeleteStaticMember();
    Environment::DeleteStaticMember();
    Rain::DeleteStaticMember();
    Pop::DeleteStaticMember();
    Shadow::DeleteStaticMember();
    PostEffect::DeleteStaticMember();

    TIMER->DeleteSingleton();
    INPUT->DeleteSingleton();
    GUI->DeleteSingleton();
    RANDOM->DeleteSingleton();
    DEPTH->DeleteSingleton();
    DWRITE->DeleteSingleton();
    SOUND->DeleteSingleton();
    SCENE->DeleteSingleton();
    RESOURCE->DeleteSingleton();
}


void Main::Update()
{
    Camera::ControlMainCam(50.0f);
    LIGHT->RenderDetail();
    //for (auto m : Map) m.second->RenderDetail();


    ImGui::Text("FPS: %d", TIMER->GetFramePerSecond());
    if (ImGui::Checkbox("ModifyVertex", &isWireframe))
    {
        if (!isWireframe)
            GUI->target = nullptr;
        selectVertex = nullptr;
        targetTerrain = nullptr;
    }
    ImGui::NewLine();
    ImGui::Checkbox("Mouse", &MouseBrush);
    if (!MouseBrush)
    {
        float p[2] = { brush.point.x,brush.point.z };
        ImGui::DragFloat2("position", p, 0.3f);
        brush.point.x = p[0];
        brush.point.z = p[1];
    }
    ImGui::SliderInt("BrushTexture", &brushTexture, 0, 1);
    ImGui::DragFloat("TextureFillSpeed", &TextureFillSpeed
        , 0.5f, 1.0f, 10.0f);
    ImGui::DragFloat("BrushRange", &brush.range
        , 5.0f, 0.0f, 2000.0f);
    ImGui::InputFloat2("BrushHeight", &brushMinMaxHeight[0]);
    ImGui::InputFloat("BrushAddHeightScalr", &brushAddHeightScalr);
    if (ImGui::Button("Rect"))
    {
        brush.shape = 0;
    }
    ImGui::SameLine();
    if (ImGui::Button("Circle"))
    {
        brush.shape = 1;
    }

    if (ImGui::Button("1"))
    {
        brush.type = 0;
    }
    ImGui::SameLine();
    if (ImGui::Button("2"))
    {
        brush.type = 1;
    }
    ImGui::SameLine();
    if (ImGui::Button("3"))
    {
        brush.type = 2;
    }

    if (ImGui::Button("AddNode"))
    {
        nodeEdit = 0;
    }
    ImGui::SameLine();
    if (ImGui::Button("DeleteNode"))
    {
        nodeEdit = 1;
    }
    ImGui::SameLine();
    if (ImGui::Button("LinkNode"))
    {
        nodeEdit = 2;
        prevPick = -1;
    }
    ImGui::Text("State: %s", nodeEdit == 0 ? "Add" : nodeEdit == 1 ? "Delete" : prevPick == -1 ? "Link" : "Link(First Pick)");
    ImGui::Text("ObjPick: %s", ObjPick ? "true" : "false");

    ImGui::Begin("Hierarchy");
    Grid->RenderHierarchy();
    Cam->RenderHierarchy();
    for (auto m : terrainMap) m->RenderHierarchy();
    Sphere->RenderHierarchy();
    Sphere2->RenderHierarchy();
    ImGui::End();


    ImGui::Begin("AddTerrain");
    if (ImGui::Button("NewTerrain"))
    {
        terrainMap.push_back(Terrain::Create());
        terrainMap.back()->CreateStructuredBuffer();
        terrainMap.back()->shader = RESOURCE->shaders.Load("5.TerrainEditor.hlsl");

        // 새 지형 호출 시 실행취소 초기화
        for (int i = 0; i < terrainMap.size(); i++) tempVertices[i].clear();
        undo_count = 0;
    }
    if (GUI->FileImGui("LoadRawFile", "LoadRawFile",
        ".raw,.RAW", "../Contents/Raw"))
    {
        string path = ImGuiFileDialog::Instance()->GetCurrentPath();
        Util::Replace(&path, "\\", "/");
        if (path.find("/Raw/") != -1)
        {
            size_t tok = path.find("/Raw/") + 5;
            path = path.substr(tok, path.length())
                + "/" + ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        else
        {
            path = ImGuiFileDialog::Instance()->GetCurrentFileName();
        }
        string file = "../Contents/Raw/" + path;
        FILE* fp;
        //파일 열기
        fopen_s(&fp, file.c_str(), "rb");
        fseek(fp, 0, SEEK_END);
        int Size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        BYTE* Height = new BYTE[Size];

        for (int i = 0; i < Size; i++)
        {
            char c = fgetc(fp);
            Height[i] = c;
            //cout << (int)Height[i] << endl;
        }

        int terrainSize = (int)sqrt(Size);
        int triSize = (terrainSize - 1) * (terrainSize - 1) * 2;
        float half = terrainSize * 0.5f;

        VertexTerrain* vertices = new VertexTerrain[Size];

        for (int z = 0; z < terrainSize; z++)
        {
            for (int x = 0; x < terrainSize; x++)
            {
                int Index = (z * terrainSize) + x;
                float _x = x - half;
                float _z = -z + half;
                float _y = Height[Index] / 255.0f * (float)terrainSize;

                float _u = float(x) / float(terrainSize - 1);
                float _v = float(z) / float(terrainSize - 1);

                vertices[Index].uv = Vector2(_u, _v);
                vertices[Index].position = Vector3(_x, _y, _z);
                vertices[Index].normal = Vector3(0, 1, 0);
            }
        }

        UINT* indices = new UINT[triSize * 3];
        int Idx = 0;
        for (int z = 0; z < terrainSize - 1; z++)
        {
            for (int x = 0; x < terrainSize - 1; x++)
            {
                UINT Index = z * terrainSize + x;
                indices[Idx] = Index;
                Idx++;
                indices[Idx] = Index + 1;
                Idx++;
                indices[Idx] = Index + terrainSize + 1;
                Idx++;
                indices[Idx] = Index;
                Idx++;
                indices[Idx] = Index + terrainSize + 1;
                Idx++;
                indices[Idx] = Index + terrainSize;
                Idx++;

            }
        }
        terrainMap.push_back(Terrain::Create());
        terrainMap.back()->mesh = make_shared<Mesh>(vertices, Size, indices, Idx,
            VertexType::TERRAIN);
        terrainMap.back()->shader = RESOURCE->shaders.Load("5.TerrainEditor.hlsl");
        terrainMap.back()->UpdateMeshNormal();
        terrainMap.back()->CreateStructuredBuffer();

        // 새 지형 호출 시 실행취소 초기화
        for (int i = 0; i < terrainMap.size(); i++) tempVertices[i].clear();
        undo_count = 0;



        //UINT* indices, UINT indexCount,
        //VertexType type





    //파일 닫기
        fclose(fp);

        //가로 x 세로 값으로 높이값 메쉬를 만들기
    }
    ImGui::End();

    Cam->Update();
    Grid->Update();
    vertexTarget->Update();
    for (auto m : terrainMap) m->Update();
    Sphere->Update();
    Sphere2->Update();
}

void Main::LateUpdate()
{
    Ray TerrainRay;
    Terrain* Picked = nullptr;    // 현재 가리키고있는 지점의 지형

    if (INPUT->movePosition.Length() > 0 || isWireframe)
    {
        if (MouseBrush || isWireframe)
        {
            TerrainRay = Util::MouseToRay(INPUT->position, Camera::GetMainCam());
            float dis = FLT_MAX;
            for (auto m : terrainMap)
            {
                if (m->ComPutePicking(TerrainRay, brush.point))
                {
                    if (Vector3::Distance(TerrainRay.position, brush.point) < dis)
                    {
                        dis = Vector3::Distance(TerrainRay.position, brush.point);
                        Picked = m;
                    }
                }
            }
        }
        else
        {
            TerrainRay.position = brush.point;
            TerrainRay.position.y = 2000.0f;
            TerrainRay.direction = { 0,-1,0 };
            for (auto m : terrainMap) m->ComPutePicking(TerrainRay, brush.point);
        }
    }

    //Sphere->SetWorldPos(Hit);
    if (isWireframe)
    {
        if (INPUT->KeyDown(VK_LBUTTON))
        {
            if (App.activeWindow)
            {
                if (Picked)
                {
                    float dis = FLT_MAX;
                    for (UINT i = 0; i < Picked->mesh->vertexCount; i++)
                    {
                        VertexTerrain* vertices = (VertexTerrain*)Picked->mesh->vertices;
                        Vector3 pos = vertices[i].position * Picked->scale + Picked->GetLocalPos();
                        if (Vector3::Distance(pos, brush.point) < dis)
                        {
                            dis = Vector3::Distance(pos, brush.point);
                            targetTerrain = Picked;
                            selectVertex = &vertices[i];
                            vertexTarget->SetLocalPos(selectVertex->position * Picked->scale + Picked->GetLocalPos());
                            GUI->target = vertexTarget;
                            lastTargetPos = vertexTarget->GetLocalPos() + Picked->GetLocalPos();
                        }
                    }
                }
                else
                {
                    GUI->target = nullptr;
                    selectVertex = nullptr;
                    targetTerrain = nullptr;
                }
            }
        }
    }

	if (INPUT->KeyDown(VK_F1))
	{
		for (int i = 0; i < terrainMap.size(); i++) TempTerrain(i);
		undo_count++;   // 실행취소 누적
	}
	if (INPUT->KeyPress(VK_F1))
	{
		for (auto m : terrainMap) EditTerrain(brush.point, m);
		for (auto m : terrainMap) m->DeleteStructuredBuffer();
		for (auto m : terrainMap) m->CreateStructuredBuffer();
	}
	if (INPUT->KeyUp(VK_F1))
	{
		for (auto m : terrainMap) m->UpdateMeshNormal();
	}

	if (!INPUT->KeyPress(VK_F1) && INPUT->KeyPress(VK_CONTROL) && INPUT->KeyDown('Z') && undo_count > 0)
	{
		for (int i = 0; i < terrainMap.size(); i++) UndoTerrain(i);
		undo_count--;
	}

    if (INPUT->KeyDown(VK_F2))
    {
        if (MouseBrush)
        {
            TerrainRay = Util::MouseToRay(INPUT->position, Camera::GetMainCam());
            for (auto m : terrainMap) m->ComPutePicking(TerrainRay, brush.point);
        }
        else
        {
            TerrainRay.position = brush.point;
            TerrainRay.position.y = 2000.0f;
            TerrainRay.direction = { 0,-1,0 };
            for (auto m : terrainMap) m->ComPutePicking(TerrainRay, brush.point);
        }
        for (auto m : terrainMap)
        {
            if (m->ComPutePicking(TerrainRay, brush.point))
            {
                if (nodeEdit == 0)
                {
                    m->AddNode(brush.point);
                }
                else if (nodeEdit == 1)
                {
                    m->PopNode(m->PickNode(brush.point));
                }
                else if (nodeEdit == 2)
                {
                    if (prevPick == -1)
                    {
                        prevPick = m->PickNode(brush.point);
                    }
                    else
                    {
                        m->LinkNode(m->PickNode(brush.point), prevPick);
                        prevPick = -1;
                    }
                }
            }
        }
    }
    if (INPUT->KeyDown(VK_F4))
    {
        if (MouseBrush)
        {
            TerrainRay = Util::MouseToRay(INPUT->position, Camera::GetMainCam());
            for (auto m : terrainMap) m->ComPutePicking(TerrainRay, brush.point);
        }
        else
        {
            TerrainRay.position = brush.point;
            TerrainRay.position.y = 2000.0f;
            TerrainRay.direction = { 0,-1,0 };
            for (auto m : terrainMap) m->ComPutePicking(TerrainRay, brush.point);
        }
        for (auto m : terrainMap)
        {
            if (m->ComPutePicking(TerrainRay, brush.point))
            {
                int Start = m->PickNode(Sphere->GetWorldPos());
                int End = m->PickNode(brush.point);

                // Way는 맨뒤가 Start, 맨앞이 End이다
                m->PathFinding(Way, Start, End);
                
                if (Way.size() <= 2)
                {
                    // Way 노드가 1~2개만 있다. (경유안함)
                    Way.clear();
                }
                else
                {
                    // Way 노드가 3개이상이면 첫 노드와 마지막 노드는 무시한다.
                    Way.pop_front();
                    Way.pop_back();
                }
                Way.push_front(brush.point);
                Way.push_front(Vector3(0, 0, 0));   // 실질적으로 안쓰는 값. 이곳을 목적지로 설정하고 Way목록에서 삭제하는순간 Way가 비어서 이동안함.
                P1 = Sphere->GetWorldPos();
                P2 = Way.back();
                Way.pop_back();
                MoveValue = 0.0f;
                Vector3 temp = P1 - P2;
                Dis = temp.Length();
                
                break;
            }
        }
    }
    if (INPUT->KeyDown(VK_F5))
    {
        for (auto m : terrainMap) m->DeleteStructuredBuffer();
        for (auto m : terrainMap) m->CreateStructuredBuffer();
        ObjPick = !ObjPick;
    }
    if (ObjPick)
    {
        if (GUI->target)
        {
            GUI->target->SetWorldPos(brush.point);
        }
        else
        {
            ObjPick = false;
        }
    }

    if (not Way.empty())
    {
        Sphere->SetWorldPos(Util::Lerp(P1, P2, MoveValue));

        MoveValue += DELTA / Dis * 100.0f;

        if (MoveValue > 1.0f)
        {
            MoveValue = 0.0f;
            P1 = P2;
            P2 = Way.back();
            Way.pop_back();
            Vector3 temp = P1 - P2;
            Dis = temp.Length();
        }
    }

    if (selectVertex)
    {
        if (vertexTarget->GetLocalPos() != lastTargetPos)
        {
            if (!modifying)
            {
                for (int i = 0; i < terrainMap.size(); i++) TempTerrain(i);
                undo_count++;   // 실행취소 누적
            }
            modifying = true;
            selectVertex->position = (vertexTarget->GetLocalPos() - targetTerrain->GetLocalPos()) / targetTerrain->scale;
            targetTerrain->mesh->UpdateMesh();
        }
    }
    if (modifying && INPUT->KeyUp(VK_LBUTTON))
    {
        for (auto m : terrainMap) m->DeleteStructuredBuffer();
        for (auto m : terrainMap) m->CreateStructuredBuffer();
        for (auto m : terrainMap) m->UpdateMeshNormal();
        modifying = false;
    }

    // 실행취소는 max_undo가 정한 횟수까지만 가능
    if (undo_count > max_undo)
    {
        for (int i = 0; i < terrainMap.size(); i++)
            tempVertices[i].pop_back();
        undo_count--;
    }
    lastTargetPos = vertexTarget->GetLocalPos();
}

void Main::Render()
{
    LIGHT->Set();
    Cam->Set();
    vertexTarget->Render();
    //Grid->Render();

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    D3D->GetDC()->Map(brushBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy_s(mappedResource.pData, sizeof(Brush), &brush, sizeof(Brush));
    D3D->GetDC()->Unmap(brushBuffer, 0);

    if (isWireframe) RASTER->Set(D3D11_CULL_BACK, D3D11_FILL_WIREFRAME);
    for (auto m : terrainMap) m->Render();
    RASTER->Set();
    Sphere->Render();
    Sphere2->Render();
}

void Main::ResizeScreen()
{
    Cam->width = App.GetWidth();
    Cam->height = App.GetHeight();
    Cam->viewport.width = App.GetWidth();
    Cam->viewport.height = App.GetHeight();
}

void Main::EditTerrain(Vector3 Pos, Terrain* map)
{
    if (!map->visible) return;
    Matrix Inverse = map->W.Invert();
    Pos = Vector3::Transform(Pos, Inverse);
    float brushRange = brush.range / map->S._11;

    //정점 갯수만큼
    for (UINT i = 0; i < map->mesh->vertexCount; i++)
    {
        VertexTerrain* vertices = (VertexTerrain*)map->mesh->vertices;
        Vector3 v1 = Vector3(Pos.x, 0.0f, Pos.z);
        Vector3 v2 = Vector3(vertices[i].position.x,
            0.0f, vertices[i].position.z);
        Vector3 temp = v2 - v1;
        //두포지션간의 xz 차이의 길이값
        float Dis = temp.Length();
        float w;
        //네모
        if (brush.shape == 0)
        {
            if (fabs(v1.x - v2.x) < brushRange and
                fabs(v1.z - v2.z) < brushRange)
            {
                if (brush.type == 0)
                {
                    w = 1.0f;
                }
                else if (brush.type == 1)
                {
                    // 0 ~ 1
                    w = Dis / brushRange / 1.414f;
                    Util::Saturate(w);
                    w = 1.0f - w;
                }
                else if (brush.type == 2)
                {
                    // 0 ~ 1
                    w = Dis / brushRange / 1.414f;
                    Util::Saturate(w);
                    w = 1.0f - w;
                    w *= PI * 0.5f;
                    w = sinf(w);
                }
            }
            else
            {
                w = 0;
            }
        }
        //원
        else if (brush.shape == 1)
        {
            w = Dis / brushRange;

            if (brush.type == 0)
            {
                Util::Saturate(w);
                w = 1.0f - w;
                if (w > 0.0f)
                    w = 1.0f;
            }
            else if (brush.type == 1)
            {
                Util::Saturate(w);
                w = 1.0f - w;
            }
            else if (brush.type == 2)
            {
                // 0 ~ 1
                w = Dis / brushRange;
                Util::Saturate(w);
                w = 1.0f - w;
                w *= PI * 0.5f;
                w = sinf(w);
            }
        }
        vertices[i].position.y
            += w * brushAddHeightScalr * DELTA;

        vertices[i].weights
            += w * ((brushTexture) ? 1 : -1) * TextureFillSpeed * DELTA;

        if (w > 0.0f)
        {
            Util::Saturate(vertices[i].weights);

            Util::Saturate(vertices[i].position.y, brushMinMaxHeight[0] - map->GetLocalPos().y, brushMinMaxHeight[1] - map->GetLocalPos().y);
        }

    }
    map->mesh->UpdateMesh();
}

void Main::TempTerrain(UINT num)
{
    vector<VertexTerrain> info;

    //정점 갯수만큼
    for (UINT i = 0; i < terrainMap[num]->mesh->vertexCount; i++)
    {
        VertexTerrain temp;
        VertexTerrain* vertices = (VertexTerrain*)terrainMap[num]->mesh->vertices;

        temp.normal = vertices[i].normal;
        temp.position = vertices[i].position;
        temp.uv = vertices[i].uv;
        temp.weights = vertices[i].weights;

        info.push_back(temp);

    }
    tempVertices[num].push_front(info);
}

void Main::UndoTerrain(UINT num)
{
    //정점 갯수만큼
    for (UINT i = 0; i < terrainMap[num]->mesh->vertexCount; i++)
    {
        VertexTerrain* vertices = (VertexTerrain*)terrainMap[num]->mesh->vertices;

        vertices[i].normal = tempVertices[num].front()[i].normal;
        vertices[i].position = tempVertices[num].front()[i].position;
        vertices[i].uv = tempVertices[num].front()[i].uv;
        vertices[i].weights = tempVertices[num].front()[i].weights;
    }
    terrainMap[num]->DeleteStructuredBuffer();
    terrainMap[num]->CreateStructuredBuffer();
    terrainMap[num]->mesh->UpdateMesh();
    tempVertices[num].pop_front();
}


int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR param, int command)
{
    App.SetAppName(L"TerrainEditor");
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