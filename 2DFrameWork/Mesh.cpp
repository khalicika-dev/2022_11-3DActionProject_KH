#include "framework.h"
Mesh::Mesh()
    :vertices(nullptr), indices(nullptr)
    , vertexBuffer(nullptr), indexBuffer(nullptr)
{
    /*
    vertexType = VertexType::PTN;
    primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    VertexPTN* Vertex;
    byteWidth = sizeof(VertexPTN);
    //file = "3.TextureCube.mesh";
    file = "3.TextureCube3.mesh";

    vertexCount = 24;
    indexCount = 36;

    Vertex = new VertexPTN[vertexCount];
    indices = new UINT[indexCount];

    //     +y
    //  -x +z +x -z
    //     -y

    // +y(맨위, 기준)
    indices[0] = 0;
    indices[1] = 2;
    indices[2] = 1;

    indices[3] = 1;
    indices[4] = 2;
    indices[5] = 3;
    // 좌상
    Vertex[0].position = Vector3(-1.0f, 1.0f, 1.0f);
    Vertex[0].uv = Vector2(0, 0);
    Vertex[0].normal = Vector3(0, 1, 0);
    Vertex[0].normal.Normalize();

    // 좌하
    Vertex[1].position = Vector3(-1.0f, 1.0f, -1.0f);
    Vertex[1].uv = Vector2(0, 0.5f);
    Vertex[1].normal = Vector3(0, 1, 0);
    Vertex[1].normal.Normalize();

    // 우상
    Vertex[2].position = Vector3(1.0f, 1.0f, 1.0f);
    Vertex[2].uv = Vector2(0.5f, 0);
    Vertex[2].normal = Vector3(0, 1, 0);
    Vertex[2].normal.Normalize();

    // 우하
    Vertex[3].position = Vector3(1.0f, 1.0f, -1.0f);
    Vertex[3].uv = Vector2(0.5f, 0.5f);
    Vertex[3].normal = Vector3(0, 1, 0);
    Vertex[3].normal.Normalize();

    // +z
    indices[6] = 4;
    indices[7] = 6;
    indices[8] = 5;

    indices[9] = 5;
    indices[10] = 6;
    indices[11] = 7;
    // 좌상
    Vertex[4].position = Vector3(-1.0f, 1.0f, -1.0f);
    Vertex[4].uv = Vector2(0, 0);
    Vertex[4].normal = Vector3(0, 0, -1);
    Vertex[4].normal.Normalize();

    // 좌하
    Vertex[5].position = Vector3(-1.0f, -1.0f, -1.0f);
    Vertex[5].uv = Vector2(0, 1);
    Vertex[5].normal = Vector3(0, 0, -1);
    Vertex[5].normal.Normalize();

    // 우상
    Vertex[6].position = Vector3(1.0f, 1.0f, -1.0f);
    Vertex[6].uv = Vector2(1, 0);
    Vertex[6].normal = Vector3(0, 0, -1);
    Vertex[6].normal.Normalize();

    // 우하
    Vertex[7].position = Vector3(1.0f, -1.0f, -1.0f);
    Vertex[7].uv = Vector2(1, 1);
    Vertex[7].normal = Vector3(0, 0, -1);
    Vertex[7].normal.Normalize();

    // -y
    indices[12] = 8;
    indices[13] = 10;
    indices[14] = 9;

    indices[15] = 9;
    indices[16] = 10;
    indices[17] = 11;
    // 좌상
    Vertex[8].position = Vector3(-1.0f, -1.0f, -1.0f);
    Vertex[8].uv = Vector2(0, 0);
    Vertex[8].normal = Vector3(0, -1, 0);
    Vertex[8].normal.Normalize();

    // 좌하
    Vertex[9].position = Vector3(-1.0f, -1.0f, 1.0f);
    Vertex[9].uv = Vector2(0, 1);
    Vertex[9].normal = Vector3(0, -1, 0);
    Vertex[9].normal.Normalize();

    // 우상
    Vertex[10].position = Vector3(1.0f, -1.0f, -1.0f);
    Vertex[10].uv = Vector2(1, 0);
    Vertex[10].normal = Vector3(0, -1, 0);
    Vertex[10].normal.Normalize();

    // 우하
    Vertex[11].position = Vector3(1.0f, -1.0f, 1.0f);
    Vertex[11].uv = Vector2(1, 1);
    Vertex[11].normal = Vector3(0, -1, 0);
    Vertex[11].normal.Normalize();

    // -x
    indices[18] = 12;
    indices[19] = 14;
    indices[20] = 13;

    indices[21] = 13;
    indices[22] = 14;
    indices[23] = 15;
    // 좌상
    Vertex[12].position = Vector3(-1.0f, 1.0f, 1.0f);
    Vertex[12].uv = Vector2(0, 0);
    Vertex[12].normal = Vector3(-1, 0, 0);
    Vertex[12].normal.Normalize();

    // 좌하
    Vertex[13].position = Vector3(-1.0f, -1.0f, 1.0f);
    Vertex[13].uv = Vector2(0, 1);
    Vertex[13].normal = Vector3(-1, 0, 0);
    Vertex[13].normal.Normalize();

    // 우상
    Vertex[14].position = Vector3(-1.0f, 1.0f, -1.0f);
    Vertex[14].uv = Vector2(1, 0);
    Vertex[14].normal = Vector3(-1, 0, 0);
    Vertex[14].normal.Normalize();

    // 우하
    Vertex[15].position = Vector3(-1.0f, -1.0f, -1.0f);
    Vertex[15].uv = Vector2(1, 1);
    Vertex[15].normal = Vector3(-1, 0, 0);
    Vertex[15].normal.Normalize();

    // +x
    indices[24] = 16;
    indices[25] = 18;
    indices[26] = 17;

    indices[27] = 17;
    indices[28] = 18;
    indices[29] = 19;
    // 좌상
    Vertex[16].position = Vector3(1.0f, 1.0f, -1.0f);
    Vertex[16].uv = Vector2(0, 0);
    Vertex[16].normal = Vector3(1, 0, 0);
    Vertex[16].normal.Normalize();

    // 좌하
    Vertex[17].position = Vector3(1.0f, -1.0f, -1.0f);
    Vertex[17].uv = Vector2(0, 1);
    Vertex[17].normal = Vector3(1, 0, 0);
    Vertex[17].normal.Normalize();

    // 우상
    Vertex[18].position = Vector3(1.0f, 1.0f, 1.0f);
    Vertex[18].uv = Vector2(1, 0);
    Vertex[18].normal = Vector3(1, 0, 0);
    Vertex[18].normal.Normalize();

    // 우하
    Vertex[19].position = Vector3(1.0f, -1.0f, 1.0f);
    Vertex[19].uv = Vector2(1, 1);
    Vertex[19].normal = Vector3(1, 0, 0);
    Vertex[19].normal.Normalize();

    // -z
    indices[30] = 20;
    indices[31] = 22;
    indices[32] = 21;

    indices[33] = 21;
    indices[34] = 22;
    indices[35] = 23;
    // 좌상
    Vertex[20].position = Vector3(1.0f, 1.0f, 1.0f);
    Vertex[20].uv = Vector2(0, 0);
    Vertex[20].normal = Vector3(0, 0, 1);
    Vertex[20].normal.Normalize();

    // 좌하
    Vertex[21].position = Vector3(1.0f, -1.0f, 1.0f);
    Vertex[21].uv = Vector2(0, 1);
    Vertex[21].normal = Vector3(0, 0, 1);
    Vertex[21].normal.Normalize();

    // 우상
    Vertex[22].position = Vector3(-1.0f, 1.0f, 1.0f);
    Vertex[22].uv = Vector2(1, 0);
    Vertex[22].normal = Vector3(0, 0, 1);
    Vertex[22].normal.Normalize();

    // 우하
    Vertex[23].position = Vector3(-1.0f, -1.0f, 1.0f);
    Vertex[23].uv = Vector2(1, 1);
    Vertex[23].normal = Vector3(0, 0, 1);
    Vertex[23].normal.Normalize();
    */

    ////////////////////////////////////////////////////
    /*vertexType = VertexType::P;
    primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


    VertexP* Vertex;
    byteWidth = sizeof(VertexP);
    file = "0.Sphere.mesh";*/
    //vertexCount = 1;
    //indexCount = 1;
    //

    //Vertex = new VertexPSP[vertexCount];
    //indices = new UINT[indexCount];

    //Vertex[0].position = Vector3(-0.5f, -0.5f, 0.1f);
    //Vertex[0].size = Vector2(0.0f, 1.0f);
    //Vertex[0].pivot = Vector2(0.0f, 1.0f);
    //indices[0] = 0;


   /* vector<VertexP> v;
    v.push_back(VertexP(Vector3(0, 1, 0)));

    int stackCount = 36;
    int sliceCount = 72;
    float phiStep = XM_PI / stackCount;
    float thetaStep = 2.0f * XM_PI / sliceCount;

    for (UINT i = 1; i <= stackCount - 1; i++)
    {
        float phi = i * phiStep;

        for (UINT j = 0; j <= sliceCount; j++)
        {
            float theta = j * thetaStep;

            Vector3 p = Vector3
            (
                (sinf(phi) * cosf(theta)),
                (cosf(phi)),
                (sinf(phi) * sinf(theta))
            );
            v.push_back(VertexP(p));
        }

    }
    v.push_back(VertexP(Vector3(0, -1, 0)));

    Vertex = new VertexP[v.size()];
    vertexCount = v.size();
    copy(v.begin(), v.end(), stdext::checked_array_iterator<VertexP*>(Vertex, vertexCount));

    vector<UINT> vecindices;

    for (UINT i = 1; i <= sliceCount; i++)
    {
        vecindices.push_back(0);
        vecindices.push_back(i + 1);
        vecindices.push_back(i);
    }

    UINT baseIndex = 1;
    UINT ringVertexCount = sliceCount + 1;
    for (UINT i = 0; i < stackCount - 2; i++)
    {
        for (UINT j = 0; j < sliceCount; j++)
        {
            vecindices.push_back(baseIndex + i * ringVertexCount + j);
            vecindices.push_back(baseIndex + i * ringVertexCount + j + 1);
            vecindices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

            vecindices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
            vecindices.push_back(baseIndex + i * ringVertexCount + j + 1);
            vecindices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
        }
    }

    UINT southPoleIndex = v.size() - 1;
    baseIndex = southPoleIndex - ringVertexCount;

    for (UINT i = 0; i < sliceCount; i++)
    {
        vecindices.push_back(southPoleIndex);
        vecindices.push_back(baseIndex + i);
        vecindices.push_back(baseIndex + i + 1);
    }

    this->indices = new UINT[vecindices.size()];
    indexCount = vecindices.size();
    copy(vecindices.begin(), vecindices.end(), stdext::checked_array_iterator<UINT*>(this->indices, indexCount));*/




    /*
    ///////////////////////////////////////////////////////
    vertices = (void*)Vertex;
    //CreateVertexBuffer
    {
        D3D11_BUFFER_DESC desc;
        desc = { 0 };
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = byteWidth * vertexCount;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data = { 0 };
        data.pSysMem = vertices;

        HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
        assert(SUCCEEDED(hr));
    }

    //Create Index Buffer
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
        desc.ByteWidth = sizeof(UINT) * indexCount;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data = { 0 };
        data.pSysMem = indices;

        HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
        assert(SUCCEEDED(hr));
    }
    //저장용
    SaveFile(file);
    */
    
}

