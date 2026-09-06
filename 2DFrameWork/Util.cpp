#include "framework.h"

//////////////////////////////////////////////////////////////////////////
///@brief 해당 문자열에서 comp 문자를 rep로 변경
///@param str : 체크하려는 문자열
///@param comp : 비교문자열
///@param rep : 바꿀문자열
//////////////////////////////////////////////////////////////////////////
void Util::Replace(string* str, string comp, string rep)
{
	string temp = *str;

	size_t start_pos = 0;
	while ((start_pos = temp.find(comp, start_pos)) != wstring::npos)
	{
		temp.replace(start_pos, comp.length(), rep);
		start_pos += rep.length();
	}

	*str = temp;
}

void Util::Replace(wstring* str, wstring comp, wstring rep)
{
	wstring temp = *str;

	size_t start_pos = 0;
	while ((start_pos = temp.find(comp, start_pos)) != wstring::npos)
	{
		temp.replace(start_pos, comp.length(), rep);
		start_pos += rep.length();
	}

	*str = temp;
}

//////////////////////////////////////////////////////////////////////////
///@brief string형을 wstring형으로 변경
///@param value : 변환할 문자열
///@return 변환 완료 문자열
//////////////////////////////////////////////////////////////////////////
wstring Util::ToWString(string value)
{
	wstring temp = L"";
	temp.assign(value.begin(), value.end());

	return temp;
}

//////////////////////////////////////////////////////////////////////////
///@brief wstring형을 string형으로 변경
///@param value : 변환할 문자열
///@return 변환 완료 문자열
//////////////////////////////////////////////////////////////////////////
string Util::ToString(wstring value)
{
	string temp = "";
	temp.assign(value.begin(), value.end());

	return temp;
}

float Util::NormalizeAngle(float angle)
{
	while (angle > PI)
		angle -= PI_2;
	while (angle < -PI)
		angle += PI_2;
	return angle;
}

Vector3 Util::NormalizeAngles(Vector3 angles)
{
	angles.x = NormalizeAngle(angles.x);
	angles.y = NormalizeAngle(angles.y);
	angles.z = NormalizeAngle(angles.z);
	return angles;
}

Vector3 Util::QuaternionToYawPtichRoll(Quaternion q1)
{
	float sqw = q1.w * q1.w;
	float sqx = q1.x * q1.x;
	float sqy = q1.y * q1.y;
	float sqz = q1.z * q1.z;
	float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor

	float test = q1.x * q1.w - q1.y * q1.z;
	Vector3 v;
	if (test > 0.4995f * unit) { // singularity at north pole

		v.y = 2.0f * atan2f(q1.y, q1.x);
		v.x = PI / 2.0f;
		v.z = 0;
		return NormalizeAngles(v);
	}

	if (test < -0.4995f * unit) { // singularity at south pole

		v.y = -2.0f * atan2f(q1.y, q1.x);
		v.x = -PI / 2.0f;
		v.z = 0;
		return NormalizeAngles(v);
	}

	Quaternion q = Quaternion(q1.w, q1.z, q1.x, q1.y);
	// Yaw
	v.y = atan2f(2.0f * q.x * q.w + 2.0f * q.y * q.z, 1.0f - 2.0f * (q.z * q.z + q.w * q.w));
	// Pitch
	v.x = asinf(2.0f * (q.x * q.z - q.w * q.y));
	// Roll
	v.z = atan2f(2.0f * q.x * q.y + 2.0f * q.z * q.w, 1.0f - 2.0f * (q.y * q.y + q.z * q.z));

	return NormalizeAngles(v);
}

bool Util::RayIntersectTri(IN Ray WRay, IN Object* Target, OUT Vector3& HitPoint)
{
	if (not Target->mesh)return false;

	Matrix inverse = Target->W.Invert();
	WRay.direction = Vector3::TransformNormal(WRay.direction, inverse);
	WRay.direction.Normalize();

	WRay.position = Vector3::Transform(WRay.position, inverse);

	for (UINT i = 0; i < Target->mesh->GetIndexCount(); i += 3)
	{
		Vector3 v[3];
		v[0] = Target->mesh->GetVertexPosition(i);
		v[1] = Target->mesh->GetVertexPosition(i + 1);
		v[2] = Target->mesh->GetVertexPosition(i + 2);

		float Dis;
		if (WRay.Intersects(v[0], v[1], v[2], Dis))
		{
			//                         스칼라 x 방향
			HitPoint = WRay.position +  (WRay.direction * Dis);
			//다시 W 로 변환
			HitPoint = Vector3::Transform(HitPoint, Target->W);
			return true;
		}
	}
	return false;


}

bool Util::RayIntersectTriNear(IN Ray WRay, IN Object* Target, OUT Vector3& HitPoint)
{
	if (not Target->mesh)return false;

	Matrix inverse = Target->W.Invert();
	WRay.direction = Vector3::TransformNormal(WRay.direction, inverse);
	WRay.direction.Normalize();
	WRay.position = Vector3::Transform(WRay.position, inverse);

	float Dis = FLT_MAX;
	for (UINT i = 0; i < Target->mesh->GetIndexCount(); i += 3)
	{
		Vector3 v[3];
		v[0] = Target->mesh->GetVertexPosition(i);
		v[1] = Target->mesh->GetVertexPosition(i + 1);
		v[2] = Target->mesh->GetVertexPosition(i + 2);

		float temp;
		if (WRay.Intersects(v[0], v[1], v[2], temp))
		{
			if (Dis > temp)
			{
				Dis = temp;
				//                         스칼라 x 방향
				HitPoint = WRay.position + (WRay.direction * temp);
				//다시 W 로 변환
				HitPoint = Vector3::Transform(HitPoint, Target->W);
			}
		}
	}
	if (Dis == FLT_MAX)
		return false;
	else
		return true;
}

