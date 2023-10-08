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

Texture2D tex;
Texture2D spec;
SamplerState splr;

float4 main(float3 worldPos : Position, float3 n : Normal, float2 texc : TexCoord) : SV_Target
{

    const float3 vToL = lightPos - worldPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));

    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
    
    const float3 w = n * dot(vToL, n);
    const float3 r = w * 2.0f - vToL;
    const float4 specularSample = spec.Sample(splr, texc);
    const float3 specularReflectionColor = specularSample.rgb;
    const float specularPower = pow(2.0f, specularSample.a * 13.0f);
    const float3 specular = att * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);

    return float4(saturate((diffuse + ambient) * tex.Sample(splr, texc).rgb + specular * specularReflectionColor), 1.0f);
}