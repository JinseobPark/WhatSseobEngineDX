////////////////////////////////////////////////////////////////////////////////
// Filename: reflectionPS.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;
Texture2D reflectionTexture;

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ReflectionPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float2 reflectTexCoord;
    float4 reflectionColor;
    float4 color;


    // �� ��ġ���� �ؽ�ó �ȼ��� ���ø��մϴ�.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    
    // ���� �� �ݻ� �ؽ�ó ��ǥ�� ����մϴ�.
    reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
    reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;

	// ���� �� �ؽ�ó ��ǥ�� ����Ͽ� �ݻ� �ؽ�ó���� �ؽ�ó �ȼ��� ���ø��մϴ�.
    reflectionColor = reflectionTexture.Sample(SampleType, reflectTexCoord);

	// ������ ȿ���� ���� �� �ؽ�ó���� ���� ������ �����մϴ�.
    color = lerp(textureColor, reflectionColor, 0.6f);

    return color;
}