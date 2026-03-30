#include "LightingCommon.hlsli"
#include "ShadowCommon.hlsli"


cbuffer WorldViewProjection : register(b0)
{
    float4x4 world;
    float4x4 worldInvTranspose;
    float4x4 worldViewProjection;

    //float receiveShadows;
    float3 _pad_object;
};

struct DirectionalLightData
{
   float3 direction;
   float _pad0;

   float3 color;
   float intensity;
   bool turnOn;
   uint castsShadows;
   float3 _pad1;
};

struct PointLightData
{
   float3 position;
   float range;
   
   float3 color; 
   float intensity;
   
   bool turnOn;
   uint castsShadows;
   float3 _pad; 
};

cbuffer FrameLighting : register(b1)
{
   
   DirectionalLightData directionalLights[2];
   PointLightData pointLights[4];

   float3 cameraPosition;
   float ambientStrength;

   float3 specularColor;
   float shininess;

   float4x4 lightViewProjection;

   float shadowBias;
   uint numDirectionalLights;
   uint numPointLights;
   float _pad1;
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
    float4 shadowPos   : TEXCOORD3;
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
    output.shadowPos = mul(wp, lightViewProjection);

    return output;
}

Texture2D diffuseTex : register(t0);
SamplerState diffuseSampler : register(s0);

Texture2D shadowMap : register(t1);

TextureCube pointShadowMap : register(t2);
SamplerState pointShadowSampler : register(s2);

float ComputePointLightAttenuation(float distanceToLight, float range)
{
    float att = saturate(1.0f - (distanceToLight / range));
    return att * att;
}

float4 ps_main_(VS_OUTPUT input) : SV_TARGET
{
   float pointShadow = 1.0f;
   
   float3 toLight = pointLights[0].position - input.worldPos;
   float dist = length(toLight);

   if(dist < pointLights[0].range)
   {
	pointShadow = ComputePointShadow(
		pointShadowMap,
		pointShadowSampler,
		input.worldPos,
		pointLights[0].position,
                pointLights[0].range,
		shadowBias * 4.0f);
   }
   return float4(pointShadow, pointShadow, pointShadow, 1.0f);
}

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
    float4 albedo = diffuseTex.Sample(diffuseSampler, input.uv);

    float3 N = normalize(input.worldNormal);
    float3 V = normalize(cameraPosition - input.worldPos);

    float shadow = 1; 

    float3 ambient = ComputeAmbient(albedo.rgb, ambientStrength);
    float3 directLighting = float3(0.0f, 0.0f, 0.0f);

    
    [unroll]
    for (uint i = 0; i < 2; ++i)
    {
        if (i >= numDirectionalLights)
            break;

        float3 L = normalize(-directionalLights[i].direction);

        float3 diffuse = ComputeDiffuse(
            albedo.rgb,
            N,
            L,
            directionalLights[i].color,
            directionalLights[i].intensity);

        float3 specular = ComputeSpecular(
            N,
            L,
            V,
            specularColor,
            directionalLights[i].color,
            directionalLights[i].intensity,
            shininess);
        
        if(directionalLights[i].castsShadows) {
            shadow = ComputeShadowPCF5x5(shadowMap, input.shadowPos, shadowBias);
        }

        if(directionalLights[i].turnOn)
            directLighting += (diffuse + specular) * shadow;
    }

    //if(receiveShadows < 0.5f)
    //{
    //	shadow = 1.0f;
    //}

    float pointShadow = 1.0f;
    [unroll]
    for(uint j = 0; j < 4; ++j)
    {
       if(j >= numPointLights)
	        break;

       float3 toLight = pointLights[j].position - input.worldPos;
       float dist = length(toLight);
        
       if(dist >= pointLights[j].range)
	        continue;
       
       float3 L = toLight / max(dist, 0.0001f);
       float attenuation = ComputePointLightAttenuation(dist, pointLights[j].range);

       
       
       if(pointLights[j].castsShadows != 0)
       {
	    pointShadow = ComputePointShadow(
		              pointShadowMap,
		              pointShadowSampler,
		              input.worldPos,
		              pointLights[j].position,
		              pointLights[j].range,
		               shadowBias * 4.0f);

	    //if(receiveShadows < 0.5f)
	    //{
		//    pointShadow = 1.0f;
	    //}
       }

       float3 diffuse = ComputeDiffuse(
		        albedo.rgb,
                N,
		        L,
		        pointLights[j].color,
		        pointLights[j].intensity * attenuation);
	
	    float3 specular = ComputeSpecular(
	            N, 
	            L, 
	            V, 
	            specularColor, 
                pointLights[j].color, 
                pointLights[j].intensity * attenuation, 
	            shininess);
       
       if(pointLights[j].turnOn)
	        directLighting += (diffuse + specular) * pointShadow;
    }
    float3 finalColor = ambient + directLighting;
    return float4(finalColor, shadow);
    //return float4(saturate(finalColor), albedo.a);
}

float4 _ps_main(VS_OUTPUT input) : SV_TARGET
{
   float3 toFragment = input.worldPos - pointLights[0].position;
   float currentDist = length(toFragment);
   float3 sampleDir = normalize(toFragment);
   
   float storedDepth = pointShadowMap.Sample(pointShadowSampler, sampleDir).r;
   float currentDepth = saturate((currentDist - shadowBias * 2.0) / pointLights[0].range);

   return float4(currentDepth, storedDepth, abs(currentDepth - storedDepth) * 10.0f, 1.0f);
}

float4 ps_main_e(VS_OUTPUT input) : SV_TARGET
{
   float3 toFragment = input.worldPos - pointLights[0].position;
   float3 sampleDir = normalize(toFragment);
   
   float storedDepth = pointShadowMap.Sample(pointShadowSampler, sampleDir).r;
   //return float4(1.0f, 0.0f, 1.0f, 1.0f);
   return float4(storedDepth, storedDepth, storedDepth, 1.0f);
}

float4 ps_main_uv(VS_OUTPUT input) : SV_TARGET
{
   return float4(input.uv, 0.0f, 1.0f);
}

float4 ps_main_normal(VS_OUTPUT input) : SV_TARGET
{
    float n = normalize(input.worldNormal);
    return float4( n * 0.5, n * 0.5, n * 0.5, 1.0f);
}

//float4 ps_main(VS_OUTPUT input) : SV_TARGET
//{
//    float3 N = normalize(input.worldNormal);
//    float3 L = normalize(-lightDirection);

//    float ndotl = saturate(dot(N, L));
//    return float4(ndotl, ndotl, ndotl, 1.0f);
//}