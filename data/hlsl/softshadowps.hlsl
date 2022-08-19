////////////////////////////////////////////////////////////////////////////////
// Filename: softshadowps.hlsl
////////////////////////////////////////////////////////////////////////////////


//////////////
// TEXTURES //
//////////////
Texture2D shaderTexture;
Texture2D shadowTexture;


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypeClamp;
SamplerState SampleTypeWrap;


//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float4 viewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 SoftShadowPixelShader(PixelInputType input) : SV_TARGET
{
	// ��� �ȼ��� ���� �⺻ ��� ������ �ֺ� ���� ������ �����մϴ�.
    float4 color = ambientColor;

    // �� �ȼ��� ���� ���� ����մϴ�.
	float lightIntensity = saturate(dot(input.normal, input.lightPos));
    if(lightIntensity > 0.0f)
	{
		// Ȯ�� ����� ���� ���� �������� ���� ������ �����մϴ�.
		color += (diffuseColor * lightIntensity);

		// ���� ���� ä��ϴ�.
		color = saturate(color);
	}

	// �� �ؽ�ó ��ǥ ��ġ���� ���÷��� ����Ͽ� �ؽ�ó���� �ȼ� ������ ���ø��մϴ�.
	float4 textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);

	// ���� �ؽ�ó ������ �����մϴ�.
	color = color * textureColor;

	// �׸��� �ؽ�ó�� �Բ� ���� ���� �� �ؽ�ó ��ǥ�� ����մϴ�.
	float2 projectTexCoord = float2(0.0f, 0.0f);
	projectTexCoord.x =  input.viewPosition.x / input.viewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.viewPosition.y / input.viewPosition.w / 2.0f + 0.5f;

	// ���� �� �ؽ�ó ��ǥ ��ġ���� ���÷��� ����Ͽ� �׸��� �ؽ�ó���� �׸��� ���� ���ø��մϴ�.
	float shadowValue = shadowTexture.Sample(SampleTypeClamp, projectTexCoord).r;

	// �׸��ڸ� ���� ����� �����մϴ�.
	color = color * shadowValue;

    return color;
}