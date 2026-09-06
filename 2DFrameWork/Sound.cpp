#include "Framework.h"

Sound::Sound()
{
    //사운드 시스템 생성
    System_Create(&system);
    //사운드 채널 동적할당
    system->init(32, FMOD_INIT_NORMAL, nullptr);
    
    // 볼륨 초기화
    soundScale_Play[SoundType::BGM] = 1.0f;
    soundScale_Play[SoundType::SE] = 1.0f;
    soundScale_Play[SoundType::ENV] = 1.0f;

    soundScale_UserMaster = 1.0f;
    soundScale_User[SoundType::BGM] = 1.0f;
    soundScale_User[SoundType::SE] = 1.0f;
    soundScale_User[SoundType::ENV] = 1.0f;

    // 사운드 풀 생성
    for (int i = 0; i < 20; i++)
    {
        ChannelList.push_back(new ChannelNode());
    }
}

Sound::~Sound()
{
    // 채널정리
    for (auto iter = ChannelList.begin(); iter != ChannelList.end(); iter++)
    {
        (*iter)->channel->stop();
        delete *iter;
    }
    ChannelList.clear();

    // 효과음정리
    for (auto iter = SoundList.begin(); iter != SoundList.end(); iter++)
        iter->second->release();

    system->release();
    system->close();
}

