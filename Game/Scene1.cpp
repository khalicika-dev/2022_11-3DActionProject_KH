#include "stdafx.h"

Scene1::Scene1()
{
    sky = Sky::Create();
    //sky->rotation.y = PI;
    Grid = Actor::Create();
    Grid->LoadFile("Grid.xml");

    envWater = new Environment(2048, 2048);
    envWater->desc2.CubeMapType = 2;
    envWaterFall = new Environment(2048, 2048);
    envWaterFall->desc2.CubeMapType = 2;
    //envMirror = new Environment(2048, 2048);
    //envMirror->desc2.CubeMapType = 0;
    envWall = new Environment(2048, 2048);
    envWall->desc2.CubeMapType = 2;
    envWall->desc2.WaterIndex = 0.025f;
}

Scene1::~Scene1()
{
    Release();
    sky->Release();
    Grid->Release();
    SafeDelete(envWater);
    SafeDelete(envWaterFall);
    SafeDelete(envWall);
    //SafeDelete(envMirror);
}

void Scene1::Calculate_Init()
{
    RESOURCE->IncreaseXmlList_AddTotalCapacity("Cam.xml");
    // n번째 맵 계산
    for (int i = 0; i < 9; i++)
    {
        RESOURCE->IncreaseXmlList_AddTotalCapacity("Terrain" + to_string(i) + ".xml");
    }
    //------
    GAMEOBJ->Calculate(GOTYPE::MONSTER_GOBLIN, 0);

    for (int i = 1; i <= 7; i++)
    {
        GAMEOBJ->Calculate(GOTYPE::MONSTER_GOBLIN, i);
    }
    for (int i = 0; i < 11; i++)
    {
        GAMEOBJ->Calculate(GOTYPE::MONSTER_GOBLIN, i + 8);
    }
    for (int i = 0; i <= 1; i++)
    {
        GAMEOBJ->Calculate(GOTYPE::MONSTER_DRONE, i);
    }
    for (int i = 0; i < 7; i++)
    {
        GAMEOBJ->Calculate(GOTYPE::MONSTER_DRONE, i + 2);
    }

    GAMEOBJ->Calculate(GOTYPE::MONSTER_GIANT, 0);
}

void Scene1::Init()
{
    sceneTime = 0.0f;
    Cam = Camera::Create();
    Cam->LoadFile("Cam.xml");

    soundBoard.clear();
    for (int i = 0; i < SoundCondition::ALL_COUNTS; i++)
        soundBoard.push_back(false);

    // n번째 맵 로드
    for (int i = 0; i < 9; i++)
    {
        Terrain* temp = Terrain::Create();
        temp->LoadFile("Terrain" + to_string(i) + ".xml");
        if (App.GetAppQuit())
            return;

        temp->CreateStructuredBuffer();
        temp->material->shadow = 1.0f;
        temp->Update();
        terrainMap.push_back(temp);
    }
    
    GAMEOBJ->Prepare(GOTYPE::MONSTER_GOBLIN, 0);
    GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GOBLIN, 0)->Spawn(terrainMap[0]->Find("Spawn0"));
    GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GOBLIN, 0)->Appear(true);
    
    for (int i = 1; i <= 7; i++)
    {
        GAMEOBJ->Prepare(GOTYPE::MONSTER_GOBLIN, i);
        GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GOBLIN, i)->Spawn(terrainMap[0]->Find("Spawn" + to_string(i)));
    }
    for (int i = 0; i < 11; i++)
        GAMEOBJ->Prepare(GOTYPE::MONSTER_GOBLIN, i + 8);
    
    for (int i = 0; i <= 1; i++)
    {
        GAMEOBJ->Prepare(GOTYPE::MONSTER_DRONE, i);
        GAMEOBJ->Find_Mob(GOTYPE::MONSTER_DRONE, i)->Spawn(terrainMap[0]->Find("Spawn" + to_string(i + 8)));
    }
    for (int i = 0; i < 7; i++)
        GAMEOBJ->Prepare(GOTYPE::MONSTER_DRONE, i + 2);

    GAMEOBJ->Prepare(GOTYPE::MONSTER_GIANT, 0);
    //GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GIANT, 0)->Spawn(terrainMap[0]->Find("BossSpawn"));
    //GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GIANT, 0)->Appear(true);

    SCENE->checkPoint = 0;

    // 플레이어와 인터페이스의 상태 리셋
    GAMEOBJ->Prepare(GObjectManager::GOType::PLAYER, 0);
    GET_PLAYER->Spawn(terrainMap[0]->Find("Title_PlayerSpawn0"));
    Cam->SetWorldPos(terrainMap[0]->Find("Title_CameraSpawn0")->GetWorldPos());
    Cam->rotation = terrainMap[0]->Find("Title_CameraSpawn0")->rotation;
    PLAYER_UI->Reset();

    bCapture_envWaterFall = false;
    bCapture_envWall = false;
    ResizeScreen();
}

