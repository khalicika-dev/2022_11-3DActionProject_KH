#include "Common.hlsl"


struct VertexInput
{
    float4 Position : POSITION0;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
	//정점변환때만 쓰이는 멤버
    float4 Indices : INDICES0;
    float4 Weights : WEIGHTS0;
    float TexWeights : TEXWEIGHTS0;
};

struct GeometryInput
{
    float4 Position : SV_POSITION;
    float3 wPosition : POSITION0;
    float2 Uv : UV0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
    float TexWeights : TEXWEIGHTS;
};

struct PixelInput
{
    uint   TargetIndex : SV_RenderTargetArrayIndex;
    float4 Position : SV_POSITION;
    float3 wPosition : POSITION0;
    float2 Uv : UV0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
    float TexWeights : TEXWEIGHTS0;
};

GeometryInput VS(VertexInput input)
{
    GeometryInput output;
    
    //World = offset * World;
    
    
     [branch]
    if (input.Weights.x)
        output.Position = mul(input.Position, SkinWorld(input.Indices, input.Weights));
    else
        output.Position = mul(input.Position, World);
    
    output.wPosition = output.Position;
    //output.Position = mul(output.Position, ViewProj);
    output.Uv = input.Uv;
    output.Normal = mul(input.Normal, (float3x3) World);
    output.Tangent = mul(input.Tangent, (float3x3) World);
    output.Binormal = cross(output.Normal, output.Tangent);
    output.TexWeights = input.TexWeights;
    return output;
}



[maxvertexcount(18)]
void GS(triangle GeometryInput input[3], inout TriangleStream<PixelInput> stream)
{
    int vertex = 0;
    PixelInput output;
    
    [unroll(6)]
    for (int i = 0; i < 6; i++)
    {
        output.TargetIndex = i;
        
        [unroll(3)]
        for (vertex = 0; vertex < 3; vertex++)
        {
            output.Position = input[vertex].Position;
            output.Position = mul(output.Position, CubeViews[i]);
            output.Position = mul(output.Position, CubeProjection);
            output.Uv = input[vertex].Uv;
            output.wPosition = input[vertex].Position;
            output.Binormal = input[vertex].Binormal;
            output.Normal = input[vertex].Normal;
            output.Tangent = input[vertex].Tangent;
            output.TexWeights = input[vertex].TexWeights;
            stream.Append(output);
        }
        stream.RestartStrip();
    }
}


float4 PS(PixelInput input) : SV_TARGET
{
    const float pi = 3.141592f;
    
    float4 BaseColor = DiffuseMapping(input.Uv, 0);
    if (input.TexWeights > 0.0f)
    {
        float4 BaseColor2 = DiffuseMapping(input.Uv, 1);
        BaseColor = BaseColor * (1 - input.TexWeights)
        + BaseColor2 * input.TexWeights;
    }
    
    float3 Normal = NormalMapping(input.Tangent, input.Binormal, input.Normal, input.Uv, 0);
    if (input.TexWeights > 0.0f)
    {
        float3 Normal2 = NormalMapping(input.Tangent, input.Binormal, input.Normal, input.Uv, 1);
        Normal = Normal * (1 - input.TexWeights)
        + Normal2 * input.TexWeights;
    }
    
    float3 v1 = normalize(ViewPos.xyz - CubeCamPos);    // 거울카메라에서 시야로 보는 방향
    float3 v2 = normalize(input.wPosition - CubeCamPos);    // 거울카메라에서 물체를 보는 방향
    float angle = acos(dot(v1, v2));
    angle = min(angle, pi - angle);
    
    if (length(CubeCamPos - input.wPosition) * cos(angle) < CubeMapDist)
        discard;
    BaseColor = Lighting(BaseColor, input.Uv, Normal, input.wPosition, input.TexWeights);
    
    return BaseColor;
}