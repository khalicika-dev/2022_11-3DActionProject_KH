#include "framework.h"

Animation::Animation()
{
	frameMax = 0;
	boneMax = 0;
	tickPerSecond = 0;
	arrFrameBone = nullptr;
	file = "";
}

Animation::~Animation()
{
	for (UINT i = 0; i < frameMax; i++)
	{
		delete[] arrFrameBone[i];
	}
	delete[] arrFrameBone;
}

uint64_t Animation::CalculateFile(string file)
{
	return Util::CalculateFile("Contents/Animation/" + file);
}


void Animation::LoadFile(string file, bool b_AddCapacity)
{
	this->file = file;
	BinaryReader in;
	wstring path = L"Contents/Animation/" + Util::ToWString(file);
	in.Open(path);

	frameMax = in.Int(b_AddCapacity);
	boneMax = in.Int(b_AddCapacity);
	tickPerSecond = in.Float(b_AddCapacity);

	arrFrameBone = new Matrix * [frameMax];
	for (UINT i = 0; i < frameMax; i++)
	{
		arrFrameBone[i] = new Matrix[boneMax];
	}

	for (UINT i = 0; i < frameMax; i++)
	{
		for (UINT j = 0; j < boneMax; j++)
		{
			arrFrameBone[i][j] = in.matrix(b_AddCapacity);
		}
	}
	in.Close();
}

void Animation::SaveFile(string file)
{
	this->file = file;
	BinaryWriter out;
	wstring path = L"Contents/Animation/" + Util::ToWString(file);
	out.Open(path);

	out.Int(frameMax);
	out.Int(boneMax);
	out.Float(tickPerSecond);

	for (UINT i = 0; i < frameMax; i++)
	{
		for (UINT j = 0; j < boneMax; j++)
		{
			out.matrix(arrFrameBone[i][j]);
		}
	}
	out.Close();
}

void Animations::AnimatorUpdate(Animator& Animator)
{
	Animator.looped = false;
	if (Animator.animState == AnimationState::LOOP)
	{
		Animator.frameWeight += DELTA * playList[Animator.animIdx]->tickPerSecond * aniScale;
		while(Animator.frameWeight >= 1.0f)
		{
			Animator.frameWeight -= 1.0f;
			Animator.currentFrame++;
			Animator.nextFrame++;
			if (Animator.nextFrame >= playList[Animator.animIdx]->frameMax)
			{
				Animator.looped = true;
				Animator.currentFrame = 0;
				Animator.nextFrame = 1;
			}
		}
	}
	else if (Animator.animState == AnimationState::ONCE)
	{
		Animator.frameWeight += DELTA * playList[Animator.animIdx]->tickPerSecond * aniScale;
		while (Animator.frameWeight >= 1.0f)
		{
			Animator.frameWeight -= 1.0f;
			Animator.currentFrame++;
			Animator.nextFrame++;
			if (Animator.nextFrame >= playList[Animator.animIdx]->frameMax)
			{
				//Animator.currentFrame = 0;
				//Animator.nextFrame = 1;
				Animator.nextFrame = 0;
				Animator.frameWeight = 0.0f;
				Animator.animState = AnimationState::STOP;
			}
		}
	}
}

UINT Animations::PlayingIdx()
{
	if(isChanging)
		return nextAnimator.animIdx;
	return currentAnimator.animIdx;
}

bool Animations::IsPlaying()
{
	return !(!isChanging && currentAnimator.animState == AnimationState::STOP);
}

bool Animations::IsLooped()
{
	return (isChanging && nextAnimator.looped) || (!isChanging && currentAnimator.looped);
}

Animations::Animations()
{
	isChanging = false;
}

Animations::~Animations()
{
	for (int i = 0; i < playList.size(); i++)
	{
		SafeReset(playList[i]);
	}
}

void Animations::Update()
{
	if (isChanging)
	{
		AnimatorUpdate(nextAnimator);
		Changedtime += DELTA;
		if (Changedtime > blendtime)
		{
			Changedtime = 0.0f;
			//다음애니메이션을 현재애니메이션으로 바꾼다.
			currentAnimator = nextAnimator;
			isChanging = false;
		}
	}
	AnimatorUpdate(currentAnimator);
}

