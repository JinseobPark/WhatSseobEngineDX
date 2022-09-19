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


	// �� �ؽ�ó ��ǥ ��ġ���� ���÷��� ����Ͽ� �ؽ�ó���� �ȼ� ������ ���ø��մϴ�
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	// �ؽ�ó�� ���� ���� �������� �׽�Ʈ�մϴ�.
	if(textureColor.a > 0.8f)
	{
		// Z �ȼ� ���̸� ���� W ��ǥ�� ������ �ȼ��� ���� ���� �����ɴϴ�.
		depthValue = input.depthPosition.z / input.depthPosition.w;
	}
	else
	{
		// �׷��� �������� �ȼ��� ������ �����ϴ�.
		discard;
	}

	color = float4(depthValue, depthValue, depthValue, 1.0f);

	return color;
}