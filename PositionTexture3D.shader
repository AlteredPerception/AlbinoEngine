cbuffer WorldViewProjection : register(b0)
{
    float4x4 world;
    float4x4 worldInvTranspose;
    float4x4 worldViewProjection;
};

cbuffer FrameLighting : register(b1)
{
   float3 lightDirection;
   float _pad0;

   float3 lightColor;
   float lightIntensity;

   float3 cameraPosition;
   float ambientStrength;

   float3 specularColor;
   float shininess;

   float4x4 lightViewProjection;

   float shadowBias;
   float3 _pad1;
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
SamplerComparisonState shadowSampler : register(s1);

//float computeShadow(float4 shadowPos)
//{
//    float3 proj = shadowPos.xyz / shadowPos.w;

//    float2 uv = proj.xy * 0.5f + 0.5f;
//    uv.y = 1.0f - uv.y;

//    float depth = proj.z;

//    if (uv.x < 0 || uv.x > 1 || uv.y < 0 || uv.y > 1)
//        return 1.0f;

//    return shadowMap.SampleCmpLevelZero(
//        shadowSampler,
//        uv,
//        depth - shadowBias
//    );
//}

float computeShadow(float4 shadowPos)
{
    if (abs(shadowPos.w) < 0.0001f)
        return 1.0f;

    float3 proj = shadowPos.xyz / shadowPos.w;

    // NDC [-1, 1] -> UV [0, 1]
    float2 uv = proj.xy * 0.5f + 0.5f;
    uv.y = 1.0f - uv.y;

    float depth = proj.z;

    // Outside shadow map = lit
    if (uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f || depth < 0.0f || depth > 1.0f)
        return 1.0f;

    uint w, h;
    shadowMap.GetDimensions(w, h);

    int2 texel = int2(uv * float2(w, h));
    texel = clamp(texel, int2(0, 0), int2((int)w - 1, (int)h - 1));

    float shadowDepth = shadowMap.Load(int3(texel, 0)).r;
    float currentDepth = depth - shadowBias;

    return (currentDepth <= shadowDepth) ? 1.0f : 0.0f;
}

float4 ps_main(VS_OUTPUT input) : SV_TARGET
{
   float4 albedo = diffuseTex.Sample(diffuseSampler, input.uv);
   
   float3 N = normalize(input.worldNormal);
   float3 L = normalize(-lightDirection);
   float3 V = normalize(cameraPosition - input.worldPos);
   float3 H = normalize(L + V);

   float NdotL = saturate(dot(N,L));
   float NdotH = saturate(dot(N,H));

   float shadow = computeShadow(input.shadowPos);

   float3 ambient = albedo.rgb * ambientStrength;
   float3 diffuse = albedo.rgb * lightColor * NdotL * lightIntensity;
   float spec = pow(NdotH, shininess);

   float3 specular = specularColor * spec * lightColor * lightIntensity;

   float3 finalColor = ambient + (diffuse + specular) * shadow;

   return float4(saturate(finalColor), albedo.a);
   //return float4(shadow, shadow, shadow, 1.0f);
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