Mesh::Mesh(void* vertices, UINT vertexCount, UINT* indices, UINT indexCount, VertexType type, D3D_PRIMITIVE_TOPOLOGY primitiveTopology)
{
    vertexType = type;
    this->primitiveTopology = primitiveTopology;

    switch (type)
    {
    case VertexType::P:
        byteWidth = sizeof(VertexP);
        break;
    case VertexType::PC:
        byteWidth = sizeof(VertexPC);
        break;
    case VertexType::PCN:
        byteWidth = sizeof(VertexPCN);
        break;
    case VertexType::PTN:
        byteWidth = sizeof(VertexPTN);
        break;
    case VertexType::MODEL:
        byteWidth = sizeof(VertexModel);
        break;
    case VertexType::TERRAIN:
        byteWidth = sizeof(VertexTerrain);
        break;
    case VertexType::PT:
        byteWidth = sizeof(VertexPT);
        break;
    case VertexType::PS:
        byteWidth = sizeof(VertexPS);
        break;
    case VertexType::PSV:
        byteWidth = sizeof(VertexPSV);
        break;
    case VertexType::PSP:
        byteWidth = sizeof(VertexPSP);
        break;
    }
    ////////////////////////////////////////////////////

    ///////////////////////////////////////////////////
    this->vertices = vertices;
    this->vertexCount = vertexCount;
    this->indices = indices;
    this->indexCount = indexCount;

    //CreateVertexBuffer
    {
        D3D11_BUFFER_DESC desc;
        desc = { 0 };
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = byteWidth * vertexCount;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data = { 0 };
        data.pSysMem = vertices;

        HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
        assert(SUCCEEDED(hr));
    }

    //Create Index Buffer
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
        desc.ByteWidth = sizeof(UINT) * indexCount;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data = { 0 };
        data.pSysMem = indices;

        HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
        assert(SUCCEEDED(hr));
    }
}


