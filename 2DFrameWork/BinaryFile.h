#pragma once

class BinaryWriter
{
public:
	BinaryWriter();
	~BinaryWriter();

	void Open(wstring filePath, UINT openOption = CREATE_ALWAYS);
	void Close();

	void Bool(bool data);
	void Word(WORD data);
	void Int(int data);
	void UInt(UINT data);
	void Float(float data);
	void Double(double data);

	void vector2(const Vector2& data);
	void vector3(const Vector3& data);
	void vector4(const Vector4& data);
	void color3f(const Color& data);
	void color4f(const Color& data);
	void matrix(const  Matrix& data);

	void String(const string& data);
	void Byte(void* data, UINT dataSize);

protected:
	HANDLE fileHandle;
	DWORD size;
};

//////////////////////////////////////////////////////////////////////////

class BinaryReader
{
public:
	BinaryReader();
	~BinaryReader();

	bool Open(wstring filePath);
	void Close();

	bool Bool(bool b_addCapacity);
	WORD Word(bool b_addCapacity);
	int Int(bool b_addCapacity);
	UINT UInt(bool b_addCapacity);
	float Float(bool b_addCapacity);
	double Double(bool b_addCapacity);

	Vector2 vector2(bool b_addCapacity);
	Vector3 vector3(bool b_addCapacity);
	Vector4 vector4(bool b_addCapacity);
	Color color3f(bool b_addCapacity);
	Color color4f(bool b_addCapacity);
	Matrix matrix(bool b_addCapacity);

	string String(bool b_addCapacity);
	void Byte(void** data, UINT dataSize, bool b_addCapacity);

protected:
	HANDLE fileHandle;
	DWORD size;
};