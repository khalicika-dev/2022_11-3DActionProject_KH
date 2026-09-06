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
};

PixelInput VS(VertexInput input)
{
   
    PixelInput output;
    //  o           =  i X W
    output.Uv = input.Uv;
    output.Position = mul(input.Position, World);
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    
    float4 BaseColor = DiffuseMapping(input.Uv, 0);
       
    if (BaseColor.a == 0 || Opacity <= 0.0f)
        discard;
    
    BaseColor.rgb *= Kd.rgb;
    BaseColor.a *= Opacity;
    
    return BaseColor;
}