struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 text  : TEXCOORD0;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float2 text : TEXCOORD0;
};

VS_OUTPUT vs_mainQuad(VS_INPUT input)
{
   VS_OUTPUT output;

   output.position = float4(input.position, 1.0f);
   output.text = input.text;
   
   return output;
}

Texture2D QuadTexture : register(t0);
SamplerState QuadSampler : register(s0);

float4 ps_mainQuad(VS_OUTPUT output) : SV_TARGET
{
   float4 diffuse = QuadTexture.Sample(QuadSampler, output.text);
   return diffuse;   
//return float4(diffuse.xy, 1, 1.0);
}
