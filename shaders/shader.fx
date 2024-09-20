Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

float4 PS_Main(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD) : SV_TARGET {

    return tex.Sample(samplerState, texCoord); // Sample 
}
