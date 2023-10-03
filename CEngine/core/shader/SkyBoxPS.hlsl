TextureCube tex : register(t0);

SamplerState splr : register(s0);

float4 main(float3 worldPos : Position) : SV_Target
{
    return tex.Sample(splr, normalize(worldPos));
}