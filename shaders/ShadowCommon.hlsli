#ifndef SHADOW_COMMON_HLSLI
#define SHADOW_COMMON_HLSLI

float hash12(float2 p)
{
    float h = dot(p, float2(127.1, 311.7));
    return frac(sin(h) * 43758.5453123);
}

float2 rotate2(float2 v, float angle)
{
    float s = sin(angle);
    float c = cos(angle);
    return float2(
        c * v.x - s * v.y,
        s * v.x + c * v.y
    );

}

float ComputeShadowPCF5x5(
        Texture2D shadowMap, 
        float4 shadowPos, 
        float shadowBias)
{
    if(abs(shadowPos.w) < 0.001f)
        return 1.0f;
    
    float3 proj = shadowPos.xyz / shadowPos.w;
    
    float2 uv = proj.xy * 0.5f + 0.5f;
    uv.y = 1.0f - uv.y;
    
    float depth = proj.z;
    
    if (uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f || depth < 0.0f || depth > 1.0f)
        return 1.0f;
    
    uint w, h;
    shadowMap.GetDimensions(w, h);
    
    float2 texelSize = 1.0f / float2(w, h);
    float currentDepth = depth - shadowBias;
    
    float angle = hash12(floor(uv * float2(w, h))) * 6.2831853f;
    
    float shadow = 0.0f;
    
    [unroll]
    for (int y = -2; y <= 2; ++y)
    {
        [unroll]
        for (int x = -2; x <= 2; ++x)
        {
            float2 baseOffset = float2((float) x, (float) y);
            float2 rotatedOffset = rotate2(baseOffset, angle) * texelSize;
            
            float2 sampleUV = uv + rotatedOffset;
            
            if(sampleUV.x < 0.0f || sampleUV.x > 1.0f || sampleUV.y < 0.0f || sampleUV.y > 1.0f)
            {
                shadow += 1.0f;
                continue;
            }
            
            int2 texel = int2(sampleUV * float2(w, h));
            texel = clamp(texel, int2(0, 0), int2((float) w - 1, (float) h - 1));
            
            float shadowDepth = shadowMap.Load(int3(texel, 0)).r;
            shadow += (currentDepth <= shadowDepth) ? 1.0f : 0.0f;

        }

    }
    return shadow / 25.0f;
}

float ComputePointShadow(
	TextureCube shadowCube,
	SamplerState shadowSamp,
	float3 worldPos,
	float3 lightPos,
	float lightRange,
	float shadowBias)
{
	float3 toFragment = worldPos - lightPos;
        float currentDist = length(toFragment);

	if(currentDist >= lightRange)
	   return 1.0f;
	
	float3 sampleDir = normalize(toFragment);
	
	float storedDepth = shadowCube.Sample(shadowSamp, sampleDir).r;
	float currentDepth = saturate((currentDist - shadowBias) / lightRange);

	return (currentDepth <= storedDepth) ? 1.0f : 0.0f;
}

#endif