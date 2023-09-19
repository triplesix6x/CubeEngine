struct PS_INPUT
{
    float4 Pos : SV_Position; 
    float2 Tex : TexCoord; 
};


Texture2D tex : register(t0);

SamplerState splr : register(s0);

float4 main(PS_INPUT input) : SV_Target
{
    return tex.Sample(splr, input.Tex);
}