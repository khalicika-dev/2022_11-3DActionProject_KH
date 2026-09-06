#include "stdafx.h"

map<InfoMsg::Subject, shared_ptr<Texture>> InfoMsg::subjectList;
map<InfoMsg::MSG, shared_ptr<Texture>> InfoMsg::msgList;
void InfoMsg::CreateStaticMember()
{
	subjectList[Subject::Goblin] = RESOURCE->textures.Load("UI/Info/s_goblin.png");
	subjectList[Subject::Drone] = RESOURCE->textures.Load("UI/Info/s_drone.png");
	subjectList[Subject::IronColossus] = RESOURCE->textures.Load("UI/Info/s_ironColossus.png");
	subjectList[Subject::IronColossus_LShoulder] = RESOURCE->textures.Load("UI/Info/s_ironColossus_LShoulder.png");
	subjectList[Subject::IronColossus_RShoulder] = RESOURCE->textures.Load("UI/Info/s_ironColossus_RShoulder.png");
	subjectList[Subject::IronColossus_LFoot] = RESOURCE->textures.Load("UI/Info/s_ironColossus_LFoot.png");
	subjectList[Subject::IronColossus_RFoot] = RESOURCE->textures.Load("UI/Info/s_ironColossus_RFoot.png");
	subjectList[Subject::IronColossus_Head] = RESOURCE->textures.Load("UI/Info/s_ironColossus_Head.png");
	subjectList[Subject::CheckPoint] = RESOURCE->textures.Load("UI/Info/s_checkpoint.png");

	msgList[MSG::Kill] = RESOURCE->textures.Load("UI/Info/msg_kill.png");
	msgList[MSG::Destroy] = RESOURCE->textures.Load("UI/Info/msg_destroy.png");
	msgList[MSG::Resurrection] = RESOURCE->textures.Load("UI/Info/msg_resurrection.png");
	msgList[MSG::CheckUpdate] = RESOURCE->textures.Load("UI/Info/msg_checkUpdate.png");
}

void InfoMsg::DeleteStaticMember()
{
	for (auto it = subjectList.begin(); it != subjectList.end(); it++)
		it->second = nullptr;

	for (auto it = msgList.begin(); it != msgList.end(); it++)
		it->second = nullptr;
}

void InfoMsg::CalculateStaticMember()
{
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Info/s_goblin.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Info/s_drone.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Info/s_ironColossus.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Info/s_ironColossus_LShoulder.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Info/s_ironColossus_RShoulder.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Info/s_ironColossus_LFoot.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Info/s_ironColossus_RFoot.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Info/s_ironColossus_Head.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Info/s_checkpoint.png"));

	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Info/msg_kill.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Info/msg_destroy.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Info/msg_resurrection.png"));
	RESOURCE->AddTotalCapacity(RESOURCE->textures.Calculate("UI/Info/msg_checkUpdate.png"));
}
