cbuffer WorldViewProjection : register(b0)
{
	float4x4 worldViewProj; 
	
};


struct VS_INPUT {
	float3 position3D : POSITION;
	float4 color2D    : COLOR;
};

struct VS_OUTPUT {
	float4 position3D : SV_POSITION;
	float4 color2D : COLOR;
};

VS_OUTPUT vs_main(VS_INPUT input)
{
	VS_OUTPUT output;

	//output.position2D = float4(input.position2D.xy + offset.xy, 1.0f, 1.0f);
        output.position3D = mul(float4(input.position3D, 1.0f), worldViewProj);
	output.color2D = input.color2D;

	return output;
}

cbuffer pixelColor : register(b0)
{
	float4 pixColor;
}

float4 ps_main(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET {
	
	return color;
}