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
    float4 vPosition : POSITION1;
};

PixelInput VS(VertexInput input)
{
   
    PixelInput output;
    output.Uv = input.Uv;
    //  o           =  i X W
    output.Position = mul(input.Position, World);
    output.vPosition = mul(output.Position, ShadowVP);
    output.wPosition = output.Position.xyz;
    output.Position = mul(output.Position, ViewProj);
    input.Normal.w = 0.0f;
    output.Normal = mul(input.Normal, World);
    output.Weights = input.Weights;
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    //float4 BaseColor = TextureD[0].Sample(SamplerD, input.Uv);
    //
    //float4 BaseColor2 = TextureN[0].Sample(SamplerN, input.Uv);
    //BaseColor = BaseColor * input.Weights 
    //+ BaseColor2 * (1 - input.Weights);
    
    float4 BaseColor = TextureD[0].Sample(SamplerD, input.Uv);
    
    float4 BaseColor2 = TextureD[1].Sample(SamplerD, input.Uv);
    BaseColor = BaseColor * (1 - input.Weights)
    + BaseColor2 * input.Weights;
    
    
    float3 Normal = normalize(input.Normal);
    //float3 N1 = TextureN[0].Sample(SamplerN, input.Uv).rgb;
    //float3 N2 = TextureN[1].Sample(SamplerN, input.Uv).rgb;
    //float3 N = N1 * (1 - input.Weights)
    //+ N2 * input.Weights;
    //
    //N = normalize(N * 2.0f - 1.0f);
    //Normal = normalize(mul(Normal, N));
    
    BaseColor = Lighting(BaseColor, input.Uv, Normal, input.wPosition, 0.0f);
    
    BaseColor = AddShadow(BaseColor, input.vPosition);
    
    return BaseColor;
}