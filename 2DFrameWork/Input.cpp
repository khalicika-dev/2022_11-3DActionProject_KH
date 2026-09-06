#include "framework.h"

Input::Input()
{
	//시작주소로부터 어디까지 전부 0으로 초기화
	ZeroMemory(keyState, sizeof(keyState));
	ZeroMemory(keyOldState, sizeof(keyOldState));
	ZeroMemory(keyMap, sizeof(keyMap));


    position = Vector3(0, 0, 0);
    NDCPosition = Vector3(0, 0, 0);
    wheelStatus = Vector3(0.0f, 0.0f, 0.0f);
    wheelOldStatus = Vector3(0.0f, 0.0f, 0.0f);
    wheelMoveValue = Vector3(0.0f, 0.0f, 0.0f);
    pressTime = 0.0f;
    delayPress = false;
    pauseSignal = false;
    usePad = false;
    DWORD tLine = 0;
    SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &tLine, 0);

    gameMouse = make_unique<Mouse>();
    gamePad = make_unique<GamePad>();
    gamePadConnected = false;
    vib_InitPower = 0.0f;
    vib_curPower = 0.0f;
    vib_time = 0.0f;
    vib_curFadeTime = 0.0f;
    vib_maxFadeTime = 0.0f;
    gameMouse->SetVisible(false);
}

Input::~Input()
{
}

bool Input::KeyDown(int KeyCode, bool usePad)
{
    bool pad = usePad ? padMap[KeyCode] == KEY_INPUT_STATUS_DOWN : false;
    bool key = keyMap[KeyCode] == KEY_INPUT_STATUS_DOWN;
    return pad || key;
}

bool Input::KeyPress(int KeyCode, bool usePad)
{
    bool pad = usePad ? (padMap[KeyCode] == KEY_INPUT_STATUS_PRESS
        || padMap[KeyCode] == KEY_INPUT_STATUS_DOWN) : false;
    bool key = keyMap[KeyCode] == KEY_INPUT_STATUS_PRESS
        || keyMap[KeyCode] == KEY_INPUT_STATUS_DOWN;
    return pad || key;
}

bool Input::KeyDelayPress(int KeyCode, bool usePad)
{
    if ((usePad && padMap[KeyCode] == KEY_INPUT_STATUS_DOWN) || keyMap[KeyCode] == KEY_INPUT_STATUS_DOWN)
    {
        pressTime = 0.0f;
        return true;
    }
    bool pad = usePad ? padMap[KeyCode] == KEY_INPUT_STATUS_PRESS : false;
    bool key = keyMap[KeyCode] == KEY_INPUT_STATUS_PRESS;
    return delayPress && (pad || key);
}

bool Input::PadDelayPress(GamePad::ButtonStateTracker::ButtonState trackerState)
{
    if (trackerState == GamePad::ButtonStateTracker::ButtonState::PRESSED)
    {
        pressTime = 0.0f;
        return true;
    }
    return (delayPress && trackerState == GamePad::ButtonStateTracker::ButtonState::HELD);
}

bool Input::KeyUp(int KeyCode, bool usePad)
{
    bool pad = usePad ? padMap[KeyCode] == KEY_INPUT_STATUS_UP : false;
    bool key = keyMap[KeyCode] == KEY_INPUT_STATUS_UP;
    return pad || key;
}

void Input::SetVibration(float power, float time, float fadeTime)
{
    if (!VAR->enableVibration)
        return;

    vib_InitPower = power; 
    vib_time = time; 
    vib_curFadeTime = fadeTime;
    vib_maxFadeTime = fadeTime;
}

void Input::UpdateVibration()
{
    if (vib_time > 0)
    {
        vib_time -= DELTA_NPNS;
        vib_curPower = vib_InitPower;
    }
    else
    {
        vib_time = 0.0f;
        if (vib_curFadeTime > 0)
        {
            vib_curFadeTime -= DELTA_NPNS;
            vib_curPower = Util::Lerp(vib_InitPower, 0.0f, 1.0f - (vib_curFadeTime / vib_maxFadeTime));
        }
        else
        {
            vib_curFadeTime = 0.0f;
            vib_maxFadeTime = 0.0f;
            vib_InitPower = 0.0f;
            vib_curPower = 0.0f;
        }
    }
}

