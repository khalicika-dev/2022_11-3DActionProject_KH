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

struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 wPosition : POSITION0;
    float2 Uv : UV0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
    float4 vPosition : POSITION1;
    float TexWeights : TEXWEIGHTS0;
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
    output.vPosition = mul(output.Position, ShadowVP);
    output.Position = mul(output.Position, ViewProj);
    output.Uv = input.Uv;
    output.Normal = mul(input.Normal, (float3x3) World);
    output.Tangent = mul(input.Tangent, (float3x3) World);
    output.Binormal = cross(output.Normal, output.Tangent);
    output.TexWeights = input.TexWeights;
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 BaseColor = DiffuseMapping(input.Uv, 0);
    
    if(input.TexWeights > 0.0f)
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
    
    BaseColor = Lighting(BaseColor, input.Uv, Normal, input.wPosition, input.TexWeights);
    BaseColor = AddShadow(BaseColor, input.vPosition);
    
    
    return BaseColor;
}