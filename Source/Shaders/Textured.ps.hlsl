
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
    float3 worldPos : TEXCOORD1;
    float3 normal : NORMAL;
    float3 uv : TEXCOORD0;
};

struct PSOutput
{
    float4 color : SV_Target0;
};

static const float3 lightPos = { -2.0f, 2.0f, -2.0f };
static const float3 ambient = { 0.15f, 0.15f, 0.15f };
static const float3 diffuseColor = { 1.0f, 1.0f, 1.0f };
static const float diffuseIntensity = 1.0f;
static const float attConst = 1.0f;
static const float attLin = 0.007f;
static const float attQuad = 0.0002f;

PSOutput main(PSInput input)
{
    PSOutput output = (PSOutput) 0;

    float3 tex = g_Diffuse.Sample(g_Sampler, input.uv.xy).rgb;

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

    // Point light 
    const float3 vToL = lightPos - input.worldPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    // diffuse attenuation
    const float att = 1.0 / (attConst + attLin * distToL + attQuad * (distToL * distToL));
    // diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, input.normal)) * tex;
    float3 final = saturate(diffuse + ambient * tex);
    
    output.color = float4(final, 1.0);
    return output;
}
