
static const float3 ambient = { 0.15f, 0.15f, 0.15f };

Texture2D g_Diffuse : register(t0);
SamplerState g_Sampler : register(s0);

cbuffer cbLight : register(b1)
{
    float3 g_lightPos;
    float pad0;
    float3 g_diffuseColor;
    float pad1;
    float g_diffuseIntensity;
    float g_attConst;
    float g_attLin;
    float g_attQuad;
};

// static const float3 lightPos = { -2.0f, 2.0f, -2.0f };
// static const float3 diffuseColor = { 1.0f, 1.0f, 1.0f };
// static const float diffuseIntensity = 1.0f;
// static const float attConst = 1.0f;
// static const float attLin = 0.09f;
// static const float attQuad = 0.032f;

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

    // Point light 
    const float3 vToL = g_lightPos - input.worldPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    // diffuse attenuation
    const float att = 1.0 / (g_attConst + g_attLin * distToL + g_attQuad * (distToL * distToL));
    // diffuse intensity
    const float3 diffuse = g_diffuseColor * g_diffuseIntensity * att * max(0.0f, dot(dirToL, input.normalW)) * tex;
    float3 final = saturate(diffuse + ambient * tex);
    
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

