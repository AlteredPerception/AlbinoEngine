cbuffer WorldViewProjection : register(b0)
{
   float4x4 world;
   float4x4 worldInvTranspose;
   float4x4 worldViewProjection;
};

cbuffer PointShadowData : register(b2)
{
   float3 lightPosition;
   float lightRange;
};

struct VS_INPUT
{
   float3 position : POSITION;
   float3 normal   : NORMAL;
   float2 uv       : TEXCOORD0;
};

struct VS_OUTPUT
{
   float4 position : SV_POSITION;
   float2 worldPos : TEXCOORD0;
};

VS_OUTPUT vs_main(VS_INPUT input)
{
   VS_OUTPUT output;

   float4 localPos = float4(input.position, 1.0f);
   float4 wp = mul(localPos, world);
   
   output.worldPos = wp.xyz;
   output.position = mul(localPos, worldViewProjection);
   
   return output;
}

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
   float dist = distance(input.worldPos, lightPosition);
   float normalizeDepth = saturate(dist / lightRange);
   //return float4(dist, dist, dist,1.0);
   //return float4(lightRange / 10.0f, lightRange / 10.0f, lightRange / 10.0f, 1.0f);
   return float4(normalizeDepth, normalizeDepth, normalizeDepth, 1.0f);
   //float3 color = abs(input.worldPos) / 10.0f;
   //return float4(color, 1.0f);
}

// Good debug 1: normalize distance
float4 ps_main_d1(VS_OUTPUT input) : SV_TARGET
{
	float dist = distance(input.worldPos, lightPosition);
	float d = saturate(dist / lightRange);
	return float4(d,d,d,1);
}