void Sound::Init(SoundInitMode initMode)
{
    // BGM 세팅
    InitSound(initMode, "BGM00.kha", SoundKey::BGM00, true, 11665);
    InitSound(initMode, "BGM01.kha", SoundKey::BGM01, true);
    InitSound(initMode, "BGM02.kha", SoundKey::BGM02, false);

    // 사운드리스트 세팅
    InitSound(initMode, "footstep_a00.mp3", SoundKey::footstep_a00);
    InitSound(initMode, "footstep_a01.mp3", SoundKey::footstep_a01);
    InitSound(initMode, "footstep_a02.mp3", SoundKey::footstep_a02);
    InitSound(initMode, "footstep_a03.mp3", SoundKey::footstep_a03);
    InitSound(initMode, "footstep_a04.mp3", SoundKey::footstep_a04);
    InitSound(initMode, "footstep_a05.mp3", SoundKey::footstep_a05);
    InitSound(initMode, "footstep_a06.mp3", SoundKey::footstep_a06);
    InitSound(initMode, "footstep_a07.mp3", SoundKey::footstep_a07);
    InitSound(initMode, "footstep_a08.mp3", SoundKey::footstep_a08);
    InitSound(initMode, "footstep_a09.mp3", SoundKey::footstep_a09);
    InitSound(initMode, "footstep_b00.mp3", SoundKey::footstep_b00);
    InitSound(initMode, "footstep_b01.mp3", SoundKey::footstep_b01);
    InitSound(initMode, "footstep_b02.mp3", SoundKey::footstep_b02);
    InitSound(initMode, "footstep_b03.mp3", SoundKey::footstep_b03);
    InitSound(initMode, "footstep_b04.mp3", SoundKey::footstep_b04);
    InitSound(initMode, "footstep_b05.mp3", SoundKey::footstep_b05);
    InitSound(initMode, "footstep_b06.mp3", SoundKey::footstep_b06);
    InitSound(initMode, "footstep_b07.mp3", SoundKey::footstep_b07);
    InitSound(initMode, "footstep_b08.mp3", SoundKey::footstep_b08);
    InitSound(initMode, "footstep_b09.mp3", SoundKey::footstep_b09);
    InitSound(initMode, "footstep_c00.mp3", SoundKey::footstep_c00);
    InitSound(initMode, "footstep_c01.mp3", SoundKey::footstep_c01);
    InitSound(initMode, "footstep_c02.mp3", SoundKey::footstep_c02);
    InitSound(initMode, "footstep_c03.mp3", SoundKey::footstep_c03);
    InitSound(initMode, "footstep_misc00.mp3", SoundKey::footstep_misc00);
    InitSound(initMode, "slidestep00.mp3", SoundKey::slidestep00);
    InitSound(initMode, "slidestep01.mp3", SoundKey::slidestep01);
    InitSound(initMode, "slidestep02.mp3", SoundKey::slidestep02);
    InitSound(initMode, "evade00.mp3", SoundKey::evade00);
    InitSound(initMode, "evade01.mp3", SoundKey::evade01);
    InitSound(initMode, "evade02.mp3", SoundKey::evade02);
    InitSound(initMode, "evade03.mp3", SoundKey::evade03);
    InitSound(initMode, "death_impact.mp3", SoundKey::death_impact);
    InitSound(initMode, "down00.mp3", SoundKey::down00);
    InitSound(initMode, "down01.mp3", SoundKey::down01);
    InitSound(initMode, "down02.mp3", SoundKey::down02);
    InitSound(initMode, "jump00.mp3", SoundKey::jump00);
    InitSound(initMode, "echo00.mp3", SoundKey::echo00);
    InitSound(initMode, "guard_perfect.mp3", SoundKey::guard_perfect);
    InitSound(initMode, "guard00.mp3", SoundKey::guard00);
    InitSound(initMode, "guard01.mp3", SoundKey::guard01);
    InitSound(initMode, "guard02.mp3", SoundKey::guard02);
    InitSound(initMode, "guard03.mp3", SoundKey::guard03);
    InitSound(initMode, "guard04.mp3", SoundKey::guard04);
    InitSound(initMode, "guard05.mp3", SoundKey::guard05);
    InitSound(initMode, "guard06.mp3", SoundKey::guard06);
    InitSound(initMode, "item_use.mp3", SoundKey::item_use);
    InitSound(initMode, "hit_punch00.mp3", SoundKey::hit_punch00);
    InitSound(initMode, "hit_punch01.mp3", SoundKey::hit_punch01);
    InitSound(initMode, "hit_punch02.mp3", SoundKey::hit_punch02);
    InitSound(initMode, "hit_punch03.mp3", SoundKey::hit_punch03);
    InitSound(initMode, "hit_sword00.mp3", SoundKey::hit_sword00);
    InitSound(initMode, "hit_sword01.mp3", SoundKey::hit_sword01);
    InitSound(initMode, "hit_sword02.mp3", SoundKey::hit_sword02);
    InitSound(initMode, "hit_sword03.mp3", SoundKey::hit_sword03);
    InitSound(initMode, "hit_sword04.mp3", SoundKey::hit_sword04);
    InitSound(initMode, "hit_shot00.mp3", SoundKey::hit_shot00);
    InitSound(initMode, "hit_pole00.mp3", SoundKey::hit_pole00);
    InitSound(initMode, "hit_pole01.mp3", SoundKey::hit_pole01);
    InitSound(initMode, "block_sword00.mp3", SoundKey::block_sword00);
    InitSound(initMode, "block_sword01.mp3", SoundKey::block_sword01);
    InitSound(initMode, "block_sword02.mp3", SoundKey::block_sword02);
    InitSound(initMode, "block_sword03.mp3", SoundKey::block_sword03);
    InitSound(initMode, "attack_punch00.mp3", SoundKey::attack_punch00);
    InitSound(initMode, "attack_sword00.mp3", SoundKey::attack_sword00);
    InitSound(initMode, "attack_sword01.mp3", SoundKey::attack_sword01);
    InitSound(initMode, "attack_sword02.mp3", SoundKey::attack_sword02);
    InitSound(initMode, "attack_sword03.mp3", SoundKey::attack_sword03);
    InitSound(initMode, "attack_sword04.mp3", SoundKey::attack_sword04);
    InitSound(initMode, "attack_sword05.mp3", SoundKey::attack_sword05);
    InitSound(initMode, "attack_shot00.mp3", SoundKey::attack_shot00);
    InitSound(initMode, "attack_shot01.mp3", SoundKey::attack_shot01);
    InitSound(initMode, "attack_shot02.mp3", SoundKey::attack_shot02);
    InitSound(initMode, "attack_shot03.mp3", SoundKey::attack_shot03);
    InitSound(initMode, "attack_shot04.mp3", SoundKey::attack_shot04);
    InitSound(initMode, "attack_shot05.mp3", SoundKey::attack_shot05);
    InitSound(initMode, "attack_pole00.mp3", SoundKey::attack_pole00);
    InitSound(initMode, "attack_pole01.mp3", SoundKey::attack_pole01);
    InitSound(initMode, "attack_pole02.mp3", SoundKey::attack_pole02);
    InitSound(initMode, "attack_pole03.mp3", SoundKey::attack_pole03);
    InitSound(initMode, "attack_pole04.mp3", SoundKey::attack_pole04);
    InitSound(initMode, "draw_sword00.mp3", SoundKey::draw_sword00);
    InitSound(initMode, "draw_sword01.mp3", SoundKey::draw_sword01);
    InitSound(initMode, "goblin00.mp3", SoundKey::goblin00);
    InitSound(initMode, "goblin01.mp3", SoundKey::goblin01);
    InitSound(initMode, "goblin02.mp3", SoundKey::goblin02);
    InitSound(initMode, "goblin03.mp3", SoundKey::goblin03);
    InitSound(initMode, "goblin04.mp3", SoundKey::goblin04);
    InitSound(initMode, "goblin05.mp3", SoundKey::goblin05);
    InitSound(initMode, "goblin06.mp3", SoundKey::goblin06);
    InitSound(initMode, "goblin07.mp3", SoundKey::goblin07);
    InitSound(initMode, "goblin08.mp3", SoundKey::goblin08);
    InitSound(initMode, "goblin09.mp3", SoundKey::goblin09);
    InitSound(initMode, "goblin10.mp3", SoundKey::goblin10);
    InitSound(initMode, "goblin11.mp3", SoundKey::goblin11);
    InitSound(initMode, "goblin12.mp3", SoundKey::goblin12);
    InitSound(initMode, "goblin13.mp3", SoundKey::goblin13);
    InitSound(initMode, "goblin14.mp3", SoundKey::goblin14);
    InitSound(initMode, "drone00.mp3", SoundKey::drone00);
    InitSound(initMode, "drone01.mp3", SoundKey::drone01);
    InitSound(initMode, "drone02.mp3", SoundKey::drone02);
    InitSound(initMode, "drone03.mp3", SoundKey::drone03);
    InitSound(initMode, "robot00.mp3", SoundKey::robot00);
    InitSound(initMode, "robot01.mp3", SoundKey::robot01);
    InitSound(initMode, "robot02.mp3", SoundKey::robot02);
    InitSound(initMode, "robot03.mp3", SoundKey::robot03);
    InitSound(initMode, "robot04.mp3", SoundKey::robot04);
    InitSound(initMode, "robot05.mp3", SoundKey::robot05);
    InitSound(initMode, "robot06.mp3", SoundKey::robot06);
    InitSound(initMode, "robot07.mp3", SoundKey::robot07);
    InitSound(initMode, "robot08.mp3", SoundKey::robot08);
    InitSound(initMode, "robot09.mp3", SoundKey::robot09);
    InitSound(initMode, "tremble00.mp3", SoundKey::tremble00);
    InitSound(initMode, "motor00.wav", SoundKey::motor00, true);
    InitSound(initMode, "motor01.wav", SoundKey::motor01, true);
    InitSound(initMode, "spark00.mp3", SoundKey::spark00);
    InitSound(initMode, "spark01.mp3", SoundKey::spark01);
    InitSound(initMode, "spark02.mp3", SoundKey::spark02);
    InitSound(initMode, "spark03.mp3", SoundKey::spark03);
    InitSound(initMode, "steam00.mp3", SoundKey::steam00);
    InitSound(initMode, "steam01.mp3", SoundKey::steam01);
    InitSound(initMode, "crash00.mp3", SoundKey::crash00);
    InitSound(initMode, "crash01.mp3", SoundKey::crash01);
    InitSound(initMode, "crash02.mp3", SoundKey::crash02);
    InitSound(initMode, "crash03.mp3", SoundKey::crash03);
    InitSound(initMode, "explosion00.mp3", SoundKey::explosion00);
    InitSound(initMode, "explosion01.mp3", SoundKey::explosion01);
    InitSound(initMode, "explosion02.mp3", SoundKey::explosion02);
    InitSound(initMode, "explosion03.mp3", SoundKey::explosion03);
    InitSound(initMode, "wind00.wav", SoundKey::wind00, true);
    InitSound(initMode, "interface_itemopen.mp3", SoundKey::interface_itemOpen);
    InitSound(initMode, "interface_itemclose.mp3", SoundKey::interface_itemClose);
    InitSound(initMode, "interface_itemmove.mp3", SoundKey::interface_itemMove);
    InitSound(initMode, "interface_itemuse.mp3", SoundKey::interface_itemUse);
    InitSound(initMode, "interface_message.mp3", SoundKey::interface_message);
    InitSound(initMode, "interface_pause.mp3", SoundKey::interface_pause);
    InitSound(initMode, "interface_lockon.mp3", SoundKey::interface_lockOn);
    InitSound(initMode, "interface_lockoff.mp3", SoundKey::interface_lockOff);
    InitSound(initMode, "mission_start.mp3", SoundKey::mission_start);
    InitSound(initMode, "mission_clear.mp3", SoundKey::mission_clear);
    InitSound(initMode, "misc00.mp3", SoundKey::misc00);
    InitSound(initMode, "misc01.mp3", SoundKey::misc01);
    InitSound(initMode, "misc02.mp3", SoundKey::misc02);
    InitSound(initMode, "misc03.mp3", SoundKey::misc03);
    InitSound(initMode, "misc04.mp3", SoundKey::misc04);
    InitSound(initMode, "misc05.mp3", SoundKey::misc05);
    InitSound(initMode, "misc06.mp3", SoundKey::misc06);
}

