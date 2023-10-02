cbuffer TransformCBuf : register(b0)
{
    matrix viewProj;
};


struct VSOut
{
    float3 worldPos : Position;
    float4 pos : SV_Position;
};

VSOut main(float3 Pos : Position)
{
    VSOut vso;
    vso.worldPos = Pos;
    vso.pos = mul(float4(Pos, 0.0f), viewProj).xyww;
    vso.pos.z = vso.pos.w;
    return vso;
}