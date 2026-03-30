cbuffer OffsetBuffer : register(b0)
{
	float2 offset;
};


struct VS_INPUT {
	float3 position2D : POSITION;
	float4 color2D    : COLOR;
};

struct VS_OUTPUT {
	float4 position2D : SV_POSITION;
	float4 color2D : COLOR;
};

VS_OUTPUT vs_main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.position2D = float4(input.position2D.xy + offset.xy, 1.0f, 1.0f);
	output.color2D = input.color2D;

	return output;
}

float4 ps_main(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET {
	return color;
}