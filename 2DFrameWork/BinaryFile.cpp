#include "Framework.h"

//////////////////////////////////////////////////////////////////////////

BinaryWriter::BinaryWriter()
	: fileHandle(NULL), size(0)
{

}

BinaryWriter::~BinaryWriter()
{

}

void BinaryWriter::Open(wstring filePath, UINT openOption)
{
	assert(filePath.length() > 0);
	fileHandle = CreateFile
	(
		filePath.c_str()
		, GENERIC_WRITE
		, 0
		, NULL
		, openOption
		, FILE_ATTRIBUTE_NORMAL
		, NULL
	);


	bool isChecked = fileHandle != INVALID_HANDLE_VALUE;
	assert(isChecked);
}

void BinaryWriter::Close()
{
	if (fileHandle != NULL)
	{
		CloseHandle(fileHandle);
		fileHandle = NULL;
	}
}

void BinaryWriter::Bool(bool data)
{
	WriteFile(fileHandle, &data, sizeof(bool), &size, NULL);
}

void BinaryWriter::Word(WORD data)
{
	WriteFile(fileHandle, &data, sizeof(WORD), &size, NULL);
}

void BinaryWriter::Int(int data)
{
	WriteFile(fileHandle, &data, sizeof(int), &size, NULL);
}

void BinaryWriter::UInt(UINT data)
{
	WriteFile(fileHandle, &data, sizeof(UINT), &size, NULL);
}

void BinaryWriter::Float(float data)
{
	WriteFile(fileHandle, &data, sizeof(float), &size, NULL);
}

void BinaryWriter::Double(double data)
{
	WriteFile(fileHandle, &data, sizeof(double), &size, NULL);
}

void BinaryWriter::vector2(const Vector2& data)
{
	WriteFile(fileHandle, &data, sizeof(Vector2), &size, NULL);
}

void BinaryWriter::vector3(const Vector3& data)
{
	WriteFile(fileHandle, &data, sizeof(Vector3), &size, NULL);
}

void BinaryWriter::vector4(const Vector4& data)
{
	WriteFile(fileHandle, &data, sizeof(Vector4), &size, NULL);
}

void BinaryWriter::color3f(const Color& data)
{
	WriteFile(fileHandle, &data, sizeof(Color) - 4, &size, NULL);
}

void BinaryWriter::color4f(const Color& data)
{
	WriteFile(fileHandle, &data, sizeof(Color), &size, NULL);
}

void BinaryWriter::matrix(const Matrix& data)
{
	WriteFile(fileHandle, &data, sizeof(Matrix), &size, NULL);
}

void BinaryWriter::String(const string & data)
{
	UInt((UINT)data.size());

	const char* str = data.c_str();
	WriteFile(fileHandle, str, (DWORD)data.size(), &size, NULL);
}

void BinaryWriter::Byte(void * data, UINT dataSize)
{
	WriteFile(fileHandle, data, dataSize, &size, NULL);
}

//////////////////////////////////////////////////////////////////////////

BinaryReader::BinaryReader()
	: fileHandle(NULL), size(0)
{

}

BinaryReader::~BinaryReader()
{

}

bool BinaryReader::Open(wstring filePath)
{
	if (filePath.length() <= 0)
	{
		return false;
	}
	assert(filePath.length() > 0);
	fileHandle = CreateFile
	(
		filePath.c_str()
		, GENERIC_READ
		, FILE_SHARE_READ
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL
	);


	bool isChecked = fileHandle != INVALID_HANDLE_VALUE;
	assert(isChecked);
	return isChecked;
}

void BinaryReader::Close()
{
	if (fileHandle != NULL)
	{
		CloseHandle(fileHandle);
		fileHandle = NULL;
	}
}

bool BinaryReader::Bool(bool b_addCapacity)
{
	bool temp = false;
	if (ReadFile(fileHandle, &temp, sizeof(bool), &size, NULL))
		RESOURCE->AddCurrentCapacity(b_addCapacity, size);
	return temp;
}

