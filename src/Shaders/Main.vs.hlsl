
struct VSInput
{
    float3 position : POSITION;
    float3 color : COLOR0;
    float3 normal : NORMAL;
};

struct VSOutput
{
    float4 position : SV_Position;
    float3 color : COLOR0;
    float3 normal : NORMAL;
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
    output.color = input.color;
    output.position = clipPos;
    output.normal = mul(input.normal, (float3x3)transform);

    return output;
}
