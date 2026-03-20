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

   //return float4(lightRange / 10.0f, lightRange / 10.0f, lightRange / 10.0f, 1.0f);
   return float4(normalizeDepth, normalizeDepth, normalizeDepth, 1.0f);
}