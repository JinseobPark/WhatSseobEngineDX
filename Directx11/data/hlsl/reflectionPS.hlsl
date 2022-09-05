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


    // 이 위치에서 텍스처 픽셀을 샘플링합니다.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    
    // 투사 된 반사 텍스처 좌표를 계산합니다.
    reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
    reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;

	// 투영 된 텍스처 좌표를 사용하여 반사 텍스처에서 텍스처 픽셀을 샘플링합니다.
    reflectionColor = reflectionTexture.Sample(SampleType, reflectTexCoord);

	// 블렌드 효과를 위해 두 텍스처간에 선형 보간을 수행합니다.
    color = lerp(textureColor, reflectionColor, 0.6f);

    return color;
}