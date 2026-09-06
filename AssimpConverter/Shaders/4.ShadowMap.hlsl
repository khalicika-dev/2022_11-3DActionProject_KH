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
    //float TexWeights : TEXWEIGHTS0;
};

struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 wPosition : POSITION0;
    float2 Uv : UV0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
    //float TexWeights : TEXWEIGHTS0;
};

PixelInput VS(VertexInput input)
{
    PixelInput output;
    
    //World = offset * World;
    
    
     [branch]
    if (input.Weights.x)
        output.Position = mul(input.Position, SkinWorld(input.Indices, input.Weights));
    else
        output.Position = mul(input.Position, World);
    
    output.wPosition = output.Position;
    output.Position = mul(output.Position, ViewProj);
    output.Uv = input.Uv;
    output.Normal = mul(input.Normal, (float3x3) World);
    output.Tangent = mul(input.Tangent, (float3x3) World);
    output.Binormal = cross(output.Normal, output.Tangent);
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float depth = input.Position.z / input.Position.w ;
    //float depth = input.Position.z / input.Position.w * 100.0f;
    //depth = 1.0f - depth;
    return float4(depth, depth, depth, 1.0f);
}