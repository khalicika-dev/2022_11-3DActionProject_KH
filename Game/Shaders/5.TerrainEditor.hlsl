#include "Common.hlsl"

struct VertexInput
{
    float4 Position : POSITION0;
    float2 Uv : UV0;
    float4 Normal : NORMAL0;
    float Weights : WEIGHTS0;
};
struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float4 Normal : NORMAL0;
    float3 wPosition : POSITION0;
    float Weights : WEIGHTS0;
};

cbuffer PS_Brush : register(b10)
{
   float3   BrushPoint;
   float    BrushRange;
   float    BrushShape;
   float    BrushType;
   float2   BrushPadding;
}


PixelInput VS(VertexInput input)
{
   
    PixelInput output;
    output.Uv = input.Uv;
    //  o           =  i X W
    output.Position = mul(input.Position, World);
    output.wPosition = output.Position.xyz;
    output.Position = mul(output.Position, ViewProj);
    input.Normal.w = 0.0f;
    output.Normal = mul(input.Normal, World);
    output.Weights = input.Weights;
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 BaseColor = TextureD[0].Sample(SamplerD, input.Uv);
    
    float4 BaseColor2 = TextureD[1].Sample(SamplerD, input.Uv);
    BaseColor = BaseColor * (1 - input.Weights)
    + BaseColor2 * input.Weights;
    
    float3 Normal = normalize(input.Normal);
    
    BaseColor = Lighting(BaseColor, input.Uv, Normal, input.wPosition, 0.0f);
    
    float3 v1 = float3(BrushPoint.x, 0.0f, BrushPoint.z);
    float3 v2 = float3(input.wPosition.x, 0.0f, input.wPosition.z);
    //float3 temp = v2 - v1;
    float Dis = distance(v2, v1);
    float w;
    //³×¸ð
    if (BrushShape == 0.0f)
    {
        if (abs(v1.x - v2.x) < BrushRange
            && abs(v1.z - v2.z) < BrushRange)
        {
            if (BrushType == 0)
            {
                BaseColor.rgb += float3(1, 0, 0);
            }
            if (BrushType == 1)
            {
                BaseColor.rgb += float3(0, 1, 0);
            }
            if (BrushType == 2)
            {
                BaseColor.rgb += float3(0, 0, 1);
            }
        }
    }
    //¿ø
    else
    {
        w = saturate(Dis / BrushRange);
        w = 1.0f - w;
        if (BrushType == 0)
        {
            BaseColor.rgb += float3(w, 0, 0);
        }
        if (BrushType == 1)
        {
            BaseColor.rgb += float3(0, w, 0);
        }
        if (BrushType == 2)
        {
            BaseColor.rgb += float3(0, 0, w);
        }
    }
    
    return BaseColor;
}