bool Util::RayIntersectMap(IN Ray WRay, IN Object* Terrain, OUT Vector3& HitPoint)
{
	if (not Terrain->mesh)return false;
	//역행렬 구하기
	Matrix inverse = Terrain->W.Invert();

	//w=0
	WRay.direction = Vector3::TransformNormal(WRay.direction, inverse);
	WRay.direction.Normalize();
	//w=1
	WRay.position = Vector3::Transform(WRay.position, inverse);

	//Terrain 중심점 0,0,0이 가운데가 아닌 왼쪽상단이 0,0,0이 되게끔 이동

	int terrainSize = (int)sqrt(Terrain->mesh->vertexCount);
	float half = terrainSize * 0.5f;
	int TerrainIdxX , TerrainIdxZ;
	TerrainIdxX = WRay.position.x + half;
	TerrainIdxZ = -(WRay.position.z - half);

	if (TerrainIdxX < 0 or TerrainIdxZ < 0 or
		TerrainIdxX >= terrainSize - 1 or TerrainIdxZ >= terrainSize - 1)
	{
		//cout << "OutofRange" << endl;
		return false;
	}
	//사각형 인덱스
	//int index = (terrainSize - 1) * TerrainIdxZ + TerrainIdxX;
	for (int ix = TerrainIdxX - 2; ix < TerrainIdxX + 2; ix++)
	{
		if (ix < 0 || ix >= terrainSize - 1) continue;
		for (int iz = TerrainIdxZ - 2; iz < TerrainIdxZ + 2; iz++)
		{
			if (iz < 0 || iz >= terrainSize - 1) continue;
			int index = (terrainSize - 1) * iz + ix;

			//사각형의 첫번째 정점 인덱스
			index *= 6;

			for (int i = 0; i < 6; i += 3)
			{
				Vector3 v[3];
				v[0] = Terrain->mesh->GetVertexPosition(index + i);
				v[1] = Terrain->mesh->GetVertexPosition(index + i + 1);
				v[2] = Terrain->mesh->GetVertexPosition(index + i + 2);

				float Dis;
				if (WRay.Intersects(v[0], v[1], v[2], Dis))
				{
					//                         스칼라 x 방향
					HitPoint = WRay.position + Dis * WRay.direction;
					//다시 W 로 변환
					HitPoint = Vector3::Transform(HitPoint, Terrain->W);

					UINT* test = &Terrain->mesh->indices[index + 1];
					return true;
				}
			}
		}
	}
	return false;
}

int Util::GetMyStandMap(Vector3 worldPos)
{
	//for (auto m : *SCENE->Map)
	for (int i = 0; i < SCENE->Map->size(); i++)
	{
		Vector3 pos = worldPos;
		//역행렬 구하기
		Matrix inverse = (*SCENE->Map)[i]->W.Invert();

		//w=1
		pos = Vector3::Transform(pos, inverse);

		//Terrain 중심점 0,0,0이 가운데가 아닌 왼쪽상단이 0,0,0이 되게끔 이동

		int terrainSize = (int)sqrt((*SCENE->Map)[i]->mesh->vertexCount);
		float half = terrainSize * 0.5f;
		float TX, TZ;
		TX = pos.x + half;
		TZ = -(pos.z - half);

		//if (TerrainIdxX < 0 or TerrainIdxZ < 0 or
		//	TerrainIdxX >= terrainSize - 1 or TerrainIdxZ >= terrainSize - 1)
		//{
		//	//cout << "OutofRange" << endl;
		//	continue;
		//}
		if (TX < 0 or TZ < 0 or
			TX >= terrainSize - 1 or TZ >= terrainSize - 1)
		{
			//cout << "OutofRange" << endl;
			continue;
		}
		return i;
	}
	return -1;
}

Ray Util::MouseToRay(Vector3 Mouse, Camera* Cam)
{
	Mouse.x -= Cam->viewport.x;
	Mouse.y -= Cam->viewport.y;
	Vector2 MousePos;

	//ndc로의 변환
	MousePos.x = ((2.0f * Mouse.x) / Cam->viewport.width - 1.0f);
	MousePos.y = ((-2.0f * Mouse.y) / Cam->viewport.height + 1.0f);

	// view로의 변환
	MousePos.x /= Cam->proj._11;
	MousePos.y /= Cam->proj._22;

	Ray CamToMouse;
	CamToMouse.position = Cam->GetWorldPos();
	CamToMouse.direction = Vector3(MousePos.x, MousePos.y, 1.0f);
	Matrix inverse = Cam->view.Invert();
	CamToMouse.direction = Vector3::TransformNormal(CamToMouse.direction, inverse);
	CamToMouse.direction.Normalize();
	return CamToMouse;
}

uint64_t Util::CalculateFile(string path)
{
	uint64_t rst = 0;
	FILE* fp = NULL;
	fopen_s(&fp, path.c_str(), "rb");
	assert(fp != NULL);

	fseek(fp, 0, SEEK_END);
	rst = _ftelli64(fp);
	fclose(fp);
	return rst;
}