bool Sound::InitSound(SoundInitMode initMode, string File, SoundKey Key, bool loop, UINT loopStart_MS)
{
    if (App.GetAppQuit()) return false;

    string path = "Contents/Sound/" + File;

    //key 중복 허용x
    auto iter = SoundList.find(Key);

    switch (initMode)
    {
    case SoundInitMode::CALCULATE:
    {
        //중복된게 있다.
        if (iter != SoundList.end())
            return false;

        // 중복된 게 없으면 들어갈 용량을 계산하고 사운드를 먼저 nullptr로 정의한다.
        SoundList[Key] = nullptr;
        RESOURCE->AddTotalCapacity(Util::CalculateFile(path));
        return true;
    }
    case SoundInitMode::ADD:
    {
        //중복된게 있다 (내용물이 이미 존재).
        if (iter != SoundList.end() && iter->second != nullptr)
            return false;

        //중복된게 없으면 사운드를 추가
        bool b_calculated = iter->second == nullptr;
        FMOD::Sound* temp;
        system->createSound
        (
            path.c_str(),
            FMOD_DEFAULT,
            nullptr,
            &temp
        );
        if (loop)
        {
            temp->setMode(FMOD_LOOP_NORMAL);
            if (loopStart_MS > 0.0f)
            {
                UINT soundLength;
                temp->getLength(&soundLength, FMOD_TIMEUNIT_MS);
                temp->setLoopPoints(loopStart_MS, FMOD_TIMEUNIT_MS, soundLength - 1, FMOD_TIMEUNIT_MS);
            }
        }
        else
        {
            temp->setMode(FMOD_LOOP_OFF);
        }

        //맵에 할당한 배열 원소넣기
        SoundList[Key] = temp;

        RESOURCE->AddCurrentCapacity(b_calculated, Util::CalculateFile(path));
        return true;
    }
    default:
        assert(false);
    }
}

