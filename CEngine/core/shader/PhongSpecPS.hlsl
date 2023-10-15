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
    float specularMapWeight;
    bool hasGloss;
    float specularPowerConst;
};

Texture2D tex;
Texture2D spec;
SamplerState splr;

float3 CalcPointLight(LightCBuf light, float3 viewPos, float3 n, float2 texc)
{
    n = normalize(n);
    const float3 vToL = light.lightPos - viewPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    
    const float att = 1.0f / (light.attConst + light.attLin * distToL + light.attQuad * (distToL * distToL));

    const float3 diffuse = light.diffuseColor * light.diffuseIntensity * att * max(0.0f, dot(dirToL, n));
    
    const float3 w = n * dot(vToL, n);
    const float3 r = w * 2.0f - vToL;
    const float4 specularSample = spec.Sample(splr, texc);
    const float3 specularReflectionColor = specularSample.rgb * specularMapWeight;
    float3 specularPower = 0;
    if (hasGloss)
    {
        specularPower = pow(2.0f, specularSample.a * 13.0f);
    }
    else
    {
        specularPower = specularPowerConst;
    }

    const float3 specular = att * (light.diffuseColor * light.diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);
    
    return float3(saturate((diffuse + light.ambient) * tex.Sample(splr, texc).rgb + specular * specularReflectionColor));
}

float4 main(float3 viewPos : Position, float3 n : Normal, float2 texc : TexCoord) : SV_Target
{
    float3 result = 0;
    for (int i = 0; i < NR_POINT_LIGHTS; ++i)
    {
        if (pLights[i].diffuseIntensity != 0.0f || (pLights[i].ambient.r != 0.0f || pLights[i].ambient.g != 0.0f || pLights[i].ambient.b != 0.0f))
        {
            result += CalcPointLight(pLights[i], viewPos, n, texc);
        }
    }
    
    return float4(result, 1.0f);
}