unsigned char Input::GetPadState_Two(GamePad::ButtonStateTracker::ButtonState first, GamePad::ButtonStateTracker::ButtonState second)
{
    if (first == GamePad::ButtonStateTracker::ButtonState::HELD && second == GamePad::ButtonStateTracker::ButtonState::HELD) 
        return KEY_INPUT_STATUS_PRESS;
    else if ((first == GamePad::ButtonStateTracker::ButtonState::HELD && second == GamePad::ButtonStateTracker::ButtonState::RELEASED) ||
        (first == GamePad::ButtonStateTracker::ButtonState::RELEASED && second == GamePad::ButtonStateTracker::ButtonState::HELD))
        return KEY_INPUT_STATUS_UP;
    else if ((first == GamePad::ButtonStateTracker::ButtonState::HELD && second == GamePad::ButtonStateTracker::ButtonState::PRESSED) ||
        (first == GamePad::ButtonStateTracker::ButtonState::PRESSED && second == GamePad::ButtonStateTracker::ButtonState::HELD))
        return KEY_INPUT_STATUS_DOWN;

    return KEY_INPUT_STATUS_NONE;
}

LRESULT Input::InputProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    gameMouse->SetWindow(App.handle);
    
    switch (message)
    {
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
    case WM_INPUT:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_MOUSEHOVER:
        Mouse::ProcessMessage(message, wParam, lParam);
        break;
    }

    if (message == WM_LBUTTONDOWN || message == WM_MOUSEMOVE)
    {
        position.x = (float)LOWORD(lParam);
        position.y = (float)HIWORD(lParam);
    }

    if (message == WM_MOUSEWHEEL)
    {
        short tWheelValue = (short)HIWORD(wParam);

        wheelOldStatus.z = wheelStatus.z;
        wheelStatus.z += (float)tWheelValue;
    }

    return TRUE;
}

