#include "Common.hlsl"

struct VertexInput
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
};

struct GeometryInput
{
    
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
};


struct PixelInput
{
    uint TargetIndex : SV_RenderTargetArrayIndex;
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
};

GeometryInput VS(VertexInput input)
{
   
    GeometryInput output;
    output.Color = input.Color;
    //  o           =  i X W
    output.Position = mul(input.Position, World);
    //output.Position = mul(output.Position, ViewProj);
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
            output.Position.xyz = mul(output.Position.xyz, (float3x3) (CubeViews[i]));
            output.Position = mul(output.Position, CubeProjection);
            output.Color = input[vertex].Color;
            stream.Append(output);
        }
        
        stream.RestartStrip();
    }
}



float4 PS(PixelInput input) : SV_TARGET
{
    //if (input.Position.x <400)
    //{
    //    return float4(1, 0, 0, 1);
    //}

    return input.Color;
}