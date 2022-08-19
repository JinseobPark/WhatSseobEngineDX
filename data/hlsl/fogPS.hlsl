////////////////////////////////////////////////////////////////////////////////
// Filename: fogPS.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float fogFactor : FOG;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 FogPixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
    float4 fogColor;
    float4 finalColor;

	
    // �� ��ġ���� �ؽ�ó �ȼ��� ���ø��մϴ�.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    
    // �Ȱ��� ���� ȸ������ �����մϴ�.
    fogColor = float4(0.5f, 0.5f, 0.5f, 1.0f);

    // �Ȱ� ȿ�� �������� ����Ͽ� ���� ������ ����մϴ�.
    finalColor = input.fogFactor * textureColor + (1.0 - input.fogFactor) * fogColor;

	return finalColor;
}