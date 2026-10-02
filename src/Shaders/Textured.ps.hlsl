
static const float3 ambient = { 0.15f, 0.15f, 0.15f };

Texture2D g_Diffuse : register(t0);
SamplerState g_Sampler : register(s0);

struct Light
{
    float3 pos;
    float pad0;
    float3 color;
    float pad1;
    float intensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer cbLight : register(b1)
{
    Light g_lights[16];
    int g_numLights;
    float3 _pad;
};

struct PSInput
{
    float4 position : SV_Position;
    float3 worldPos : TEXCOORD1;
    float3 normalW : NORMAL;
    float3 uv : TEXCOORD0;
};

struct PSOutput
{
    float4 color : SV_Target0;
};

PSOutput main(PSInput input)
{
    PSOutput output = (PSOutput) 0;

    float3 tex = g_Diffuse.Sample(g_Sampler, input.uv.xy).rgb;
    float3 diffuse = 0.0f;

    for (int i = 0; i < g_numLights; i++)
    {
        // Point light
        const float3 vToL = g_lights[i].pos - input.worldPos;
        const float distToL = length(vToL);
        const float3 dirToL = vToL / distToL;
        // diffuse attenuation
        const float att = 1.0 / (g_lights[i].attConst + g_lights[i].attLin * distToL + g_lights[i].attQuad * (distToL * distToL));
        // diffuse intensity
        diffuse += g_lights[i].color * g_lights[i].intensity * att * max(0.0f, dot(dirToL, input.normalW));
    }

    float3 final = saturate(diffuse * tex + ambient * tex);
    output.color = float4(final, 1.0);
    return output;
}

    /*  Directional Light
    float3 N = normalize(input.normal);
    // float L = normalize(-g_LightDir);
    float3 L = normalize(-float3(3, -2, 1.5));
    
    float NdotL = saturate(dot(N, L));

    // float3 diffuse = g_LightColor * NdotL * tex;
    // float3 final = diffuse + g_Ambient * tex;
    float3 diffuse =  NdotL * tex;
    float3 final = diffuse + 0.3 * tex;
    */