Mesh::~Mesh()
{
    switch (vertexType)
    {
    case VertexType::P:
        delete[](VertexP*)vertices;
        break;
    case VertexType::PC:
        delete[](VertexPC*)vertices;
        break;
    case VertexType::PCN:
        delete[](VertexPCN*)vertices;
        break;
    case VertexType::PTN:
        delete[](VertexPTN*)vertices;
        break;
    case VertexType::MODEL:
        delete[](VertexModel*)vertices;
        break;
    case VertexType::TERRAIN:
        delete[](VertexTerrain*)vertices;
        break;
    case VertexType::PT:
        delete[](VertexPT*)vertices;
        break;
    case VertexType::PS:
        delete[](VertexPS*)vertices;
        break;
    case VertexType::PSV:
        delete[](VertexPSV*)vertices;
        break;
    case VertexType::PSP:
        delete[](VertexPSP*)vertices;
        break;
    }
    SafeRelease(vertexBuffer);
    SafeRelease(indexBuffer);
    SafeDeleteArray(indices);
}

uint64_t Mesh::CalculateFile(string file)
{
    return Util::CalculateFile("Contents/Mesh/" + file);
}

void Mesh::Set()
{
    UINT offset = 0;
    D3D->GetDC()->IASetVertexBuffers(0,
        1,
        &vertexBuffer,
        &byteWidth,
        &offset);
    D3D->GetDC()->IASetPrimitiveTopology
    (primitiveTopology);
    D3D->GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void Mesh::LoadFile(string file, bool b_AddCapacity)
{
    this->file = file;


    BinaryReader in;
    wstring path = L"Contents/Mesh/" + Util::ToWString(file);
    in.Open(path);

    vertexType = (VertexType)in.UInt(b_AddCapacity);
    
    //읽기전
    switch (vertexType)
    {
    case VertexType::P:
        if (vertices)delete[](VertexP*)vertices;
        break;
    case VertexType::PC:
        if (vertices)delete[](VertexPC*)vertices;
        break;
    case VertexType::PCN:
        if (vertices)delete[](VertexPCN*)vertices;
        break;
    case VertexType::PTN:
        if (vertices)delete[](VertexPTN*)vertices;
        break;
    case VertexType::MODEL:
        if (vertices)delete[](VertexModel*)vertices;
        break;
    case VertexType::TERRAIN:
        if (vertices)delete[](VertexTerrain*)vertices;
        break;
    case VertexType::PT:
        if (vertices)delete[](VertexPT*)vertices;
        break;
    case VertexType::PS:
        if (vertices)delete[](VertexPS*)vertices;
        break;
    case VertexType::PSV:
        if (vertices)delete[](VertexPSV*)vertices;
        break;
    case VertexType::PSP:
        if (vertices)delete[](VertexPSP*)vertices;
        break;
    }

    primitiveTopology = (D3D_PRIMITIVE_TOPOLOGY)in.UInt(b_AddCapacity);
    byteWidth = in.UInt(b_AddCapacity);
    //if (vertexType == VertexType::MODEL) byteWidth += 4; 새 멤버변수를 추가할 때 바이트도 단위별로 늘려줘야한다.
    vertexCount = in.UInt(b_AddCapacity);
    indexCount = in.UInt(b_AddCapacity);

    SafeDeleteArray(indices);
    indices = new UINT[indexCount];


    //읽고난후
    switch (vertexType)
    {
    case VertexType::P:
    {
        vertices = new VertexP[vertexCount];
        VertexP* vertex = (VertexP*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3(b_AddCapacity);
        }
        break;
    }

    case VertexType::PC:
    {
        vertices = new VertexPC[vertexCount];
        VertexPC* vertex = (VertexPC*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3(b_AddCapacity);
            vertex[i].color = in.color3f(b_AddCapacity);
        }
        break;
    }
    case VertexType::PCN:
    {
        vertices = new VertexPCN[vertexCount];
        VertexPCN* vertex = (VertexPCN*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3(b_AddCapacity);
            vertex[i].color = in.color3f(b_AddCapacity);
            vertex[i].normal = in.vector3(b_AddCapacity);
        }
        break;
    }
    case VertexType::PTN:
    {
        vertices = new VertexPTN[vertexCount];
        VertexPTN* vertex = (VertexPTN*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3(b_AddCapacity);
            vertex[i].uv = in.vector2(b_AddCapacity);
            vertex[i].normal = in.vector3(b_AddCapacity);
        }
        break;
    }
    case VertexType::MODEL:
    {
        vertices = new VertexModel[vertexCount];
        VertexModel* vertex = (VertexModel*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3(b_AddCapacity);
            vertex[i].uv = in.vector2(b_AddCapacity);
            vertex[i].normal = in.vector3(b_AddCapacity);
            vertex[i].tangent = in.vector3(b_AddCapacity);
            vertex[i].indices = in.vector4(b_AddCapacity);
            vertex[i].weights = in.vector4(b_AddCapacity);
            vertex[i].tex_weights = 0.0f;
        }
        break;
    }
    case VertexType::TERRAIN:
    {
        vertices = new VertexTerrain[vertexCount];
        VertexTerrain* vertex = (VertexTerrain*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3(b_AddCapacity);
            vertex[i].uv = in.vector2(b_AddCapacity);
            vertex[i].normal = in.vector3(b_AddCapacity);
            vertex[i].weights = in.Float(b_AddCapacity);
        }
        break;
    }
    case VertexType::PT:
    {
        vertices = new VertexPT[vertexCount];
        VertexPT* vertex = (VertexPT*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3(b_AddCapacity);
            vertex[i].uv = in.vector2(b_AddCapacity);
        }
        break;
    }
    case VertexType::PS:
    {
        vertices = new VertexPS[vertexCount];
        VertexPS* vertex = (VertexPS*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3(b_AddCapacity);
            vertex[i].size = in.vector2(b_AddCapacity);
        }
        break;
    }
    case VertexType::PSV:
    {
        vertices = new VertexPSV[vertexCount];
        VertexPSV* vertex = (VertexPSV*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3(b_AddCapacity);
            vertex[i].size = in.vector2(b_AddCapacity);
            vertex[i].velocity = in.vector3(b_AddCapacity);
        }
        break;
    }
    case VertexType::PSP:
    {
        vertices = new VertexPSP[vertexCount];
        VertexPSP* vertex = (VertexPSP*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            vertex[i].position = in.vector3(b_AddCapacity);
            vertex[i].size = in.vector2(b_AddCapacity);
            vertex[i].pivot = in.vector2(b_AddCapacity);
        }
        break;
    }
    }
    for (UINT i = 0; i < indexCount; i++)
    {
        indices[i] = in.UInt(b_AddCapacity);
    }
    in.Close();

    SafeRelease(vertexBuffer);
    SafeRelease(indexBuffer);
    //CreateVertexBuffer
    {
        D3D11_BUFFER_DESC desc;
        desc = { 0 };
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = byteWidth * vertexCount;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data = { 0 };
        data.pSysMem = vertices;

        HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
        assert(SUCCEEDED(hr));
    }

    //Create Index Buffer
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
        desc.ByteWidth = sizeof(UINT) * indexCount;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data = { 0 };
        data.pSysMem = indices;

        HRESULT hr = D3D->GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
        assert(SUCCEEDED(hr));
    }

}

