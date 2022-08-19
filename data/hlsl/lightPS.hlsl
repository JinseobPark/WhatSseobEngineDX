////////////////////////////////////////////////////////////////////////////////
// Filename: lightPS.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
};

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;
	

	// 이 위치에서 텍스처 픽셀을 샘플링합니다.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
	
	// 모든 픽셀의 기본 출력 색상을 주변 광원 값으로 설정합니다.
    color = ambientColor;

	// 계산을 위해 빛 방향을 반전시킵니다.
    lightDir = -lightDirection;

	// 이 픽셀의 빛의 양을 계산합니다.
    lightIntensity = saturate(dot(input.normal, lightDir));

    if (lightIntensity > 0.0f)
    {
        // // 확산 색과 광 강도의 양에 따라 최종 확산 색을 결정합니다.
        color += (diffuseColor * lightIntensity);
    }

	// 최종 빛의 색상을 채 웁니다.
    color = saturate(color);

	// 텍스처 픽셀과 입력 색상을 곱해 최종 결과를 얻습니다.
    color = color * textureColor;
	
    return color;
}
