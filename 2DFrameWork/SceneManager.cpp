#include "framework.h"

SceneManager::SceneManager()
{
    postEffect = new PostEffect();
    shadow = new Shadow();
    Map = nullptr;
}

SceneManager::~SceneManager()
{
    SafeDelete(shadow);
    SafeDelete(postEffect);
    for (auto i = scenes.begin();
        i != scenes.end(); i++)
    {
        SafeDelete(i->second);
    }
    scenes.clear();
    Map = nullptr;
}

bool SceneManager::AddScene(SceneKey key, Scene* value)
{

    auto iter = scenes.find(key);

    if (iter != scenes.end())
    {
        delete value;
        return false;
    }
    scenes[key] = value;

    return true;
}

bool SceneManager::DeleteScene(SceneKey key)
{
    auto iter = scenes.find(key);
    if (iter == scenes.end())
    {
        return false;
    }
    SafeDelete(iter->second);
    scenes.erase(iter);

    return true;

}

Scene* SceneManager::ChangeScene(SceneKey key, float changingTime)
{
    Scene* temp = GetScene(key);
    if (temp == currentScene)
        return temp;
    if (temp)
    {
        nextScene = temp;
        nextScene->sceneState = SceneState::FADEIN;
        this->changingTime = changingTime;
        isChanging = true;
        if (currentScene)
            currentScene->sceneState = SceneState::FADEOUT;
    }
    return temp;
}

Scene* SceneManager::GetScene(SceneKey key)
{
    auto iter = scenes.find(key);
    if (iter == scenes.end())
    {
        return nullptr;
    }
    return iter->second;
}

Scene* SceneManager::GetCurrentScene()
{
    return currentScene;
}

void SceneManager::SetLoadingScene(LoadingManager::LoadingType type, function<void()> func_calculate, function<void()> func_load)
{
    loadingSC.thread_id = thread::id();
    loadingSC.type = type;
    loadingSC.func_calculate = func_calculate;
    loadingSC.func_load = func_load;
    loadingSC.b_finished = false;
    loadingSC.b_started = false;

    SCENE->ChangeScene(SCENEKEY::LOADING)->Init();
}

void SceneManager::Release()
{
    if (currentScene)currentScene->Release();
}

void SceneManager::Update()
{
    if (isChanging)
    {
        changingTime -= DELTA;
        if (changingTime <= 0.0f)
        {
            if (currentScene)
                currentScene->Release();
            Map = &nextScene->terrainMap;
            //nextScene->Init();

            currentScene = nextScene;
            isChanging = false;
        }
    }

    currentScene->Update();
}

void SceneManager::LateUpdate()
{
    currentScene->LateUpdate();
}

void SceneManager::DebugRender()
{
    currentScene->DebugRender();
}

void SceneManager::PreRender()
{
    currentScene->PreRender();
}

void SceneManager::Render()
{
    currentScene->Render();
}

void SceneManager::ResizeScreen()
{
    if (postEffect)
    {
        postEffect->ResizeScreen(App.GetWidth(), App.GetHeight());
    }

    if (!currentScene) return;
    currentScene->ResizeScreen();
}