void Mesh::SaveFile(string file)
{
    this->file = file;
    BinaryWriter out;
    wstring path = L"Contents/Mesh/" + Util::ToWString(file);
    out.Open(path);

    out.UInt((UINT)vertexType);
    out.UInt((UINT)primitiveTopology);
    out.UInt(byteWidth);
    out.UInt(vertexCount);
    out.UInt(indexCount);

    switch (vertexType)
    {
    case VertexType::P:
    {
        VertexP* vertex = (VertexP*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
        }
        break;
    }

    case VertexType::PC:
    {
        VertexPC* vertex = (VertexPC*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
            out.color3f(vertex[i].color);
        }
        break;
    }
    case VertexType::PCN:
    {
        VertexPCN* vertex = (VertexPCN*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
            out.color3f(vertex[i].color);
            out.vector3(vertex[i].normal);
        }
        break;
    }
    case VertexType::PTN:
    {
        VertexPTN* vertex = (VertexPTN*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
            out.vector2(vertex[i].uv);
            out.vector3(vertex[i].normal);
        }
        break;
    }
    case VertexType::MODEL:
    {
        VertexModel* vertex = (VertexModel*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
            out.vector2(vertex[i].uv);
            out.vector3(vertex[i].normal);
            out.vector3(vertex[i].tangent);
            out.vector4(vertex[i].indices);
            out.vector4(vertex[i].weights);
            //tex_weights
        }
        break;
    }
    case VertexType::TERRAIN:
    {
        VertexTerrain* vertex = (VertexTerrain*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
            out.vector2(vertex[i].uv);
            out.vector3(vertex[i].normal);
            out.Float(vertex[i].weights);
        }
        break;
    }
    case VertexType::PT:
    {
        VertexPT* vertex = (VertexPT*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
            out.vector2(vertex[i].uv);
        }
        break;
    }
    case VertexType::PS:
    {
        VertexPS* vertex = (VertexPS*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
            out.vector2(vertex[i].size);
        }
        break;
    }
    case VertexType::PSV:
    {
        VertexPSV* vertex = (VertexPSV*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
            out.vector2(vertex[i].size);
            out.vector3(vertex[i].velocity);
        }
        break;
    }
    case VertexType::PSP:
    {
        VertexPSP* vertex = (VertexPSP*)vertices;
        for (UINT i = 0; i < vertexCount; i++)
        {
            out.vector3(vertex[i].position);
            out.vector2(vertex[i].size);
            out.vector2(vertex[i].pivot);
        }
        break;
    }
    }
    for (UINT i = 0; i < indexCount; i++)
    {
        out.UInt(indices[i]);
    }
    out.Close();
}