Matrix Animations::GetFrameBone(int boneIndex, int rootBoneIndex, OUT Vector3& rootOffset)
{
	Matrix M, M1,M2;
	if (isChanging)
	{
		M1 = playList[currentAnimator.animIdx]->arrFrameBone[currentAnimator.currentFrame][boneIndex]
			* (1.0f - Changedtime / blendtime);
		M2 = (playList[nextAnimator.animIdx]->arrFrameBone[nextAnimator.nextFrame][boneIndex]
			* nextAnimator.frameWeight +
			playList[nextAnimator.animIdx]->arrFrameBone[nextAnimator.currentFrame][boneIndex]
			* (1.0f - nextAnimator.frameWeight)) * (Changedtime / blendtime);
		M = M1 + M2;
	}
	else
	{
		M2 = 
			playList[currentAnimator.animIdx]->arrFrameBone[currentAnimator.nextFrame][boneIndex]
			* currentAnimator.frameWeight +
			playList[currentAnimator.animIdx]->arrFrameBone[currentAnimator.currentFrame][boneIndex]
			* (1.0f - currentAnimator.frameWeight);
		M = M2;
	}
	if (boneIndex == rootBoneIndex)
	{
		rootOffset = { M2._41, M2._42, M2._43 };
		//M._41 = M._42 = M._43 = 0.0f;
		M = Matrix::Matrix();
	}
	return M;
}

void Animations::PlayAnimation(AnimationState state, UINT idx, float blendtime)
{
	Changedtime = 0.0f;

	isChanging = true;
	aniScale = 1.0f;

	currentAnimator.animState = AnimationState::STOP;
	currentAnimator.frameWeight = 1.0f;
	nextAnimator.animState = state;
	this->blendtime = blendtime;
	nextAnimator.animIdx = idx;
	nextAnimator.currentFrame = 0;
	nextAnimator.nextFrame = 1;
	nextAnimator.frameWeight = 0.0f;
	nextAnimator.looped = false;
}

void Animations::PlayCurAnimation(AnimationState state)
{
	if (isChanging)
	{
		nextAnimator.animState = state;
		if (nextAnimator.currentFrame >= playList[nextAnimator.animIdx]->frameMax - 1)
		{
			nextAnimator.currentFrame = 0;
			nextAnimator.nextFrame = 1;
			nextAnimator.frameWeight = 0.0f;
			nextAnimator.looped = false;
		}
	}
	else
	{
		currentAnimator.animState = state;
		if (currentAnimator.currentFrame >= playList[currentAnimator.animIdx]->frameMax - 1)
		{
			currentAnimator.currentFrame = 0;
			currentAnimator.nextFrame = 1;
			currentAnimator.frameWeight = 0.0f;
			currentAnimator.looped = false;
		}
	}
}

void Animations::RenderDetail()
{
	ImGui::Text("PlayTime : %f", GetPlayTime());
	ImGui::SliderFloat("AniScale", &aniScale, 0.001f, 10.0f);
	ImGui::SliderFloat("AniBlendTime", &VAR->debugAniBlend, 0.0f, 1.0f);
	if (ImGui::Button("0.0f")) VAR->debugAniBlend = 0.0f; ImGui::SameLine();
	if (ImGui::Button("0.2f")) VAR->debugAniBlend = 0.2f; ImGui::SameLine();
	if (ImGui::Button("0.35f")) VAR->debugAniBlend = 0.35f; ImGui::SameLine();
	if (ImGui::Button("0.5f")) VAR->debugAniBlend = 0.5f; ImGui::SameLine();
	if (ImGui::Button("1.0f")) VAR->debugAniBlend = 1.0f;
	for (UINT i = 0; i < playList.size(); i++)
	{
		string name = to_string(i) + playList[i]->file;
		string button = name + "Stop";
		
		if (ImGui::Button(button.c_str()))
		{
			PlayAnimation(AnimationState::STOP, i, VAR->debugAniBlend);
		}
		ImGui::SameLine();
		button = name + "Once";
		if (ImGui::Button(button.c_str()))
		{
			PlayAnimation(AnimationState::ONCE, i, VAR->debugAniBlend);
		}
		ImGui::SameLine();
		button = name + "Loop";
		if (ImGui::Button(button.c_str()))
		{
			PlayAnimation(AnimationState::LOOP, i, VAR->debugAniBlend);
		}
	}
}

float Animations::GetPlayTime()
{
	if (isChanging)
	{
		return (float)nextAnimator.currentFrame /
			(float)(playList[nextAnimator.animIdx]->frameMax - 1);
	}
	return (float)currentAnimator.currentFrame /
		(float)(playList[currentAnimator.animIdx]->frameMax - 1);
}
