////////////////////////////////////////////////////////////////////////////////
// Filename: transparentdepth.ps
////////////////////////////////////////////////////////////////////////////////


//////////////
// TEXTURES //
//////////////
Texture2D shaderTexture : register(t0);


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType : register(s0);


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD0;
    float2 tex : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TransparentDepthPixelShader(PixelInputType input) : SV_TARGET
{
	float depthValue;
	float4 color;
	float4 textureColor;


	// 이 텍스처 좌표 위치에서 샘플러를 사용하여 텍스처에서 픽셀 색상을 샘플링합니다
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	// 텍스처의 알파 값을 기준으로 테스트합니다.
	if(textureColor.a > 0.8f)
	{
		// Z 픽셀 깊이를 균질 W 좌표로 나누어 픽셀의 깊이 값을 가져옵니다.
		depthValue = input.depthPosition.z / input.depthPosition.w;
	}
	else
	{
		// 그렇지 않으면이 픽셀을 완전히 버립니다.
		discard;
	}

	color = float4(depthValue, depthValue, depthValue, 1.0f);

	return color;
}