const Vector3& Mesh::GetVertexPosition(UINT idx, bool bFollowIndices)
{
    UINT index = bFollowIndices ? indices[idx] : idx;

    if (vertexType == VertexType::PC)
    {
        VertexPC* Vertices = (VertexPC*)vertices;
        return Vertices[index].position;
    }
    else if (vertexType == VertexType::PCN)
    {
        VertexPCN* Vertices = (VertexPCN*)vertices;
        return Vertices[index].position;
    }
    else if (vertexType == VertexType::PTN)
    {
        VertexPTN* Vertices = (VertexPTN*)vertices;
        return Vertices[index].position;
    }
    else if (vertexType == VertexType::MODEL)
    {
        VertexModel* Vertices = (VertexModel*)vertices;
        return Vertices[index].position;
    }
    else if (vertexType == VertexType::TERRAIN)
    {
        VertexTerrain* Vertices = (VertexTerrain*)vertices;
        return Vertices[index].position;
    }
    else if (vertexType == VertexType::PT)
    {
        VertexPT* Vertices = (VertexPT*)vertices;
        return Vertices[index].position;
    }
    else if (vertexType == VertexType::PS)
    {
        VertexPS* Vertices = (VertexPS*)vertices;
        return Vertices[index].position;
    }
    else if (vertexType == VertexType::PSV)
    {
        VertexPSV* Vertices = (VertexPSV*)vertices;
        return Vertices[index].position;
    }
    else if (vertexType == VertexType::PSP)
    {
        VertexPSP* Vertices = (VertexPSP*)vertices;
        return Vertices[index].position;
    }

    VertexP* Vertices = (VertexP*)vertices;
    return Vertices[index].position;
}

