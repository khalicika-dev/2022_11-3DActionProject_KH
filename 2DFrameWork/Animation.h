#pragma once
class Animation
{
public:
	UINT					frameMax;
	UINT					boneMax;
	Matrix**				arrFrameBone;//	프레임갯수* 본갯수
	float					tickPerSecond;
	string					file;


	Animation();
	~Animation();
	static uint64_t CalculateFile(string file);
	void LoadFile(string file, bool b_AddCapacity);
	void SaveFile(string file);
};
enum class AnimationState
{
	LOOP,
	ONCE,
	STOP//Pause
};

class Animations
{
	struct Animator
	{
		float frameWeight = 0.0f;
		UINT  currentFrame = 0;
		UINT  nextFrame = 1;
		UINT  animIdx = 0;
		bool  looped = false;
		AnimationState animState = AnimationState::STOP;
	};
	void AnimatorUpdate(Animator& Animator);
public:
	Animations();
	~Animations();
	void Update();
	Animator							currentAnimator;
	Animator							nextAnimator;
	bool								isChanging;
	float								blendtime;
	float								Changedtime;
	float								aniScale = 1.0f;
	vector<shared_ptr<Animation>>		playList;
	Matrix	GetFrameBone(int boneIndex, int rootBoneIndex, OUT Vector3& rootOffset);
	void	PlayAnimation(AnimationState state, UINT idx, float blendtime = 0.2f);
	void	PlayCurAnimation(AnimationState state);
	void	RenderDetail();
	float   GetPlayTime();	// 0처음 ~ 1 끝

	UINT	PlayingIdx();
	bool	IsPlaying();
	bool	IsLooped();
};