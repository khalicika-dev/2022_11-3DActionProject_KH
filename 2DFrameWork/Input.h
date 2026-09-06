#pragma once


class Input : public Singleton<Input>
{
    friend class Window;
private:
    unique_ptr<Mouse> gameMouse;
    unique_ptr<GamePad> gamePad;  // 게임패드
    bool gamePadConnected;

    unsigned char   keyState[256];
    unsigned char   keyOldState[256];
    unsigned char   keyMap[256];
    map<int, unsigned char> padMap;
    float           pressTime;      // KeyDelayPress함수용 입력딜레이 시간
    bool            delayPress;    // 길게 눌러서 지연눌림을 발생시켰는가?
    bool            pauseSignal;    // 일시정지 신호 보내기
    Vector3         wheelStatus;
    Vector3         wheelOldStatus;
    Vector3         oldPostion;
    Vector3         currentPostion;

    //익명 enum
    enum 
    {
        KEY_INPUT_STATUS_NONE = 0,
        KEY_INPUT_STATUS_PRESS = 1,
        KEY_INPUT_STATUS_UP = 2,
        KEY_INPUT_STATUS_DOWN = 3,
    };
public:
    Vector3 position;       //마우스 위치
    Vector3 NDCPosition;
    Vector3 movePosition;   //마우스 이동벡터
    Vector3 wheelMoveValue;
    Vector2 relPosition;    //마우스 상대입력좌표 (이동벡터와 값이 많이 다름)

    GamePad::ButtonStateTracker tracker; // 게임패드의 특정 버튼에 대한 눌림상태를 보관
    Mouse::ButtonStateTracker   mouseTracker;
    bool usePad;    // 패드입력이 있었는가?
    //float vib_LMotor;
    //float vib_RMotor;
    //float vib_LTrigger;
    //float vib_RTrigger;
    float vib_InitPower;
    float vib_curPower;
    float vib_curFadeTime;
    float vib_maxFadeTime;
    float vib_time;
public:
    Input();
    ~Input();

    bool KeyDown(int KeyCode, bool usePad = true); //눌렀을때 최초1회
    bool KeyPress(int KeyCode, bool usePad = true); //누르고있을때
    bool KeyDelayPress(int KeyCode, bool usePad = true); // 길게 누르고있을때
    bool PadDelayPress(GamePad::ButtonStateTracker::ButtonState trackerState); // 길게 누르고있을때(패드한정)
    bool KeyUp(int KeyCode, bool usePad = true); //눌렀다가 떼었을때 최초1회
    void ResetVibration() { vib_InitPower = vib_curPower = 0.0f; } // 패드진동 초기화
    void SetVibration(float power, float time, float fadeTime);
    void UpdateVibration();
    unsigned char GetPadState_Two(GamePad::ButtonStateTracker::ButtonState first, GamePad::ButtonStateTracker::ButtonState second);
    void SetMouseMode(Mouse::Mode mode) { gameMouse->SetMode(mode); }
    void TogglePuase() { pauseSignal = true; }

    LRESULT InputProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
    void Update();
};

