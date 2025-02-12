Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

struct Input {
    float4 pos : SV_POSITION;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

float4 main(Input input) : SV_TARGET {
    float3 lightDir = float3(1, 1, 1);
    lightDir = normalize(lightDir);
    float4 lightDir4 = float4(lightDir, 1.0);

    float4 lightReflect = reflect(lightDir4, input.normal);

    float4 color = tex.Sample(samplerState, input.uv);
    return saturate((0.5 + dot(input.normal, lightDir4) * 0.5) * color);
}
