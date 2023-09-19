cbuffer CBuf
{
    matrix transform;
};

struct VSOut
{
    float4 color : Color;
    float4 pos : SV_POSITION;
};

VSOut main(float3 pos : POSITION, float4 color : Color)
{
    VSOut vso;
    vso.pos = mul(float4(pos, 1.0f), transform);
    vso.color = color;
    return vso;
}