bool Sound::DeleteSound(SoundKey Key)
{
    auto iter = SoundList.find(Key);

    // 검색된 것이 없다.
    if (iter == SoundList.end())
    {
        return false;
    }

    // 관련채널 재생정지
    for (auto iter2 = ChannelList.begin(); iter2 != ChannelList.end(); iter2++)
    {
        if ((*iter2)->key == Key)
            (*iter2)->channel->stop();
    }
    iter->second->release();

    //맵에서도 삭제
    SoundList.erase(iter);

    return true;
}

Sound::ChannelNode* Sound::Play(SoundKey Key, vector<bool>& soundList, int num, SoundType type, float volume, ChannelFlags flags)
{
    if (soundList[num]) return nullptr;
    soundList[num] = true;

    return Play(Key, type, volume, flags);
}

Sound::ChannelNode* Sound::Play(SoundKey Key, vector<bool>& soundList, int num, SoundType type, float volume, Vector3 pos, float dist, ChannelFlags flags)
{
    if(soundList[num]) return nullptr;
    soundList[num] = true;

    return Play(Key, type, volume, pos, dist, flags);
}

Sound::ChannelNode* Sound::Play(SoundKey Key, vector<bool>& soundList, int num, SoundType type, float volume, const Object* follow, float dist, ChannelFlags flags)
{
    if(soundList[num]) return nullptr;
    soundList[num] = true;

    return Play(Key, type, volume, follow, dist, flags);
}

Sound::ChannelNode* Sound::Play(SoundKey Key, SoundType type, float volume, ChannelFlags flags)
{
    if (Key == SoundKey::NONE) return nullptr;
    ChannelNode* node = GenerateChannel(Key);

    // 해당 채널의 설정을 변경 및 재생
    node->key = Key;
    node->type = type;
    node->volume = volume;
    node->mode = FMOD_DEFAULT;
    node->vel = { 0,0,0 };
    node->follow = nullptr;
    node->vel = { 0,0,0 };
    node->dist = 1.0f;
    node->flags = flags;
    return PlayChannel(node);
}

