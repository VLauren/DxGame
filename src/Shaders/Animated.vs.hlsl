struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;

    uint4 boneIndices : BLENDINDICES;
    float4 boneWeights : BLENDWEIGHT;
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

cbuffer Bones : register(b1)
{
    matrix boneMatrices[256];
}

VSOutput main(VSInput input)
{
    float3 blendedPos = float3(0, 0, 0);
    float3 blendedNormal = float3(0, 0, 0);

    for (int i = 0; i < 4; i++)
    {
        if (input.boneWeights[i] <= 0.0)
            break;
        blendedPos += input.boneWeights[i] * mul(float4(input.position, 1.0), boneMatrices[input.boneIndices[i]]).xyz;
        blendedNormal += input.boneWeights[i] * mul(input.normal, (float3x3)boneMatrices[input.boneIndices[i]]);
    }

    // float4 worldPos = mul(float4(input.position, 1.0), transform);
    float4 worldPos = mul(float4(blendedPos, 1.0), transform);
    float4 clipPos = mul(worldPos, viewProj);

    VSOutput output;
    output.position = clipPos;
    output.worldPos = worldPos.xyz;
    output.normalW = normalize(mul(input.normal, (float3x3)transform));
    output.uv = input.uv;

    return output;
}
