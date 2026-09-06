#pragma once
class Mesh
{
    //정점(꼭지점),정점을 잇는 타입(선,점,면),

    //점들이 갖는멤버값
    //기준점에서부터의 좌표(상대좌표) x,y,z,w
    //색
    //텍스처 좌표 (uv)
    friend class Object;
    friend class Rain;
    friend class Pop;
private:
    ID3D11Buffer*           vertexBuffer;
    ID3D11Buffer*           indexBuffer;
  
    D3D_PRIMITIVE_TOPOLOGY  primitiveTopology;
    VertexType              vertexType;
    UINT                    byteWidth;

  
public:
    UINT                    vertexCount;
    UINT*                   indices;
    UINT                    indexCount;
    void*                   vertices;
    string                  file;
public:
    Mesh();
    Mesh(void* vertices, UINT vertexCount, UINT* indices,
        UINT indexCount, VertexType type, D3D_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ~Mesh();
    static uint64_t CalculateFile(string file);

    void Set(); // binding
    void LoadFile(string file, bool b_AddCapacity);
    void SaveFile(string file);
    UINT GetIndexCount() { return indexCount; }
    UINT GetVertexCount() { return vertexCount; }
    const Vector3& GetVertexPosition(UINT idx, bool bFollowIndices = true);
    Vector3& SetVertexPosition(UINT idx, bool bFollowIndices = true);
    VertexType GetVertexType() { return vertexType; }
    void UpdateMesh();
    void OptimizeMesh();
};

