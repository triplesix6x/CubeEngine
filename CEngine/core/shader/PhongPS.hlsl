struct LightCBuf
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

#define NR_POINT_LIGHTS 32
cbuffer PLightCbuf
{
    LightCBuf pLights[NR_POINT_LIGHTS];
};

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
    float padding[2];
};

Texture2D tex;
SamplerState splr;

float4 CalcPointLight(LightCBuf light, float3 viewPos, float3 n, float2 texc)
{
    float4 dtex = tex.Sample(splr, texc);
    clip(dtex.a < 0.1f ? -1 : 1);
    if (dot(n, viewPos) >= 0.0f)
    {
        n = -n;
    }
    n = normalize(n);
    const float3 vToL = light.lightPos - viewPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    
    const float att = 1.0f / (light.attConst + light.attLin * distToL + light.attQuad * (distToL * distToL));

    const float3 diffuse = light.diffuseColor * light.diffuseIntensity * att * max(0.0f, dot(dirToL, n));
    
    const float3 w = n * dot(vToL, n);
    const float3 r = w * 2.0f - vToL;
    const float3 specular = att * (light.diffuseColor * light.diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);
    
    return float4(saturate((diffuse + light.ambient) * dtex.rgb + specular), dtex.a);
}

float4 main(float3 viewPos : Position, float3 n : Normal, float2 texc : TexCoord) : SV_Target
{
    float4 result = 0;
    for (int i = 0; i < NR_POINT_LIGHTS; ++i)
    {
        if (pLights[i].diffuseIntensity != 0.0f || (pLights[i].ambient.r != 0.0f || pLights[i].ambient.g != 0.0f || pLights[i].ambient.b != 0.0f))
        {
            result += CalcPointLight(pLights[i], viewPos, n, texc);
        }
    }
    
    return result;
}