cbuffer WorldViewProjection : register(b0)
{
    float4x4 world;
    float4x4 worldInvTranspose;
    float4x4 worldViewProjection;
};

cbuffer FrameLighting : register(b0)
{
   float3 lightDirection;
   float _pad0;

   float3 lightColor;
   float lightIntensity;

   float3 cameraPosition;
   float ambientStrength;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position    : SV_POSITION;
    float3 worldPos    : TEXCOORD1;
    float3 worldNormal : TEXCOORD2;
    float2 uv          : TEXCOORD0;
};

VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output;
    float4 localPos = float4(input.position, 1.0f);

    float4 wp = mul(localPos, world);
    output.worldPos = wp.xyz;

    output.position = mul(localPos, worldViewProjection);

    float3 n = mul(float4(input.normal, 0.0f), worldInvTranspose).xyz;
    output.worldNormal = normalize(n);
    
    output.uv = input.uv;
    return output;
}

Texture2D diffuseTex : register(t0);
SamplerState diffuseSampler : register(s0);


float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
   float4 albedo = diffuseTex.Sample(diffuseSampler, input.uv);
   
   float3 N = normalize(input.worldNormal);
   float3 L = normalize(-lightDirection);

   float NdotL = saturate(dot(N,L));

   float3 ambient = albedo.rgb * ambientStrength;
   float3 diffuse = albedo.rgb * lightColor * NdotL * lightIntensity;

   float3 finalColor = ambient + diffuse;

   return float4(finalColor, albedo.a);
}