void Scene1::Release()
{
    Cam->Release();
    for(auto m: terrainMap)
        m->Release();
    terrainMap.clear();
    SOUND->StopAllKeySound(SDKEY::wind00);
}


void Scene1::Update()
{
    Cam->Update();
    Grid->Update();

    GAMEOBJ->Update(GOTYPE::PLAYER,0);
    for (int i = 0; i <= 7; i++)
        GAMEOBJ->Update(GOTYPE::MONSTER_GOBLIN, i);
    for (int i = 0; i < 11; i++)
        GAMEOBJ->Update(GOTYPE::MONSTER_GOBLIN, i + 8);

    for (int i = 0; i <= 1; i++)
        GAMEOBJ->Update(GOTYPE::MONSTER_DRONE, i);
    for (int i = 0; i < 7; i++)
        GAMEOBJ->Update(GOTYPE::MONSTER_DRONE, i + 2);

    GAMEOBJ->Update(GOTYPE::MONSTER_GIANT, 0);

    for (auto m : terrainMap)
        m->Update();
    sky->Update();
    PLAYER_UI->Update();
}

void Scene1::LateUpdate()
{
    GAMEOBJ->LateUpdate(GOTYPE::PLAYER, 0);

    for (int i = 0; i <= 7; i++)
        GAMEOBJ->LateUpdate(GOTYPE::MONSTER_GOBLIN, i);
    for (int i = 0; i < 11; i++)
        GAMEOBJ->LateUpdate(GOTYPE::MONSTER_GOBLIN, i + 8);

    for (int i = 0; i <= 1; i++)
        GAMEOBJ->LateUpdate(GOTYPE::MONSTER_DRONE, i);
    for (int i = 0; i < 7; i++)
        GAMEOBJ->LateUpdate(GOTYPE::MONSTER_DRONE, i + 2);

    GAMEOBJ->LateUpdate(GOTYPE::MONSTER_GIANT, 0);
    PLAYER_UI->LateUpdate();

    // 이 맵의 환경음 재생
    SOUND->Play(SDKEY::wind00, soundBoard, SPACE1, SDTYPE::ENV, 0.3f);

    // 카메라 시점 전환
    if (PLAYER_UI->GetGameStart() < Player_Interface::EnumGameStartState::STATE_START_GAME)
    {
        CameraControl::SetCamState(Camera::CameraState::SCENE_CAM);
        if (Camera::GetMainCam() != Cam) Camera::SetMainCam(Cam);
    }

    // 게임 시작 시의 이벤트
    if (PLAYER_UI->GetGameStart() == Player_Interface::EnumGameStartState::STATE_START_GAME)
    {
        SOUND->Play(SDKEY::BGM00, soundBoard, SPACE0, SDTYPE::BGM, 0.2f);
        //SetCheckPoint(2);
        GET_PLAYER->Start(terrainMap[0]->Find("CPoint" + to_string(SCENE->checkPoint)));
    }

    // 게임클리어 조건
    PLAYER_UI->GameClear(GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GIANT, 0)->GetHpRatio() <= 0.0f && PLAYER_UI->GetDying() <= 0);

    // EventCol1
    if (GET_PLAYER->GetActor()->Find("MoveCollider")->collider->Intersect(terrainMap[0]->Find("EventCol1")->collider))
    {
        for (int i = 1; i <= 7; i++)
        {
            GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GOBLIN, i)->Appear(false, true);
        }
        terrainMap[0]->Find("EventCol1")->collider->enable = false;
    }

    // EventCol2
    if (GET_PLAYER->GetActor()->Find("MoveCollider")->collider->Intersect(terrainMap[0]->Find("EventCol2")->collider))
    {
        for (int i = 0; i <= 1; i++)
        {
            GAMEOBJ->Find_Mob(GOTYPE::MONSTER_DRONE, i)->Appear(false, true);
        }
        terrainMap[0]->Find("EventCol2")->collider->enable = false;
    }

    // EventCol3
    if (GET_PLAYER->GetHpRatio() > 0.0f && GET_PLAYER->GetActor()->Find("MoveCollider")->collider->Intersect(terrainMap[0]->Find("EventCol3")->collider))
    {
        PLAYER_UI->OpenCinematic(1);
        terrainMap[0]->Find("EventCol3")->collider->enable = false;
    }

    switch (SCENE->checkPoint)
    {
    case 0:
    {
        bool isOpen = true;
        for (int i = 1; i <= 7; i++)
        {
            if (!GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GOBLIN, i)->IsKilled())
            {
                isOpen = false;
                break;
            }
        }
        if (isOpen)
            SetCheckPoint(1);
        break;
    }
    case 1:
    {
        bool isOpen = true;
        for (int i = 0; i <= 1; i++)
        {
            if (!GAMEOBJ->Find_Mob(GOTYPE::MONSTER_DRONE, i)->IsKilled())
            {
                isOpen = false;
                break;
            }
        }
        if (isOpen)
            SetCheckPoint(2);
        break;
    }
    }
}

