#pragma once

enum class ObType
{
	Object,
	Actor,
	Camera,
	Terrain,
	UI,
	Billboard,
	Light,
	Rain,
	Pop
};

class Object : public Transform
{
	friend class Actor;

	//static
private:
	static Object*		axis;
	static Material*	defalutMaterial;
	static Shader**		cubeMapShader;
	static Shader**		shadowMapShader;
	static uint64_t CalculateObject(Xml::XMLElement* This, string actorFileName, ObType type);

	// ImGui전용 변수
	int startIndex = 0;
	string searchLog = "";
public:
	static Object* Create(string name = "Object");
	static void CreateStaticMember();
	static void DeleteStaticMember();
	static void CalculateStaticMember();
	//member
protected:
	ObType						type;
	bool						shown;	// 카메라등이나 옵션에 의해 보여지고 있는 상태?
public:
	int                         boneIndex = -1;
	string						name;	//key
	bool						visible;
	class Actor*				root;
	map<string, Object*>		children;
	shared_ptr<Shader>			shader;
	shared_ptr<Mesh>			mesh;
	shared_ptr<Material>		material;
	Collider*					collider;
	//Method
protected:
	Object();
	virtual	~Object();
	void	SaveObject(Xml::XMLElement* This, Xml::XMLDocument* doc);
	void	LoadObject(Xml::XMLElement* This);
public:
	virtual void	Release();
	virtual void	Update();
	virtual void	Render();
	virtual void	CubeMapRender();
	virtual void	ShadowMapRender();
	void			AddChild(Object* child);
	void			AddBone(Object* child);
	bool			RenderHierarchy();
	virtual void	RenderDetail();

	//Getter Setter
	bool			IsShown() { return shown; };
};


class Actor : public Object
{
	friend Object;
private:
	Vector3 animCurrentOffset = { 0.0f,0.0f,0.0f };	// 애니메이션으로 인한 root골격의 이동거리
	Vector3 animLastOffset = { 0.0f,0.0f,0.0f };	// 이전 root골격의 이동거리
	Matrix standardRootAxis = Matrix();
public:
	unordered_map<string, Object*> obList;
	int             boneIndexCount = 1;
	string			file;
	Skeleton*		skeleton;
	Animations*		anim;
	int				rootBoneIndex = -1;
	Vector3			animOffset = { 0.0f,0.0f,0.0f };			// root골격의 1프레임동안 이동한 거리
public:
	static uint64_t CalculateFile(string file);	// 해당 파일이 불러오는 용량을 계산
protected:
	Actor();
	virtual	~Actor();
public:
	virtual void	Release();
	void			ReleaseMember();
	static Actor*	Create(string name = "Actor");
	Object*			Find(string name);
	bool            DeleteObject(string Name);
	void			SaveFile(string file);
	void			LoadFile(string file);
	virtual void	RenderDetail();
	virtual void	Update();
	virtual void    Render();
	virtual void	CubeMapRender();
	virtual void	ShadowMapRender();
	void			ClearAttackTouchedList();	// 이전에 타격된 대상들의 목록을 초기화한다. (재타격 가능)

	void SetStandardMatrix(Matrix m) { standardRootAxis = m; }
};

