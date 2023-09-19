cbuffer CBuf
{
    matrix transform;
};

struct VS_INPUT
{
    float3 Pos : Position;
    float2 Tex : TexCoord;
};


struct VSOut
{
    float4 pos : SV_Position;
    float2 tex : TexCoord;
};

VSOut main(VS_INPUT input)
{
    VSOut vso;
    vso.pos = mul(float4(input.Pos, 1.0f), transform);
    vso.tex = input.Tex;
    return vso;
}