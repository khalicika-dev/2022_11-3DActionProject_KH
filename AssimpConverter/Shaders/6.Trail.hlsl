#include "Common.hlsl"

struct VertexInput
{
    float4 Position : POSITION0;
    float2 Uv : UV0;
};
struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 wPosition : POSITION0;
};

PixelInput VS(VertexInput input)
{
   
    PixelInput output;
    output.Uv = input.Uv;
    //  o           =  i X W
    output.Position = mul(input.Position, World);
    output.wPosition = output.Position.xyz;
    //output.oPosition = input.Position.xyz;
    output.Position = mul(output.Position, ViewProj);
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 BaseColor = float4(1, 1, 1, 1);
    float transparent;
    [flatten]
    if (Ka.a)
        transparent = TextureN[0].Sample(SamplerN, input.Uv).r;
    else
        transparent = 0.0f;
    
    if (transparent <= 0.0f || Opacity <= 0.0f)
        discard;
    
    BaseColor.rgb = Kd.rgb;
    BaseColor.a = transparent * Opacity;
    
    return BaseColor;
}