WORD BinaryReader::Word(bool b_addCapacity)
{
	WORD temp = 0;
	if (ReadFile(fileHandle, &temp, sizeof(WORD), &size, NULL))
		RESOURCE->AddCurrentCapacity(b_addCapacity, size);
	return temp;
}

int BinaryReader::Int(bool b_addCapacity)
{
	int temp = 0;
	if (ReadFile(fileHandle, &temp, sizeof(int), &size, NULL))
		RESOURCE->AddCurrentCapacity(b_addCapacity, size);
	return temp;
}

UINT BinaryReader::UInt(bool b_addCapacity)
{
	UINT temp = 0;
	if (ReadFile(fileHandle, &temp, sizeof(UINT), &size, NULL))
		RESOURCE->AddCurrentCapacity(b_addCapacity, size);
	return temp;
}

float BinaryReader::Float(bool b_addCapacity)
{
	float temp = 0.0f;
	if (ReadFile(fileHandle, &temp, sizeof(float), &size, NULL))
		RESOURCE->AddCurrentCapacity(b_addCapacity, size);
	return temp;
}

double BinaryReader::Double(bool b_addCapacity)
{
	double temp = 0.0f;
	if (ReadFile(fileHandle, &temp, sizeof(double), &size, NULL))
		RESOURCE->AddCurrentCapacity(b_addCapacity, size);
	return temp;
}

Vector2 BinaryReader::vector2(bool b_addCapacity)
{
	float x = Float(b_addCapacity);
	float y = Float(b_addCapacity);

	return Vector2(x, y);
}

Vector3 BinaryReader::vector3(bool b_addCapacity)
{
	float x = Float(b_addCapacity);
	float y = Float(b_addCapacity);
	float z = Float(b_addCapacity);

	return Vector3(x, y, z);
}

Vector4 BinaryReader::vector4(bool b_addCapacity)
{
	float x = Float(b_addCapacity);
	float y = Float(b_addCapacity);
	float z = Float(b_addCapacity);
	float w = Float(b_addCapacity);

	return Vector4(x, y, z, w);
}

Color BinaryReader::color3f(bool b_addCapacity)
{
	float r = Float(b_addCapacity);
	float g = Float(b_addCapacity);
	float b = Float(b_addCapacity);

	return Color(r, g, b, 1.0f);
}

Color BinaryReader::color4f(bool b_addCapacity)
{
	float r = Float(b_addCapacity);
	float g = Float(b_addCapacity);
	float b = Float(b_addCapacity);
	float a = Float(b_addCapacity);

	return Color(r, g, b, a);
}

Matrix BinaryReader::matrix(bool b_addCapacity)
{
	Matrix matrix;
	matrix._11 = Float(b_addCapacity);	matrix._12 = Float(b_addCapacity);	matrix._13 = Float(b_addCapacity);	matrix._14 = Float(b_addCapacity);
	matrix._21 = Float(b_addCapacity);	matrix._22 = Float(b_addCapacity);	matrix._23 = Float(b_addCapacity);	matrix._24 = Float(b_addCapacity);
	matrix._31 = Float(b_addCapacity);	matrix._32 = Float(b_addCapacity);	matrix._33 = Float(b_addCapacity);	matrix._34 = Float(b_addCapacity);
	matrix._41 = Float(b_addCapacity);	matrix._42 = Float(b_addCapacity);	matrix._43 = Float(b_addCapacity);	matrix._44 = Float(b_addCapacity);

	return matrix;
}

string BinaryReader::String(bool b_addCapacity)
{
	UINT size = Int(b_addCapacity);

	char* temp = new char[size + 1];
	if (ReadFile(fileHandle, temp, sizeof(char) * size, &this->size, NULL)) //데이터 읽기
		RESOURCE->AddCurrentCapacity(b_addCapacity, this->size);
	temp[size] = '\0';
	return temp;
}

void BinaryReader::Byte(void ** data, UINT dataSize, bool b_addCapacity)
{
	if (ReadFile(fileHandle, *data, dataSize, &size, NULL))
		RESOURCE->AddCurrentCapacity(b_addCapacity, size);
}
