
struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_Position;
    float3 worldPos : TEXCOORD1;
    float3 normalW : NORMAL;
    float2 uv : TEXCOORD0;
};

cbuffer Constants : register(b0)
{
    matrix transform;
    matrix viewProj;
};

VSOutput main(VSInput input)
{
    float4 worldPos = mul(float4(input.position, 1.0), transform);
    float4 clipPos = mul(worldPos, viewProj);

    VSOutput output;
    output.position = clipPos;
    output.worldPos = worldPos.xyz;
    output.normalW = mul(input.normal, (float3x3) transform);
    output.uv = input.uv;

    return output;
}
