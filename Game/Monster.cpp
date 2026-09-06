#include "stdafx.h"

void Monster::PatternInit()
{
	for (auto it = pattern.begin(); it != pattern.end(); it++)
	{
		it->second.time = it->second.first_cooldown;
		it->second.playing = 0;
	}
}

void Monster::AllPatternUpdate()
{
	//for (auto p : pattern)
	//{
	//	switch (p.second.playing)
	//	{
	//	case 0:	// cooldown time 감소
	//	case 2:	// duration time 감소
	//		if (p.second.time > 0.0f)
	//			p.second.time -= DELTA;
	//		break;
	//	}
	//}
	
	for (auto it = pattern.begin(); it != pattern.end() ; it++)
	{
		switch (it->second.playing)
		{
		case 0:	// cooldown time 감소
		case 2:	// duration time 감소
			if (it->second.time > 0.0f)
				it->second.time -= DELTA;
			break;
		}
	}
}

void Monster::ChangePattern(int patternNum)
{
	// 이전 패턴의 상태 초기화
	if (pattern[currentPattern].playing >= 2)
	{
		// 이미 사용하고 있었다면 쿨타임이 발생
		pattern[currentPattern].time = pattern[currentPattern].cooldown;
		lastPattern = currentPattern;
	}
	else
		pattern[currentPattern].time = 0.0f;
	pattern[currentPattern].mustUse = false;
	pattern[currentPattern].playing = 0;

	// 패턴 갱신
	currentPattern = patternNum;
	pattern[patternNum].time = pattern[patternNum].duration;
	/* playing변수는 패턴이 진짜로 실행될때만 1이상으로 변한다. playing변수를 지정하는 것은 상속받은 몬스터클래스의 몫이다. */
}

bool Monster::RandomPattern()
{
	if (pattern.find(currentPattern) == pattern.end())
		return false;

	vector<int> available;
	available.clear();
	for (auto it = pattern.begin(); it != pattern.end(); it++)
	{
		if (it->second.playing == 0 && it->second.time <= 0.0f && it->second.condition())
			available.push_back(it->first);
	}
	if (available.size() > 0)
	{
		ChangePattern(available[RANDOM->Int(0, available.size() - 1)]);
		return true;
	}
	return false;
}

void Monster::destUpdate(Vector3 pos, Vector3 playerPos, Ray toPlayerRay)
{
	Object* ob;
	int col_Idx = 0;
	Vector3 Hit;

	int idx = Util::GetMyStandMap(pos);			// 자신이 서있는 맵의 번호
	int pidx = Util::GetMyStandMap(playerPos);	// 타겟이 서있는 맵의 번호
	int cidx = idx;							// 현재 검사중인 맵의 번호
	while (idx >= 0 && pidx >= 0)
	{
		col_Idx++;
		if (ob = (*SCENE->Map)[cidx]->Find("Col" + to_string(col_Idx)))
		{
			if (!(ob->collider->gameType & (int)GameType::ONLY_PLAYER) && // 플레이어전용 col은 무시한다
				ob->collider->Intersect(toPlayerRay, Hit))
			{
				// 자신과 타겟사이를 가로막는 벽이 있다.
				if (Vector3::Distance(pos, Hit) < Vector3::Distance(pos, playerPos))
				{
					// 다익스트라 길찾기 사용
					int Start = (*SCENE->Map)[idx]->PickNode(pos);
					int End;
					if (idx == pidx)
					{
						End = (*SCENE->Map)[idx]->PickNode(playerPos);
					}
					else
					{
						// 다른 맵을 방문하기 위한 우회로 검사. 
						// 정해진 우회로가 있으면 자신이 서있는 맵에 등록된 '다른 맵으로 가기 위한' 경유지를 목적지로 설정.
						// 그렇지 않으면 자신이 서있는 맵 노드로 다른맵의 플레이어의 위치를 추적 (사실상 추적포기)
						if ((*SCENE->Map)[idx]->visitOtherMapPos.find(pidx) == (*SCENE->Map)[idx]->visitOtherMapPos.end())
							End = (*SCENE->Map)[idx]->PickNode(playerPos);
						else
							End = (*SCENE->Map)[idx]->PickNode((*SCENE->Map)[idx]->visitOtherMapPos[pidx]);
					}

					// Way는 맨뒤가 Start, 맨앞이 End이다
					(*SCENE->Map)[idx]->PathFinding(Way, Start, End);

					if (Way.size() <= 2)
					{
						// Way 노드가 1~2개만 있다. (경유안하고 직진함)
						Way.clear();

						// 경로상에 점프장애물이 있으면 체크.
						destJump = (ob->collider->gameType & (int)GameType::JUMP) ? true : false;
					}
					else
					{
						// Way 노드가 3개이상이면 첫 노드와 마지막 노드는 무시한다.
						Way.pop_front();
						Way.pop_back();
					}
					Way.push_front(playerPos);
					//Way.push_front(Vector3(0, 0, 0));   // 실질적으로 안쓰는 값. 이곳을 목적지로 설정하고 Way목록에서 삭제하는순간 Way가 비어서 이동안함.
					//P1 = Sphere->GetWorldPos();
					//P2 = Way.back();
					destPos = Way.back();
					Way.pop_back();
					//MoveValue = 0.0f;
					//Vector3 temp = P1 - P2;
					//Dis = temp.Length();
					break;
				}
			}
		}
		else
		{
			if (cidx == pidx)
			{
				// 자신과 타겟사이를 가로막는 벽이 없다.
				Way.clear();
				destPos = playerPos;
				break;
			}
			else
			{
				// 플레이어가 있는 맵의 번호에서도 충돌확인
				cidx = pidx;
				col_Idx = 0;
			}
		}
	}
}

bool Monster::Check_Unbreakable()
{
	// 플레이어가 리타이어하거나 자기가 리타이어하면 unbreakable상태가 된다.
	return PLAYER_UI->GetDying() >= 2 || Check_Death();
}