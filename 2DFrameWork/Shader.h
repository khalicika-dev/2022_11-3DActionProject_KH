#pragma once
class Shader
{
    ID3D11InputLayout*      vertexLayout;
    ID3D11VertexShader*     vertexShader;
    ID3D11PixelShader*      pixelShader;
    ID3D11GeometryShader*   geometryShader;
public:
    VertexType              vertextype;
    string                  file;
    Shader();
    ~Shader();
    static uint64_t CalculateFile(string file);
    void LoadFile(string file, bool b_AddCapacity);
    void LoadGeometry();
    void Set();
};
