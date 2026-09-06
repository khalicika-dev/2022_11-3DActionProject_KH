#pragma once
class InfoMsg
{
	friend class Player_Interface;
public:
	static void CreateStaticMember();
	static void DeleteStaticMember();
	static void CalculateStaticMember();

	enum class Subject
	{
		Goblin,
		Drone,
		IronColossus,
		IronColossus_LShoulder,
		IronColossus_RShoulder,
		IronColossus_LFoot,
		IronColossus_RFoot,
		IronColossus_Head,
		CheckPoint,
	};

	enum class MSG
	{
		Kill,
		Destroy,
		Resurrection,
		CheckUpdate
	};

private:
	struct InfoText
	{
		Subject subject;
		MSG msg;

		InfoText(Subject subject, MSG msg) : subject(subject), msg(msg) {}
	};
	static map<Subject, shared_ptr<Texture>> subjectList;
	static map<MSG, shared_ptr<Texture>> msgList;
};

