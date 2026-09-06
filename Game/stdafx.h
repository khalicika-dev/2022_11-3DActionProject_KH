#pragma once
#include "../2DFrameWork/framework.h"
#pragma comment (lib,"../Build/2DFrameWork/2DFrameWork.lib")
#include "Config.h"
#include "PauseMenu.h"
#include "InfoMsg.h"
#include "DmgNumber.h"
#include "ItemManager.h"
#include "GameObject.h"
#include "Object_Trail.h"
#include "Player.h"
#include "Monster.h"
#include "Monster_Goblin.h"
#include "Monster_Drone.h"
#include "Monster_Giant.h"
#include "Effect_Hit.h"
#include "Bullet_Red.h"
#include "GObjectManager.h"
#include "Player_Interface.h"
#include "Scene1.h"
#include "LoadingScene.h"

#define GAMEOBJ GObjectManager::GetInstance()
#define GOTYPE GObjectManager::GOType
#define PTCTYPE GObjectManager::ParticleType
#define GET_PLAYER GObjectManager::GetInstance()->FindPlayer()
#define CONFIG ConfigManager::GetInstance()
#define PLAYER_UI Player_Interface::GetInstance()