void Scene1::DebugRender()
{
    ImGui::SliderFloat("App.deltaScale", &App.deltaScale, 0.001, 10);

    //Camera::ControlMainCam();
    LIGHT->RenderDetail();
    SCENE->shadow->RenderDetail();
    //envWaterFall->RenderDetail();
    //envMirror->RenderDetail();
    envWall->RenderDetail();
    SCENE->postEffect->RenderDetail();
    GET_PLAYER->RenderDetail();
    dynamic_cast<Monster_Giant*>(GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GIANT, 0))->RenderDetail();

    ImGui::Checkbox("camDebug", &camdebug);
    ImGui::Checkbox("PlayerLock", &VAR->playLock);
    if (camdebug)
    {
        CameraControl::SetCamState(Camera::CameraState::SCENE_CAM);
        SCENE->currentCam = Cam;
        Camera::SetMainCam(Cam);
        Camera::ControlMainCam();
    }
    ImGui::Text("bCapture_envWaterFall: %s", bCapture_envWaterFall ? "true" : "false");

    //
    ////Ui->RenderHierarchy();
    //
    ImGui::Begin("Hierarchy");
    sky->RenderHierarchy();
    GET_PLAYER->GetActor()->RenderHierarchy();
    //GAMEOBJ->Find(GOTYPE::MONSTER_GOBLIN, 0)->GetActor()->RenderHierarchy();
    //GAMEOBJ->Find(GOTYPE::MONSTER_DRONE, 0)->GetActor()->RenderHierarchy();
    GAMEOBJ->Find(GOTYPE::MONSTER_GIANT, 0)->GetActor()->RenderHierarchy();
    for (auto m : terrainMap)
    {
        m->RenderHierarchy();
    }
    //Cam->RenderHierarchy();
    GET_PLAYER->GetCamPivot()->RenderHierarchy();
    //GAMEOBJ->
    Camera::GetFollowCam()->root->RenderHierarchy();
    Cam->RenderHierarchy();
    ImGui::End();
}

