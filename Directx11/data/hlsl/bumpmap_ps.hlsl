////////////////////////////////////////////////////////////////////////////////
// Filename: bumpmap_ps.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D colorTexture : register(t0);
Texture2D normalMapTexture : register(t1);
SamplerState SampleType;

cbuffer LightBuffer
{
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
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 BumpMapPixelShader(PixelInputType input) : SV_TARGET
{
	//이 위치에서 텍스처 픽셀을 샘플링합니다.
	float4 textureColor = colorTexture.Sample(SampleType, input.tex);

	// 범프 맵에서 픽셀을 샘플링합니다.
	float4 bumpMap = normalMapTexture.Sample(SampleType, input.tex);

	// 정상 값의 범위를 (0, +1)에서 (-1, +1)로 확장합니다.
	bumpMap = (bumpMap * 2.0f) - 1.0f;

	// 범프 맵의 데이터에서 법선을 계산합니다.
	float3 bumpNormal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;
	
	// 결과 범프 법선을 표준화합니다.
	bumpNormal = normalize(bumpNormal);

	// 계산을 위해 빛 방향을 반전시킵니다.
	float3 lightDir = -lightDirection;

	// 범프 맵 일반 값을 기반으로 픽셀의 빛의 양을 계산합니다.
	float lightIntensity = saturate(dot(bumpNormal, lightDir));

	// 확산 색과 광 강도의 양에 따라 최종 확산 색을 결정합니다.
	float4 color = saturate(diffuseColor * lightIntensity);

	// 최종 범프 라이트 색상과 텍스처 색상을 결합합니다.
	color = color * textureColor;
	
	return color;
}
