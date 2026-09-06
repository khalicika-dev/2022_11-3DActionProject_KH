#include "Common.hlsl"

struct VertexInput
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
    float4 Normal : NORMAL0;
};
struct PixelInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
    float4 Normal : NORMAL0;
    float3 wPosition : POSITION0;
    //float3 oPosition : POSITION1;
};

PixelInput VS(VertexInput input)
{
   
    PixelInput output;
    output.Color = input.Color;
    //  o           =  i X W
    output.Position = mul(input.Position, World);
    output.wPosition = output.Position.xyz;
    //output.oPosition = input.Position.xyz;
    output.Position = mul(output.Position, ViewProj);
    input.Normal.w = 0.0f;
    output.Normal = mul(input.Normal, World);
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 BaseColor = input.Color;
    
    float3 Normal = normalize(input.Normal);
    
    BaseColor = Lighting(BaseColor, float2(1, 1), Normal, input.wPosition, 0.0f);
    
    
    return BaseColor;
}