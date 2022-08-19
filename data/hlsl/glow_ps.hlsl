////////////////////////////////////////////////////////////////////////////////
// Filename: glow_ps.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D colorTexture : register(t0);
Texture2D glowTexture : register(t1);
SamplerState SampleType;

cbuffer GlowBuffer
{
	float glowStrength;
	float3 padding;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 GlowPixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float4 glowColor;
	float4 color;


	// 이 텍스처 좌표 위치에서 샘플러를 사용하여 텍스처에서 픽셀 색상을 샘플링합니다.
	textureColor = colorTexture.Sample(SampleType, input.tex);

	// 광선 텍스처를 샘플링합니다.
	glowColor = glowTexture.Sample(SampleType, input.tex);

	// 광선의 길이에 곱한 광선의 색상에 텍스처 색상을 추가합니다.
    color = saturate(textureColor + (glowColor * glowStrength));

    return color;
}