// PreRender: 포스트이펙트(렌더타겟) 텍스쳐에 그리기, 일반 객체 렌더
void Scene1::PreRender()
{
    LIGHT->Set();

    bCapture_envWall = false;
    bCapture_envWaterFall = false;
    Object* ob_ev1 = terrainMap[0]->Find("EventWall1");
    Object* ob_ev2 = terrainMap[0]->Find("EventWall2");
    if ((ob_ev1 && ob_ev1->IsShown()) || (ob_ev2 && ob_ev2->IsShown()))
        bCapture_envWall = true;
    for (auto m : terrainMap)
    {
        Object* ob;
        for (int i = 1; ob = m->Find("WaterFall" + to_string(i)); i++)
        {
            if (ob->IsShown())
            {
                bCapture_envWaterFall = true;
                break;
            }
        }
        if (bCapture_envWaterFall) 
            break;
    }

    //환경 매핑 그리기(물)
    {
        // 하늘, 지형만 그림
        envWater->rot.z = Util::NormalizeAngle(envWater->rot.z - 5.0f * TORADIAN * DELTA);
        envWater->SetCapture(SCENE->currentCam->GetWorldPos(), 0.0f, 1.57f, 0.1f, 2500.0f);
        sky->CubeMapRender();
        for (auto m : terrainMap)
            m->CubeMapRender();
        //AllObjectRender("CubeMap");
    }

    //환경 매핑 그리기(폭포)
    if(bCapture_envWaterFall)
    {
        // 하늘, 지형만 그림
        envWaterFall->rot.z = Util::NormalizeAngle(envWaterFall->rot.z - 20.0f * TORADIAN * DELTA);
        envWaterFall->SetCapture(SCENE->currentCam->GetWorldPos(), 0.0f, 1.57f, 0.1f, 2500.0f);
        sky->CubeMapRender();
        terrainMap[0]->CubeMapRender();
        terrainMap[1]->CubeMapRender();
        //AllObjectRender("CubeMap");
    }

    //환경 매핑 그리기(벽)
    if(bCapture_envWall)
    {
        envWall->rot.z = Util::NormalizeAngle(envWall->rot.z - 10.0f * TORADIAN * DELTA);
        envWall->SetCapture(SCENE->currentCam->GetWorldPos(), 0.0f, 1.57f, 0.1f, 2500.0f);
        AllObjectRender("CubeMap");
    }

    ////환경 매핑 그리기(거울)
    //{
    //    Vector3 Pos = ob_ev1->GetWorldPos()/* + ob_ev1->GetForward() * fabs(ob_ev1->S._33)*/;
    //    Vector3 Dir = Pos - SCENE->currentCam->GetWorldPos();
    //    float Distance = Dir.Length();
    //    Dir.Normalize();
    //    Vector3 reflect = Vector3::Reflect(Dir, ob_ev1->GetForward());
    //    ImGui::Text("mirrorPos: (%f, %f, %f)", Pos.x, Pos.y, Pos.z);
    //    ImGui::Text("Dir: (%f, %f, %f)", Dir.x, Dir.y, Dir.z);
    //    ImGui::Text("reflect: (%f, %f, %f)", reflect.x, reflect.y, reflect.z);
    //    envMirror->rot.z = Util::NormalizeAngle(envWater->rot.z - 5.0f * TORADIAN * DELTA);
    //    envMirror->GetCurrentCapture().reflectNormal = reflect;
    //    Vector3 result = Pos - reflect * Distance;
    //    float minDist = (SCENE->currentCam->GetWorldPos() - result).Length() / 2.0f;    // 투영되는 물체에서 투영매개체까지의 최소거리 
    //    envMirror->SetCapture(result, minDist, 1.57f, 0.1f, 2500.0f);
    //    ImGui::Text("result: (%f, %f, %f)", result.x, result.y, result.z);
    //    AllObjectRender("CubeMap");
    //}

    //그림자 텍스쳐 그리기
    {
        SCENE->shadow->SetCapture(GET_PLAYER->GetActor()->GetWorldPos());
        AllObjectRender("Shadow");
    }

    //포스트이펙트 텍스쳐에 그리기
    {
        SCENE->postEffect->SetCapture();
        SCENE->currentCam->Set();
        sky->Render();

        envWater->SetTexture();
        for (auto m : terrainMap)
        {
            Object* ob;
            for (int i = 1; ob = m->Find("Water" + to_string(i)); i++)
            {
                ob->Render();
            }
        }

        if (bCapture_envWaterFall)
        {
            envWaterFall->SetTexture();
            for (auto m : terrainMap)
            {
                Object* ob;
                for (int i = 1; ob = m->Find("WaterFall" + to_string(i)); i++)
                {
                    ob->Render();
                }
            }
        }

        if (bCapture_envWall)
        {
            //envMirror->SetTexture();
            envWall->SetTexture();
            ob_ev1->Render();
            ob_ev2->Render();
        }

        SCENE->shadow->SetTexture();
        AllObjectRender();
    }
}

// UI등 렌더타겟에 영향안받는 객체 렌더
void Scene1::Render()
{
    DEPTH->Set(false);
    //포스트이펙트 렌더
    SCENE->postEffect->Render();
    PLAYER_UI->Render();
    DEPTH->Set(true);
}

void Scene1::ResizeScreen()
{
    if (!Camera::GetFollowCam() || !Camera::GetMainCam())
        return;
    //Cam->width = App.GetWidth();
    //Cam->height = App.GetHeight();
    //Cam->viewport.width = App.GetWidth();
    //Cam->viewport.height = App.GetHeight();

    Camera::GetFollowCam()->width = App.GetWidth();
    Camera::GetFollowCam()->height = App.GetHeight();
    Camera::GetFollowCam()->viewport.width = App.GetWidth();
    Camera::GetFollowCam()->viewport.height = App.GetHeight();

    Camera::GetMainCam()->width = App.GetWidth();
    Camera::GetMainCam()->height = App.GetHeight();
    Camera::GetMainCam()->viewport.width = App.GetWidth();
    Camera::GetMainCam()->viewport.height = App.GetHeight();
}

