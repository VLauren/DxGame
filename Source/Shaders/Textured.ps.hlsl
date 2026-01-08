
Texture2D g_Diffuse : register(t0);
SamplerState g_Sampler : register(s0);

cbuffer cbLight : register(b1)
{
    float3 g_LightDir;
    float3 g_LightColor;
    float3 g_Ambient;
};

struct PSInput
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float3 uv : TEXCOORD0;
};

struct PSOutput
{
    float4 color : SV_Target0;
};

PSOutput main(PSInput input)
{
    PSOutput output = (PSOutput) 0;

    float3 N = normalize(input.normal);
    // float L = normalize(-g_LightDir);
    float3 L = normalize(-float3(3, -2, 1.5));
    
    float NdotL = saturate(dot(N, L));

    float3 tex = g_Diffuse.Sample(g_Sampler, input.uv).rgb;
    // float3 diffuse = g_LightColor * NdotL * tex;
    // float3 final = diffuse + g_Ambient * tex;
    float3 diffuse =  NdotL * tex;
    float3 final = diffuse + 0.3 * tex;
    
    output.color = float4(final, 1.0);
    return output;
}