Vector3& Mesh::SetVertexPosition(UINT idx, bool bFollowIndices)
{
    UINT index = bFollowIndices ? indices[idx] : idx;

    if (vertexType == VertexType::PC)
    {
        VertexPC* Vertices = (VertexPC*)vertices;
        return Vertices[index].position;
    }
    else if (vertexType == VertexType::PCN)
    {
        VertexPCN* Vertices = (VertexPCN*)vertices;
        return Vertices[index].position;
    }
    else if (vertexType == VertexType::PTN)
    {
        VertexPTN* Vertices = (VertexPTN*)vertices;
        return Vertices[index].position;
    }
    else if (vertexType == VertexType::MODEL)
    {
        VertexModel* Vertices = (VertexModel*)vertices;
        return Vertices[index].position;
    }
    else if (vertexType == VertexType::TERRAIN)
    {
        VertexTerrain* Vertices = (VertexTerrain*)vertices;
        return Vertices[index].position;
    }
    else if (vertexType == VertexType::PT)
    {
        VertexPT* Vertices = (VertexPT*)vertices;
        return Vertices[index].position;
    }
    else if (vertexType == VertexType::PS)
    {
        VertexPS* Vertices = (VertexPS*)vertices;
        return Vertices[index].position;
    }
    else if (vertexType == VertexType::PSV)
    {
        VertexPSV* Vertices = (VertexPSV*)vertices;
        return Vertices[index].position;
    }
    else if (vertexType == VertexType::PSP)
    {
        VertexPSP* Vertices = (VertexPSP*)vertices;
        return Vertices[index].position;
    }

    VertexP* Vertices = (VertexP*)vertices;
    return Vertices[index].position;
}

