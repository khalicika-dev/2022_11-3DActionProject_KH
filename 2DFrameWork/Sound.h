#pragma once

typedef int ChannelFlags;
enum ChannelFlags_
{
    ChannelFlags_None = 0,              // 없음
    ChannelFlags_Independent = 1 << 0,  // soundScale_Play값의 영향을 받지않고 볼륨이 독립적임.
    ChannelFlags_NotPuase = 1 << 1,     // (App.Pause의 영향을 받는 SE사운드 한정) App.Pause의 영향을 받지 않음
};

class Sound : public Singleton<Sound>
{
public:
    enum class SoundInitMode
    {
        CALCULATE,
        ADD
    };
    enum class SoundType
    {
        BGM,            // 배경음
        SE,             // 효과음
        ENV             // 환경음
    };
    enum class SoundKey
    {
        NONE,
        BGM00,
        BGM01,
        BGM02,
        footstep_a00,
        footstep_a01,
        footstep_a02,
        footstep_a03,
        footstep_a04,
        footstep_a05,
        footstep_a06,
        footstep_a07,
        footstep_a08,
        footstep_a09,
        footstep_b00,
        footstep_b01,
        footstep_b02,
        footstep_b03,
        footstep_b04,
        footstep_b05,
        footstep_b06,
        footstep_b07,
        footstep_b08,
        footstep_b09,
        footstep_c00,
        footstep_c01,
        footstep_c02,
        footstep_c03,
        footstep_misc00,
        slidestep00,
        slidestep01,
        slidestep02,
        evade00,
        evade01,
        evade02,
        evade03,
        death_impact,
        down00,
        down01,
        down02,
        jump00,
        echo00,
        guard_perfect,
        guard00,
        guard01,
        guard02,
        guard03,
        guard04,
        guard05,
        guard06,
        item_use,
        hit_punch00,
        hit_punch01,
        hit_punch02,
        hit_punch03,
        hit_sword00,
        hit_sword01,
        hit_sword02,
        hit_sword03,
        hit_sword04,
        hit_shot00,
        hit_pole00,
        hit_pole01,
        block_sword00,
        block_sword01,
        block_sword02,
        block_sword03,
        attack_punch00,
        attack_sword00,
        attack_sword01,
        attack_sword02,
        attack_sword03,
        attack_sword04,
        attack_sword05,
        attack_shot00,
        attack_shot01,
        attack_shot02,
        attack_shot03,
        attack_shot04,
        attack_shot05,
        attack_pole00,
        attack_pole01,
        attack_pole02,
        attack_pole03,
        attack_pole04,
        draw_sword00,
        draw_sword01,
        goblin00,
        goblin01,
        goblin02,
        goblin03,
        goblin04,
        goblin05,
        goblin06,
        goblin07,
        goblin08,
        goblin09,
        goblin10,
        goblin11,
        goblin12,
        goblin13,
        goblin14,
        drone00,
        drone01,
        drone02,
        drone03,
        robot00,
        robot01,
        robot02,
        robot03,
        robot04,
        robot05,
        robot06,
        robot07,
        robot08,
        robot09,
        tremble00,
        motor00,
        motor01,
        spark00,
        spark01,
        spark02,
        spark03,
        steam00,
        steam01,
        crash00,
        crash01,
        crash02,
        crash03,
        explosion00,
        explosion01,
        explosion02,
        explosion03,
        wind00,
        interface_itemOpen,
        interface_itemClose,
        interface_itemMove,
        interface_itemUse,
        interface_message,
        interface_pause,
        interface_lockOn,
        interface_lockOff,
        mission_start,
        mission_clear,
        misc00,
        misc01,
        misc02,
        misc03,
        misc04,
        misc05,
        misc06,
    };
    struct ChannelNode
    {
        SoundKey key;
        FMOD::Channel* channel;    //출력해줄 채널
        SoundType type;
        FMOD_MODE mode;
        FMOD_VECTOR pos;
        const Object* follow;
        FMOD_VECTOR vel;
        float dist;
        float volume;
        ChannelFlags flags;   // 이 채널의 특수플래그
    };

    // 청취자전용 변수
    Vector3 listenerPos;
    Vector3 listenerForward;
    Vector3 listenerUp;

    float soundScale_UserMaster;            // 볼륨스케일: 유저가 마스터볼륨 조절
    map<SoundType, float> soundScale_User;  // 볼륨스케일을 유저가 타입별로 조절
    map<SoundType, float> soundScale_Play;  // 볼륨스케일을 플레이 중 타입별로 조절
private:
    struct SoundFadeStatus
    {
        bool isFading = false;
        float fadeTime = 0.0f;
        float fadeMaxTime = 0.0f;
        float startVol = 0.0f;
        float endVol = 0.0f;
    };
    map<SoundType, SoundFadeStatus> fadeStatus;  // 타입별로 사운드 페이드조절

    map<SoundKey, FMOD::Sound*> SoundList;          // 출력할 사운드데이터
    list<ChannelNode*> ChannelList;
    FMOD::System* system;
    ChannelNode* GenerateChannel(SoundKey Key);
    ChannelNode* PlayChannel(ChannelNode* node);

public:
    Sound();
    ~Sound();

    bool InitSound(SoundInitMode initMode, string File, SoundKey Key, bool loop = false, UINT loopStart_MS = 0.0f);
    bool DeleteSound(SoundKey Key);
    void Init(SoundInitMode initMode);
    
    // SoundList: 연속재생 방지를 위한 잠금장치
    ChannelNode* Play(SoundKey Key, vector<bool>& soundList, int num, SoundType type, float volume, ChannelFlags flags = ChannelFlags_None); // 표준재생(어디서나 들림)
    ChannelNode* Play(SoundKey Key, vector<bool>& soundList, int num, SoundType type, float volume, Vector3 pos, float dist = 10.0f, ChannelFlags flags = ChannelFlags_None); // 좌표고정 재생 (한 곳에서만 재생됨)
    ChannelNode* Play(SoundKey Key, vector<bool>& soundList, int num, SoundType type, float volume, const Object* follow, float dist = 10.0f, ChannelFlags flags = ChannelFlags_None); // 객체추종 재생 (소리가 객체를 따라감)

    // SoundList매개변수 제거 = 조건없이 재생
    ChannelNode* Play(SoundKey Key, SoundType type, float volume, ChannelFlags flags = ChannelFlags_None);
    ChannelNode* Play(SoundKey Key, SoundType type, float volume, Vector3 pos, float dist = 10.0f, ChannelFlags flags = ChannelFlags_None);
    ChannelNode* Play(SoundKey Key, SoundType type, float volume, const Object* follow, float dist = 10.0f, ChannelFlags flags = ChannelFlags_None);

    void SetVolumeType(SoundType type, float value);
    void SetAllVolumeType(float value_BGM, float value_SE, float value_ENV);
    void FadeVolumeType(SoundType type, float destVol, float fadeTime);
    void FadeAllVolumeType(float destVol_BGM, float destVol_SE, float destVol_ENV, float fadeTime);
    void StopSoundType(SoundType type);
    void StopAllKeySound(SoundKey Key); // 특정 키를 재생중인 모든 사운드 제거
    void StopAllSound();    // 재생중인 모든 사운드 제거
    void SetListenerAttr(Vector3 vel = { 0,0,0 });
    float GetTotalVolume(float initial_volume, SoundType type, ChannelFlags flags);

    void SetMasterVolume();
    void Update();
    void DebugRender();
};

