#pragma once

struct LoadingManager
{
    friend class SceneManager;
    friend class LoadingScene;
    friend class Window;
public:
    enum class LoadingType
    {
        NONE,
        COMPONENTS,
        LOAD_SC1
    };
private:
    thread* th_loading;     // 스레드 객체
    thread::id thread_id;   // 현 스레드ID
    LoadingType type = LoadingType::NONE;   // 로딩중인 키 타입
    function<void()> func_calculate;    // 용량을 계산하기위해 등록된 함수
    function<void()> func_load;         // 내용을 로드하기위해 등록된 함수
    bool b_started = false;     // 로딩 시작유무
    bool b_finished = false;    // 로딩 완료유무
};

class SceneManager :public Singleton<SceneManager>
{
public:
    enum class SceneKey
    {
        LOADING,
        SC1
    };
private:
    map<SceneKey, Scene*> scenes;
    Scene* currentScene = nullptr;
    Scene* nextScene = nullptr;
    bool   isChanging = false;
    float  changingTime = 0.0f;
public:
    class Shadow* shadow = nullptr;
    class PostEffect* postEffect = nullptr;
    struct LoadingManager loadingSC;
    vector<class Terrain*>* Map;
    class Camera* currentCam;

    SceneManager();
    ~SceneManager();
    //씬 추가
    bool    AddScene(SceneKey key, Scene* value);
    //씬 삭제
    bool    DeleteScene(SceneKey key);
    //씬 변경
    Scene* ChangeScene(SceneKey key, float changingTime = 0.0f);
    //씬 가져오기
    Scene* GetScene(SceneKey key);
    //현재 씬 가져오기
    Scene* GetCurrentScene();
    bool GetIsChaning() { return isChanging; }
    float   gravity = 98.0f;
    int     checkPoint = 0;

    // 로딩씬 스레드용 함수 (사용 시 로딩씬으로 넘어간다)
    void    SetLoadingScene(LoadingManager::LoadingType type, function<void()> func_calculate, function<void()> func_load);

    // 씬 관련 함수
    void    Release();
    void    Update();
    void    LateUpdate();
    void    DebugRender();
    void    PreRender();
    void    Render();
    void    ResizeScreen();
};