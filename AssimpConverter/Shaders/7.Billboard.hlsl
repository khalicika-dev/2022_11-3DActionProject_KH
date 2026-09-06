#include "Common.hlsl"

struct VertexInput
{
    float4 Position : POSITION0;
    float2 Size : SIZE0;
    float2 Pivot : PIVOT0;
};
struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};

VertexInput VS(VertexInput input)
{
   
    VertexInput output;
    output.Size = input.Size;
    output.Pivot = input.Pivot;
    //  o           =  i X W
    output.Position = mul(input.Position, World);
    return output;
}

static const float2 TEXCOORD[4] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(1.0f, 0.0f)
};
[maxvertexcount(4)]
void GS(point VertexInput input[1], inout TriangleStream<PixelInput> output)
{
    //한개의 점을 네개로 나누기
    
    // 월드변환후 뷰 프로젝션변환
    
    float3 up = GSViewUp;
    float3 forward = GSViewForward;
    float3 right = normalize(cross(up, forward));
    
    float2 halfSize = input[0].Size * 0.5f;
    
    float4 vertices[4];
    //input[0].Position.xyz (기준좌표,중점)
    
    //왼쪽 아래
   // vertices[0] = float4(input[0].Position.xyz - halfSize.x * right - halfSize.y * up, 1.0f);
    vertices[0] = float4(input[0].Position.xyz + right * (0.0f - input[0].Pivot.x) - up * halfSize.y, 1.0f);
    // 왼 위
    vertices[1] = float4(input[0].Position.xyz + right * (0.0f - input[0].Pivot.x) + up * halfSize.y, 1.0f);
    // 오 아래
    vertices[2] = float4(vertices[0].xyz + right * (input[0].Size.x * 2.0f), 1.0f);
    // 오 위
    vertices[3] = float4(vertices[1].xyz + right * (input[0].Size.x * 2.0f), 1.0f);
    
    PixelInput pixelInput;
    
    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        //월드에서 다시 ndc까지 변환
        pixelInput.Position = mul(vertices[i], GSViewProj);
        pixelInput.Uv = TEXCOORD[i];
        
        output.Append(pixelInput);
    }
}

float4 PS(PixelInput input) : SV_TARGET
{
    
    float4 BaseColor = DiffuseMapping(input.Uv, 0);
       
    if (BaseColor.a == 0)
        discard;
    return BaseColor;
}