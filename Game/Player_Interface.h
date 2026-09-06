#pragma once
class Player_Interface : public Singleton<Player_Interface>
{
	friend class Player;
public:
	map<string, shared_ptr<Texture>> textureResources;
	struct DmgLog
	{
		UINT priority;	// 가장 높은 번호(가장 마지막으로 표시된 번호)가 맨 앞으로 보이게
		DmgNumber* obj;

		// priority가 작을수록 Top에 가깝다
		bool operator<(const DmgLog& tg) const {
			return this->priority > tg.priority;
		}
	};
	enum EnumGameStartState // 미션스타트 상태변수표
	{
		STATE_TITLE = -1,			// 시작전
		STATE_STARTING_GAME = 0,	// 시작중
		STATE_START_GAME = 1,		// 시작
		STATE_IN_GAME = 2			// 인게임
	};

	Player_Interface();
	~Player_Interface();

	void Init();
	void Calculate();
	void Reset();
	void Update();
	void LateUpdate();
	void DebugRender();
	void Render();

	void ShowDamage(Vector3 pos, int value, DmgNumber::DmgType type);
	void PushMessage(InfoMsg::Subject subject, InfoMsg::MSG msg);
	void SetBossUI(Monster* target) { boss = target; }
	UINT GetCinematicLevel() { return cinematicLevel; };
	UINT GetDying() { return dying; };
	void OpenCinematic(int id);
	void GameStart();
	void GameClear(bool value);
	bool IsGameEnding() { return b_missionClear && clearTime <= 0.0f; }
	int GetGameStart() { return gameStartState; }
private:
	Player* player;		// 플레이어
	Monster* boss;		// 보스

	Actor* bar_hp;		// 체력
	Actor* bar_st;		// 스태미나
	Actor* bar_item;	// 소지품창
	Actor* bar_bossHp;	// 보스체력
	Actor* bar_bossGroggy;	// 보스그로기
	deque<DmgNumber*> log_dmg;	// 대미지 표시
	Actor* lock_mark;	// 록온표시
	Actor* log_info;	// 메세지 로그
	Actor* log_clear;	// 귀환 로그
	UI* text_gameStart;
	UI* text_gameClear;
	UI* text_title;
	queue<InfoMsg::InfoText> queue_msg;	// 표시중인 메세지내용

	enum SoundCondition
	{
		TITLE_TRANSITION,
		MISSION_START,
		MISSION_CLEAR,
		ITEM_OPEN,
		ITEM_CLOSE,
		ITEM_MOVE,
		DIE,
		SALVATION,
		CINEMATIC_FADEIN,
		CINEMATIC_BGM_CHANGE,
		ALL_COUNTS	// 사운드의 갯수를 구할때 사용하는 상수
	};
	vector<bool> soundBoard;	// 사운드의 출력여부를 결정하는 변수

	void MakeGuage(Actor* parent, float val, float maxVal, Vector2 NDCPos, Vector2 Size, Color color, float stroke, bool relative);
	void MakeItemBar(Actor* parent);
	void MakeInfoBar(Actor* parent);
	void MakeClearBar(Actor* parent);
	void MoveItem(int moveDir) { menuMove = moveDir; menuMoveTime = 0.0f; }
	void UpdateGuage(Actor* parent, float val, float maxVal, float width, bool relative);
	void SetDying(int value) { if (dying != value) { dying = value; deathTime = 0.0f; } }
	void SetCinematicLevel(int value);
	void CloseCinematic();
private:
	enum class MenuState
	{
		INVISIBLE,
		NORMAL,
		ITEM
	}menu;
	float stAlertTime = 0.0f;	// 스태미나가 고갈직전의 깜빡임의 현재주기를 나타냄
	float bossGroggyAlertTime = 0.0f;	// 보스의 그로기 회복상태의 깜빡임의 현재주기를 나타냄
	const Vector2 centerPos = { 1662.0f,995.0f };	// 아이템블록 센터좌표
	const float ITSizeITV1 = 95.0f; // 아이템블록 간격: 중앙-사이드간의 x간격
	const float ITSizeITV2 = 80.0f; // 아이템블록 간격: 사이드간의 x간격
	int menuOpen = 0;	// 0:사용불가, 1:사용가능, 2:사용중
	int menuMove = 0;	// 0:고정, 양(+):우측이동, 음(-):좌측이동
	float menuOpenTime = 0.0f;
	float menuMoveTime = 0.0f;
	bool menuItemUse = false;	// 아이템창이 닫힐 때 아이템을 사용했는가의 유무

	const float msgShowTimeMax = 3.0f;
	float msgShowTime = 0.0f;	// 메세지로그를 표시하는 시간
	const float msgBarPosY = 650.0f;
	float msgBarWidth;

	// 리타이어 컷신 부문
	int dying = 0;
	float deathTime = 0.0f;
	bool resurrectionMSG = true;

	// 시네마틱 부문
	UINT cinematicLevel = 0;		// 시네마틱 컷신진행도
	float cinematicTime = 0.0f;
	int cinematicID;				// 사용할 시네마틱ID
	bool b_singalForCinematic;		// 시네마틱전용 상태전환을 요구하는 신호(true면 대상자가 시네마틱에 적합한 상태로 변경)
	bool b_cinematicProceed;		// 시네마틱이 진행중인가? (cinematicLevel = 2 에서 사용)
	bool b_cinematicEnding;			// 시네마틱이 종료되는 중인가? (cinematicLevel = 2 에서 사용, true면 장면 페이드아웃)
	vector<GameObject*> sceneActors;	// 해당 시네마틱에 출현할 객체(배우)들
	UINT centerActor;					// 해당 시네마틱에서 조명중인 sceneActors 객체번호

	// 타이틀-스타트 부문
	int gameStartState;		// 미션스타트 상태변수
	float gameStartTime;	// 게임스타트 누른 후 걸리는 시간

	// 클리어 부문
	bool b_missionClear;	// 게임클리어 여부
	float clearTime;	// 클리어 후 귀환까지 남은 시간
	float clearLogOpacity;
	const float clearBarPosY = 565.0f;
	const float clearLogTime = 0.3f;	// 클리어로그를 띄우는 데 걸리는 시간
	const float clearMaxTime = 30.0f;	// 클리어직후 주어지는 귀환까지 남은 시간

	float startTextTime;	// 게임시작 텍스트를 띄우는 시간
};