void Scene1::AllObjectRender(string type)
{
    if (type == "CubeMap")
    {
        sky->CubeMapRender();
        for (auto m : terrainMap)
            m->CubeMapRender();
        for (int i = 0; i <= 7; i++) GAMEOBJ->CubeMapRender(GOTYPE::MONSTER_GOBLIN, i);
        for (int i = 0; i < 11; i++) GAMEOBJ->CubeMapRender(GOTYPE::MONSTER_GOBLIN, i + 8);

        for (int i = 0; i <= 1; i++) GAMEOBJ->CubeMapRender(GOTYPE::MONSTER_DRONE, i);
        for (int i = 0; i < 7; i++) GAMEOBJ->CubeMapRender(GOTYPE::MONSTER_DRONE, i + 2);
        GAMEOBJ->CubeMapRender(GOTYPE::MONSTER_GIANT, 0);
        GAMEOBJ->CubeMapRender(GOTYPE::PLAYER, 0);
        for (auto it = GAMEOBJ->m_particles.begin(); it != GAMEOBJ->m_particles.end(); it++)
        {
            if (it->second->Enable) it->second->CubeMapRender();
        }
    }
    else if (type == "Shadow")
    {
        // sky, Terrain 제외
        for (int i = 0; i <= 7; i++) GAMEOBJ->ShadowMapRender(GOTYPE::MONSTER_GOBLIN, i);
        for (int i = 0; i < 11; i++) GAMEOBJ->ShadowMapRender(GOTYPE::MONSTER_GOBLIN, i + 8);

        for (int i = 0; i <= 1; i++) GAMEOBJ->ShadowMapRender(GOTYPE::MONSTER_DRONE, i);
        for (int i = 0; i < 7; i++) GAMEOBJ->ShadowMapRender(GOTYPE::MONSTER_DRONE, i + 2);
        GAMEOBJ->ShadowMapRender(GOTYPE::MONSTER_GIANT, 0);
        GAMEOBJ->ShadowMapRender(GOTYPE::PLAYER, 0);
        for (auto it = GAMEOBJ->m_particles.begin(); it != GAMEOBJ->m_particles.end(); it++)
        {
            if (it->second->Enable) it->second->ShadowMapRender();
        }
    }
    else if (type == "Normal")
    {
        // sky 제외
        for (auto m : terrainMap)
            m->Render();
        for (int i = 0; i <= 7; i++) GAMEOBJ->Render(GOTYPE::MONSTER_GOBLIN, i);
        for (int i = 0; i < 11; i++) GAMEOBJ->Render(GOTYPE::MONSTER_GOBLIN, i + 8);

        for (int i = 0; i <= 1; i++) GAMEOBJ->Render(GOTYPE::MONSTER_DRONE, i);
        for (int i = 0; i < 7; i++) GAMEOBJ->Render(GOTYPE::MONSTER_DRONE, i + 2);
        GAMEOBJ->Render(GOTYPE::MONSTER_GIANT, 0);
        GAMEOBJ->Render(GOTYPE::PLAYER, 0);
    }
}

void Scene1::SetCheckPoint(int num)
{
    switch (num)
    {
    case 2:
        SOUND->Play(SDKEY::misc05, SDTYPE::SE, 1.0f, terrainMap[0]->Find("EventWall2")->GetWorldPos(), 100.0f);
        terrainMap[0]->Find("EventWall2")->visible = false;
        for (int i = 0; i <= 1; i++)
        {
            Monster* mob = GAMEOBJ->Find_Mob(GOTYPE::MONSTER_DRONE, i);
            if (!mob->IsKilled())
                mob->Enable = false;
        }
        terrainMap[0]->Find("EventCol2")->collider->enable = false;
        terrainMap[0]->Find("Col64")->collider->enable = false;
    case 1:
        SOUND->Play(SDKEY::misc05, SDTYPE::SE, 1.0f, terrainMap[0]->Find("EventWall1")->GetWorldPos(), 100.0f);
        terrainMap[0]->Find("EventWall1")->visible = false;
        for (int i = 0; i <= 7; i++)
        {
            Monster* mob = GAMEOBJ->Find_Mob(GOTYPE::MONSTER_GOBLIN, i);
            if(!mob->IsKilled())
                mob->Enable = false;
        }
        terrainMap[0]->Find("EventCol1")->collider->enable = false;
        terrainMap[0]->Find("Col32")->collider->enable = false;
    }
    if(SCENE->checkPoint != num)
        PLAYER_UI->PushMessage(InfoMsg::Subject::CheckPoint, InfoMsg::MSG::CheckUpdate);
    
    SCENE->checkPoint = num;
}
