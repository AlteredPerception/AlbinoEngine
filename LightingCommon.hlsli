#ifndef LIGHTING_COMMON_HLSLI
#define LIGHTING_COMMIN_HLSLI

float3 ComputeAmbient(float3 albedo, float ambientStrength)
{
    return albedo * ambientStrength;
}

float3 ComputeDiffuse(float3 albedo, float3 normal, float3 lightDir, float3 lightColor, float lightIntensity)
{
    float NdotL = saturate(dot(normal, lightDir));
    return albedo * lightColor * NdotL * lightIntensity;
}

float3 ComputeSpecular(
            float3 normal,
            float3 lightDir,
            float3 viewDir,
            float3 specularColor,
            float3 lightColor,
            float lightIntensity,
            float shininess)
{
    float3 H = normalize(lightDir + viewDir);
    float NdotH = saturate(dot(normal, H));
    float spec = pow(NdotH, shininess);
    return specularColor * spec * lightColor * lightIntensity;
}

#endif