void Input::Update()
{
    auto mouse = gameMouse->GetState();
    mouseTracker.Update(mouse);
    if (pauseSignal)
    {
        pauseSignal = false;
        VAR->isPause = !VAR->isPause;
        if (VAR->isPause)
        {
            // MODE_ABSOLUTE: state.x and state.y are absolute pixel values; system cursor is visible
            SOUND->StopAllKeySound(SDKEY::interface_pause);
            SOUND->Play(SDKEY::interface_pause, SDTYPE::SE, 1.0f, ChannelFlags_NotPuase);

            if (VAR->isDebug)
            {
                gameMouse->SetMode(Mouse::MODE_ABSOLUTE);
                gameMouse->SetVisible(false);
            }
        }
        else
        {
            // MODE_RELATIVE: state.x and state.y are relative values; system cursor is not visible
            
            if (VAR->isDebug)
            {
                gameMouse->SetMode(Mouse::MODE_RELATIVE);
                gameMouse->SetVisible(false);
            }
        }
    }
    if (mouse.positionMode == Mouse::MODE_RELATIVE)
        relPosition = { (float)mouse.x, (float)mouse.y };
    else
        relPosition = { 0.0f, 0.0f };

    // 이 게임패드 코드는 듀얼센스 패드를 DS4Windows외부프로그램에 연결해 사용한 것을 기준으로 작성됨.
    if (gamePadConnected && !App.activeWindow)
    {
        gamePad->Suspend();
        gamePadConnected = false;
        tracker.Reset();
    }
    else if (!gamePadConnected && App.activeWindow)
    {
        gamePad->Resume();
        gamePadConnected = true;
        tracker.Reset();
    }
    auto pad = gamePad->GetState(0, GamePad::DEAD_ZONE_NONE);
    if (pad.IsConnected())
    {
        gamePad->SetVibration(0, vib_curPower, vib_curPower, vib_curPower, vib_curPower);

        // 데드존 설정
        Vector2 stickL = { pad.thumbSticks.leftX ,pad.thumbSticks.leftY };
        Vector2 stickR = { pad.thumbSticks.rightX ,pad.thumbSticks.rightY };
        if (stickL.Length() < 0.5f) pad.thumbSticks.leftX = pad.thumbSticks.leftY = 0.0f;
        if (stickR.Length() < 0.5f) pad.thumbSticks.rightX = pad.thumbSticks.rightY = 0.0f;
        //------------
        tracker.Update(pad);
    }
    else
    {
        tracker.Reset();
    }
    UpdateVibration();


    //메모리복사 keyOldState = keyState 
    memcpy(keyOldState, keyState, sizeof(keyOldState));

    //기존의 배열값 초기화
    ZeroMemory(keyState, sizeof(keyState));

    ZeroMemory(keyMap, sizeof(keyMap));

    if (!App.activeWindow) return;

    if (tracker.a || tracker.b || tracker.x || tracker.y ||
        tracker.leftStick || tracker.rightStick ||
        tracker.leftShoulder || tracker.rightShoulder ||
        tracker.back || tracker.view ||
        tracker.start || tracker.menu ||
        tracker.dpadUp || tracker.dpadDown || tracker.dpadLeft || tracker.dpadRight ||
        tracker.leftStickUp || tracker.leftStickDown || tracker.leftStickLeft || tracker.leftStickRight ||
        tracker.rightStickUp || tracker.rightStickDown || tracker.rightStickLeft || tracker.rightStickRight ||
        tracker.leftTrigger || tracker.rightTrigger)
    {
        // 마지막 입력이 패드이다.
        usePad = true;
    }

    //BOOL a;
    //현재 호출상태에서의 키입력값을 가져오는 함수
    if (GetKeyboardState(keyState))
    {
        bool pressTest = false;
        delayPress = false;
        for (UINT i = 0; i < 256; i++)
        {
            byte key = keyState[i] & 0x80;
            //삼항연산자   (bool)? 1{} 0{}
            //키를 누른상태면 1, 아니면 0으로 대입
            keyState[i] = key ? 1 : 0;

            int oldState = keyOldState[i];
            int state = keyState[i];
            
            if (state) usePad = false;

            switch (i)
            {
            //case 'W': keyMap[i] = tracker.leftStickUp; break;
            //case 'A': keyMap[i] = tracker.leftStickLeft; break;
            //case 'S': keyMap[i] = tracker.leftStickDown; break;
            //case 'D': keyMap[i] = tracker.leftStickRight; break;
            //case 'I': keyMap[i] = tracker.rightStickUp; break;
            //case 'J': keyMap[i] = tracker.rightStickLeft; break;
            //case 'K': keyMap[i] = tracker.rightStickDown; break;
            //case 'L': keyMap[i] = tracker.rightStickRight; break;
            case VK_SPACE: padMap[i] = tracker.a; break;
            case 'E': padMap[i] = tracker.b; break;
            case VK_PRIOR: padMap[i] = tracker.dpadUp; break;
            case VK_NEXT: padMap[i] = tracker.dpadDown; break;
            case 'Q': padMap[i] = tracker.leftShoulder; break;
            case 'R': padMap[i] = GetPadState_Two(INPUT->tracker.rightShoulder, INPUT->tracker.b); break;
            case VK_LSHIFT: padMap[i] = tracker.rightShoulder; break;
            case VK_LBUTTON: padMap[i] = tracker.x; break;
            case VK_RBUTTON: padMap[i] = tracker.y; break;
            case VK_MBUTTON: padMap[i] = tracker.rightStick; break;
            case VK_TAB: padMap[i] = tracker.view; break;
            case VK_ESCAPE: padMap[i] = tracker.menu; break;
            }

            if (oldState == 0 && state == 1)
                keyMap[i] = KEY_INPUT_STATUS_DOWN; //이전 0, 현재 1 - KeyDown
            else if (oldState == 1 && state == 0)
                keyMap[i] = KEY_INPUT_STATUS_UP; //이전 1, 현재 0 - KeyUp
            else if (oldState == 1 && state == 1)
                keyMap[i] = KEY_INPUT_STATUS_PRESS; //이전 1, 현재 1 - KeyPress
            else
                keyMap[i] = KEY_INPUT_STATUS_NONE;

            if (padMap[i] == KEY_INPUT_STATUS_PRESS || keyMap[i] == KEY_INPUT_STATUS_PRESS)
                pressTest = true;
        }
        if (pressTest)
        {
            if (pressTime >= 0.7f)
            {
                delayPress = true;
                pressTime = 0.6f;
            }
            else
            {
                pressTime += DELTA;
            }
        }
    }

    POINT point;
    GetCursorPos(&point);

    ScreenToClient(App.handle, &point);

    wheelOldStatus.x = wheelStatus.x;
    wheelOldStatus.y = wheelStatus.y;

    wheelStatus.x = float(point.x);
    wheelStatus.y = float(point.y);

    wheelMoveValue = wheelStatus - wheelOldStatus;
    wheelOldStatus.z = wheelStatus.z;

    currentPostion = position;
    movePosition = currentPostion - oldPostion;

    oldPostion = currentPostion;

    //0~ 800  ->   0 ~ 2  -> -1 ~ 1
    NDCPosition.x = position.x / App.GetHalfWidth() - 1.0f;
    //0~ 600 -> 0~ -2 -> 1 ~ -1
    NDCPosition.y = position.y / -App.GetHalfHeight() + 1.0f;
}
