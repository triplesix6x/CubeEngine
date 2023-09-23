cbuffer LightCBuf
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer ObjectCBuf
{
    float3 materialColors[6];
    float padding;
    float specularIntensity;
    float specularPower;
};


float4 main(float3 worldPos : Position, float3 n : Normal, uint tid : SV_PrimitiveID) : SV_Target
{
    const float3 vToL = lightPos - worldPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;

    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));

    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));

    const float3 w = n * dot(vToL, n);
    const float3 r = w * 2.0f - vToL;

    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);

    return float4(saturate((diffuse + ambient + specular) * materialColors[(tid / 2) % 6]), 1.0f);
}