Sound::ChannelNode* Sound::Play(SoundKey Key, SoundType type, float volume, Vector3 pos, float dist, ChannelFlags flags)
{
    if (Key == SoundKey::NONE) return nullptr;
    ChannelNode* node = GenerateChannel(Key);

    // 해당 채널의 설정을 변경 및 재생
    node->key = Key;
    node->type = type;
    node->volume = volume;
    node->mode = FMOD_3D;
    node->pos = { pos.x,pos.y,pos.z };
    node->follow = nullptr;
    node->vel = { 0,0,0 };
    node->dist = dist;
    node->flags = flags;
    return PlayChannel(node);
}

Sound::ChannelNode* Sound::Play(SoundKey Key, SoundType type, float volume, const Object* follow, float dist, ChannelFlags flags)
{
    if (follow == nullptr || Key == SoundKey::NONE) return nullptr;
    ChannelNode* node = GenerateChannel(Key);

    // 해당 채널의 설정을 변경 및 재생
    node->key = Key;
    node->type = type;
    node->volume = volume;
    node->mode = FMOD_3D;
    node->pos = { 0,0,0 };
    node->follow = follow;
    node->vel = { 0,0,0 };
    node->dist = dist;
    node->flags = flags;
    return PlayChannel(node);
}

Sound::ChannelNode* Sound::GenerateChannel(SoundKey Key)
{
    // 해당 리소스가 존재한다면
    auto iter_sound = SoundList.find(Key);
    if (iter_sound != SoundList.end())
    {
        auto iter_channel = ChannelList.begin();
        ChannelNode* node = nullptr;

        // 재생중이지 않은 채널이 존재하면 해당 채널을 사용
        while (iter_channel != ChannelList.end())
        {
            bool isPlay;
            (*iter_channel)->channel->isPlaying(&isPlay);
            if (!isPlay)
            {
                node = (*iter_channel);
                break;
            }
            iter_channel++;
        }

        // 모든 채널이 재생중이라면 새 채널을 추가
        if (iter_channel == ChannelList.end())
        {
            ChannelNode* temp = new ChannelNode();
            ChannelList.push_back(temp);
            node = temp;
        }
        return node;
    }
    return nullptr;
}

void Sound::SetVolumeType(SoundType type, float value)
{
    soundScale_Play[type] = value;
}

void Sound::SetAllVolumeType(float value_BGM, float value_SE, float value_ENV)
{
    SetVolumeType(SoundType::BGM, value_BGM);
    SetVolumeType(SoundType::SE, value_SE);
    SetVolumeType(SoundType::ENV, value_ENV);
}

void Sound::FadeVolumeType(SoundType type, float destVol, float fadeTime)
{
    fadeStatus[type].isFading = true;
    fadeStatus[type].fadeTime = 0.0f;
    fadeStatus[type].fadeMaxTime = fadeTime;
    fadeStatus[type].startVol = soundScale_Play[type];
    fadeStatus[type].endVol = destVol;
}

void Sound::FadeAllVolumeType(float destVol_BGM, float destVol_SE, float destVol_ENV, float fadeTime)
{
    FadeVolumeType(SoundType::BGM, destVol_BGM, fadeTime);
    FadeVolumeType(SoundType::SE, destVol_SE, fadeTime);
    FadeVolumeType(SoundType::ENV, destVol_ENV, fadeTime);
}

void Sound::StopSoundType(SoundType type)
{
    for (auto iter = ChannelList.begin(); iter != ChannelList.end(); iter++)
    {
        if ((*iter)->type == type)
            (*iter)->channel->stop();
    }
}

void Sound::StopAllKeySound(SoundKey Key)
{
    for (auto iter = ChannelList.begin(); iter != ChannelList.end(); iter++)
    {
        if ((*iter)->key == Key)
            (*iter)->channel->stop();
    }
}

