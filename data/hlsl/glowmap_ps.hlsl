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


	// �� �ؽ�ó ��ǥ ��ġ���� ���÷��� ����Ͽ� �ؽ�ó���� �ȼ� ������ ���ø��մϴ�.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	// �۷ο� ���� ���ø��մϴ�.
	glowMap = glowMapTexture.Sample(SampleType, input.tex);

	// �۷ο� ���� ���� ���̸� ���� ������ ǥ�õ˴ϴ�. �׷��� ������ �۷ο� �ʿ� �������ִ� ��� �ؽ�ó���� ������ ��ȯ�մϴ�.
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