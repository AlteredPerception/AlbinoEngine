cbuffer WorldViewProjection : register(b0)
{
   float4x4 world;
   float4x4 worldInvTranspose;
   float4x4 worldViewProjection;
};

struct VS_INPUT
{
   float3 position : POSITION;
   float3 normal   : NORMAL;
   float2 uv	   : TEXCOORD0;
};

struct VS_OUTPUT
{
   float4 position : SV_POSITION;
};

VS_OUTPUT vs_shadow(VS_INPUT input)
{
   VS_OUTPUT output;
   output.position = mul(float4(input.position, 1.0f), worldViewProjection);

   return output;
}