Sound::ChannelNode* Sound::PlayChannel(ChannelNode* node)
{
    if (node != nullptr)
    {
		system->playSound(
			SoundList[node->key], nullptr,
			false,
			&node->channel);

        node->channel->setVolume(GetTotalVolume(node->volume, node->type, node->flags));

        node->channel->setMode(node->mode);
        if (node->follow)
            node->pos = { node->follow->W._41, node->follow->W._42, node->follow->W._43 };
        node->channel->set3DAttributes(&node->pos, &node->vel);
        node->channel->set3DMinMaxDistance(node->dist, 10000.0f);

        switch (node->type)
        {
        case SoundType::BGM: node->channel->setPriority(0); break;  // BGM은 0순위 (우선순위가 높을수록 채널 재생초과 시 먼저 사라지지 않음)
        case SoundType::ENV: node->channel->setPriority(1); break;  // ENV는 1순위
        default: node->channel->setPriority(128);
        }
    }
    return node;
}

void Sound::SetListenerAttr(Vector3 vel)
{
    FMOD_VECTOR temppos = { listenerPos.x, listenerPos.y, listenerPos.z };
    FMOD_VECTOR tempvel = { vel.x, vel.y, vel.z };
    FMOD_VECTOR tempforward = { listenerForward.x, listenerForward.y, listenerForward.z };
    FMOD_VECTOR tempup = { listenerUp.x, listenerUp.y, listenerUp.z };
    system->set3DListenerAttributes(0, &temppos, &tempvel, &tempforward, &tempup);
}

float Sound::GetTotalVolume(float initial_volume, SoundType type, ChannelFlags flags)
{
    float total = initial_volume * soundScale_UserMaster;
    switch (type)
    {
    case SoundType::BGM: total *= (soundScale_User[SoundType::BGM] * ((flags & ChannelFlags_Independent) ? 1.0f : soundScale_Play[SoundType::BGM])); break;
    case SoundType::SE:  total *= (soundScale_User[SoundType::SE] * ((flags & ChannelFlags_Independent) ? 1.0f : soundScale_Play[SoundType::SE])); break;
    case SoundType::ENV: total *= (soundScale_User[SoundType::ENV] * ((flags & ChannelFlags_Independent) ? 1.0f : soundScale_Play[SoundType::ENV])); break;
    default: assert(false);
    }
    return total;
}

void Sound::StopAllSound()
{
    for (auto iter = ChannelList.begin(); iter != ChannelList.end(); iter++)
    {
        (*iter)->channel->stop();
    }
}

void Sound::SetMasterVolume()
{
    for (auto iter = ChannelList.begin(); iter != ChannelList.end(); iter++)
    {
        (*iter)->channel->setVolume(GetTotalVolume((*iter)->volume, (*iter)->type, (*iter)->flags));
    }
}

void Sound::Update()
{
    // 페이드 업데이트
    for (auto iter = fadeStatus.begin(); iter != fadeStatus.end(); iter++)
    {
        if (iter->second.isFading)
        {
            iter->second.fadeTime += DELTA_NS;
            if (iter->second.fadeTime >= iter->second.fadeMaxTime)
            {
                iter->second.isFading = false;
                soundScale_Play[iter->first] = iter->second.endVol;
            }
            else
                soundScale_Play[iter->first] = Util::Lerp(iter->second.startVol, iter->second.endVol, iter->second.fadeTime / iter->second.fadeMaxTime);
        }
    }

    // 채널 업데이트
    for (auto iter = ChannelList.begin(); iter != ChannelList.end(); iter++)
    {
        if ((*iter)->type == SoundType::SE && !((*iter)->flags & ChannelFlags_NotPuase))
        {
            bool isPaused;
            (*iter)->channel->getPaused(&isPaused);
            if (isPaused && !VAR->isPause)
                (*iter)->channel->setPaused(false);
            else if (!isPaused && VAR->isPause)
                (*iter)->channel->setPaused(true);
        }

        bool isPlay;
        (*iter)->channel->isPlaying(&isPlay);

        if (isPlay)
        {
            if ((*iter)->follow)
                (*iter)->pos = { (*iter)->follow->W._41, (*iter)->follow->W._42, (*iter)->follow->W._43 };
            (*iter)->channel->set3DAttributes(&(*iter)->pos, &(*iter)->vel);
            (*iter)->channel->set3DMinMaxDistance((*iter)->dist, 10000.0f);
        }
        (*iter)->channel->setVolume(GetTotalVolume((*iter)->volume, (*iter)->type, (*iter)->flags));
    }

    // 사운드 시스템 업데이트
    system->update();
}

void Sound::DebugRender()
{
    if (!VAR->isDebug) return;
    ImGui::Text("SoundChannels: %d", ChannelList.size());
}
