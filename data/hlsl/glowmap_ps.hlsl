////////////////////////////////////////////////////////////////////////////////
// Filename: glowmap_ps.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture : register(t0);
Texture2D glowMapTexture : register(t1);
SamplerState SampleType;


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
float4 GlowMapPixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float4 glowMap;
	float4 color;


	// 이 텍스처 좌표 위치에서 샘플러를 사용하여 텍스처에서 픽셀 색상을 샘플링합니다.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	// 글로우 맵을 샘플링합니다.
	glowMap = glowMapTexture.Sample(SampleType, input.tex);

	// 글로우 맵이 검은 색이면 검은 색으로 표시됩니다. 그렇지 않으면 글로우 맵에 색상이있는 경우 텍스처에서 색상을 반환합니다.
	if((glowMap.r == 0.0f) && (glowMap.g == 0.0f) && (glowMap.b == 0.0f))
	{
		color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else
	{
		color = textureColor;
	}

    return color;
}
