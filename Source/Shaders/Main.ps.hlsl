
struct PSInput
{
    float4 position : SV_Position;
    float3 color : COLOR0;
};

struct PSOutput
{
    float4 color : SV_Target0;
};

PSOutput main(PSInput input)
{
    PSOutput output = (PSOutput) 0;
    output.color = float4(input.color, 1.0);
    return output;
}

/*
// ===========================
// Dithering and noise effect

static const uint Bayer[4][4] =
{
    {  0,  8,  2, 10 },
    { 12,  4, 14,  6 },
    {  3, 11,  1,  9 },
    { 15,  7, 13,  5 }
};

uint wang_hash(uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

cbuffer FrameCB : register(b0) { uint g_frameSeed; };   // changes each frame
float4 main(PSInput i) : SV_Target0
{
    uint2 px = (uint2) i.position.xy;

    // scramble frame + pixel -> big pseudo-random number
    uint seedX = g_frameSeed * 0x9e3779b9;
    uint seedY = g_frameSeed * 0x7f4a7c15;
    uint h     = wang_hash((px.x ^ seedX) + (px.y ^ seedY) * 1987);

    // 16-bit noise, centred, +-2 LSB
    float n  = (h & 0xFFFF) / 65535.0;      // 0 … 1
    float3 noise = (n - 0.5) * (1 / 7.0); // +-2 LSB

    // quantise
    float3 c = saturate(i.color + noise);
    c = floor(c * 7.0 + 0.5) / 7.0;
    return float4(c, 1.0); 
}
*/