void Mesh::UpdateMesh()
{
    D3D->GetDC()->UpdateSubresource
    (vertexBuffer, 0, NULL, vertices, 0, 0);
}

void Mesh::OptimizeMesh()
{
    /*
    * 이중배열 첫번째 칸에는 중복되지 않는 버텍스의 번호
    * 그 이후 칸부터는 첫칸의 번호와 중복되는 버텍스의 번호들을 넣는다
    */
    vector<deque<UINT>> vTemp;

    for (UINT i = 0; i < vertexCount; i++)
    {
        bool bOverlaped = false;
        switch (vertexType)
        {
        case VertexType::P:
        {
            VertexP* vertex = (VertexP*)vertices;
            for (UINT j = 0; j < vTemp.size(); j++)
            {
                if (vertex[i].position == vertex[vTemp[j][0]].position)
                {
                    vTemp[j].push_back(i);
                    bOverlaped = true;
                    break;
                }
            }
            break;
        }
        case VertexType::PC:
        {
            VertexPC* vertex = (VertexPC*)vertices;
            for (UINT j = 0; j < vTemp.size(); j++)
            {
                if (vertex[i].position == vertex[vTemp[j][0]].position &&
                    vertex[i].color == vertex[vTemp[j][0]].color)
                {
                    vTemp[j].push_back(i);
                    bOverlaped = true;
                    break;
                }
            }
            break;
        }
        case VertexType::PCN:
        {
            VertexPCN* vertex = (VertexPCN*)vertices;
            for (UINT j = 0; j < vTemp.size(); j++)
            {
                if (vertex[i].position == vertex[vTemp[j][0]].position &&
                    vertex[i].color == vertex[vTemp[j][0]].color &&
                    vertex[i].normal == vertex[vTemp[j][0]].normal)
                {
                    vTemp[j].push_back(i);
                    bOverlaped = true;
                    break;
                }
            }
            break;
        }
        case VertexType::PTN:
        {
            VertexPTN* vertex = (VertexPTN*)vertices;
            for (UINT j = 0; j < vTemp.size(); j++)
            {
                if (vertex[i].position == vertex[vTemp[j][0]].position &&
                    vertex[i].uv == vertex[vTemp[j][0]].uv &&
                    vertex[i].normal == vertex[vTemp[j][0]].normal)
                {
                    vTemp[j].push_back(i);
                    bOverlaped = true;
                    break;
                }
            }
            break;
        }
        case VertexType::MODEL:
        {
            VertexModel* vertex = (VertexModel*)vertices;
            for (UINT j = 0; j < vTemp.size(); j++)
            {
                if (vertex[i].position == vertex[vTemp[j][0]].position &&
                    vertex[i].uv == vertex[vTemp[j][0]].uv &&
                    vertex[i].normal == vertex[vTemp[j][0]].normal &&
                    vertex[i].tangent == vertex[vTemp[j][0]].tangent &&
                    vertex[i].indices == vertex[vTemp[j][0]].indices &&
                    vertex[i].weights == vertex[vTemp[j][0]].weights)
                {
                    vTemp[j].push_back(i);
                    bOverlaped = true;
                    break;
                }
            }
            break;
        }
        case VertexType::TERRAIN:
        {
            VertexTerrain* vertex = (VertexTerrain*)vertices;
            for (UINT j = 0; j < vTemp.size(); j++)
            {
                if (vertex[i].position == vertex[vTemp[j][0]].position &&
                    vertex[i].uv == vertex[vTemp[j][0]].uv &&
                    vertex[i].normal == vertex[vTemp[j][0]].normal &&
                    vertex[i].weights == vertex[vTemp[j][0]].weights)
                {
                    vTemp[j].push_back(i);
                    bOverlaped = true;
                    break;
                }
            }
            break;
        }
        case VertexType::PT:
        {
            VertexPT* vertex = (VertexPT*)vertices;
            for (UINT j = 0; j < vTemp.size(); j++)
            {
                if (vertex[i].position == vertex[vTemp[j][0]].position &&
                    vertex[i].uv == vertex[vTemp[j][0]].uv)
                {
                    vTemp[j].push_back(i);
                    bOverlaped = true;
                    break;
                }
            }
            break;
        }
        case VertexType::PS:
        {
            VertexPS* vertex = (VertexPS*)vertices;
            for (UINT j = 0; j < vTemp.size(); j++)
            {
                if (vertex[i].position == vertex[vTemp[j][0]].position &&
                    vertex[i].size == vertex[vTemp[j][0]].size)
                {
                    vTemp[j].push_back(i);
                    bOverlaped = true;
                    break;
                }
            }
            break;
        }
        case VertexType::PSV:
        {
            VertexPSV* vertex = (VertexPSV*)vertices;
            for (UINT j = 0; j < vTemp.size(); j++)
            {
                if (vertex[i].position == vertex[vTemp[j][0]].position &&
                    vertex[i].size == vertex[vTemp[j][0]].size &&
                    vertex[i].velocity == vertex[vTemp[j][0]].velocity)
                {
                    vTemp[j].push_back(i);
                    bOverlaped = true;
                    break;
                }
            }
            break;
        }
        case VertexType::PSP:
        {
            VertexPSP* vertex = (VertexPSP*)vertices;
            for (UINT j = 0; j < vTemp.size(); j++)
            {
                if (vertex[i].position == vertex[vTemp[j][0]].position &&
                    vertex[i].size == vertex[vTemp[j][0]].size &&
                    vertex[i].pivot == vertex[vTemp[j][0]].pivot)
                {
                    vTemp[j].push_back(i);
                    bOverlaped = true;
                    break;
                }
            }
            break;
        }
        }

        if (not bOverlaped)
        {
            deque<UINT> tmp(1, i); // i를 저장한 1칸짜리 deque를 정의
            vTemp.push_back(tmp);
        }
    }

    for (int i = 0; i < vTemp.size(); i++)
    {
        // 중복배열의 맨 뒷칸부터 소거
        while (vTemp[i].size() > 1)
        {
            // indices에서 중복이되는 버텍스를 하나로 병합
            for (int n = 0; n < indexCount; n++)
            {
                if (indices[n] == vTemp[i].back())
                    indices[n] = vTemp[i].front();
            }

            // 병합되어 사라진 버텍스 이후의 모든 배열을 한 칸씩 앞당김
            for (int k = vTemp[i].back() + 1; k < vertexCount; k++)
            {
                switch (vertexType)
                {
                case VertexType::P: { VertexP* vertex = (VertexP*)vertices;       vertex[k - 1] = vertex[k]; break; }
                case VertexType::PC: { VertexPC* vertex = (VertexPC*)vertices;      vertex[k - 1] = vertex[k]; break; }
                case VertexType::PCN: { VertexPCN* vertex = (VertexPCN*)vertices;     vertex[k - 1] = vertex[k]; break; }
                case VertexType::PTN: { VertexPTN* vertex = (VertexPTN*)vertices;     vertex[k - 1] = vertex[k]; break; }
                case VertexType::MODEL: { VertexModel* vertex = (VertexModel*)vertices;   vertex[k - 1] = vertex[k]; break; }
                case VertexType::TERRAIN: { VertexTerrain* vertex = (VertexTerrain*)vertices; vertex[k - 1] = vertex[k]; break; }
                case VertexType::PT: { VertexPT* vertex = (VertexPT*)vertices;      vertex[k - 1] = vertex[k]; break; }
                case VertexType::PS: { VertexPS* vertex = (VertexPS*)vertices;      vertex[k - 1] = vertex[k]; break; }
                case VertexType::PSV: { VertexPSV* vertex = (VertexPSV*)vertices;     vertex[k - 1] = vertex[k]; break; }
                case VertexType::PSP: { VertexPSP* vertex = (VertexPSP*)vertices;     vertex[k - 1] = vertex[k]; break; }
                }
            }
            vertexCount--;

            UINT deleted_index = vTemp[i].back();
            vTemp[i].pop_back();

            /* 삭제된 버텍스 이후의 모든 버텍스의 번호를 한 단계 낮춤 */
            // vTemp배열
            for (int a = 0; a < vTemp.size(); a++)
            {
                for (int b = 0; b < vTemp[a].size(); b++)
                {
                    if (vTemp[a][b] > deleted_index)
                        vTemp[a][b]--;
                }
            }

            // indices
            for (int a = 0; a < indexCount; a++)
            {
                if (indices[a] > deleted_index)
                    indices[a]--;
            }
        }
    }

    UpdateMesh();
    D3D->GetDC()->UpdateSubresource
    (indexBuffer, 0, NULL, indices, 0, 0);
}