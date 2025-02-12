Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

struct Input {
    float4 pos : SV_POSITION;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

float4 main(Input input) : SV_TARGET {
    float4 color = tex.Sample(samplerState, input.uv);

    clip(color.a < 0.1 ? -1 : 1);

    float4 lightDirReflected = float4(1, 1, 1, 0);
    float NdotL = saturate(dot(input.normal, lightDirReflected));

    color.rgb = color.rgb * (0.5 + NdotL * 0.5);
    
    return color;
}
