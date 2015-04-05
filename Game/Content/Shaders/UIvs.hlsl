cbuffer PositionBuffer
{
};

struct VertexInputType
{
    float4 position : POSITION;
	int index		: TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    
	float4 pos = input.position;
	pos.xy /= input.index;
    output.position = pos;
    
    return output;
}