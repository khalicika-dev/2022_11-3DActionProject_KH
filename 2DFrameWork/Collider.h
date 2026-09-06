#pragma once
enum class ColliderType
{
   SPHERE,
   BOX,
   OBOX,
};

// Collider가 가진 속성 플래그
enum class GameType
{
    NONE             = 0,       // 없음
    JUMP             = 1 << 0,  // 몬스터가 인식하기 위한 점프지형
    ONLY_PLAYER      = 1 << 1,  // 오직 플레이어만이 충돌가능함

    HIT_PLAYER       = 1 << 2,  // 플레이어의 히트콜라이더
    HIT_MONSTER      = 1 << 3,  // 몬스터의 히트콜라이더
    LOCKON_MON       = 1 << 4,  // 록온이 가능한 몬스터(중심이 되는 몸통)
    LOCKON_MON_PARTS = 1 << 5,  // 록온이 가능한 몬스터(록온을 허용하는 부위)
    TERRAIN_COL      = 1 << 6,  // 지형효과(카메라, 투사체 블록) 부여
    COL_CENTER       = 1 << 7,  // 공격판정을 이 공격콜라이더를 중심으로 (꺼져있으면 주인오브젝트를 중심으로)
    DONT_SHOW_DAMAGE = 1 << 8,  // 플레이어가 이 물체에게 입히는 데미지 표기금지
    IGNORE_BULLETS   = 1 << 9   // 이 유닛은 탄환의 충돌을 받지 않음
};

class Collider : public Transform
{
    friend class Object;
public:
    shared_ptr<Mesh>        mesh;
    shared_ptr<Shader>      shader;
    ColliderType            type;
    
    bool                    enable;
    bool                    visible;

    // 게임용 변수
    int                     gameType;   // 콜라이더의 특수속성
    struct AttackProperty
    {
        int                 damage          = 0;        // 피격된 대상에게 입히는 대미지
        int                 knockdown       = 0;        // 피격된 대상에게 입히는 넉다운게이지
        Vector2             kb_velocity     = { 0,0 };  // 피격된 대상이 넉백될 시 밀려나는 수평,수직속도 (플레이어대상)
        Sound::SoundKey     hitSound = Sound::SoundKey::NONE;   // 피격된 대상에게서 나는 사운드
        Sound::SoundKey     guardSound = Sound::SoundKey::NONE; // 피격된 대상이 데미지 경감 시 나는 사운드
    }atk;   // AttackCollider의 속성

    struct HitProperty
    {
        float                   crit        = 1.0f;     // 피격된 대상이 받는 대미지의 치명타 배율
        float                   knock_crit  = 1.0f;     // 피격된 대상이 받는 넉다운게이지의 치명타배율
    }hit;   // HitCollider의 속성

    vector<class Actor*>           touched;    // 피격된 대상목록
public:
    Collider(ColliderType type);
    ~Collider();
    static uint64_t     CalculateType(ColliderType type);   // 생성하고자 하는 콜라이더의 용량 계산
    void                Update(class Object* ob);
    void                Update();
    void                Render();
    void                RenderDetail();
    bool                Intersect(Collider* target);
    bool                Intersect(Ray Ray,Vector3& Hit);
};

