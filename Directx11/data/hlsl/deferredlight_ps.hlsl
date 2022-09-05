////////////////////////////////////////////////////////////////////////////////
// Filename: deferredlight_ps.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypePoint : register(s0);


//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer LightBuffer
{
	float3 lightDirection;
	float padding;
};


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
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
	float4 colors;
	float4 normals;
	float3 lightDir;
	float lightIntensity;
	float4 outputColor;
	
    float ambient = 0.2f;
    float4 ambientColor;


	// 이 텍스처 좌표 위치에서 포인트 샘플러를 사용하여 컬러 렌더링 텍스처에서 색상을 샘플링합니다.
	colors = colorTexture.Sample(SampleTypePoint, input.tex);
    ambientColor  = colors * ambient;

	// 이 텍스쳐 좌표 위치에있는 포인트 샘플러를 사용하여 normal 렌더 텍스처로부터 법선을 샘플링합니다.
	normals = normalTexture.Sample(SampleTypePoint, input.tex);
	
	// 계산을 위해 빛 방향을 반전시킵니다.
	lightDir = -lightDirection;

	// 이 픽셀의 빛의 양을 계산합니다.
	lightIntensity = saturate(dot(normals.xyz, lightDir));

	// 광도와 결합 된 픽셀의 색상을 기반으로 최종 색상 확산 량을 결정합니다.
    if (!colors.x && !colors.y && !colors.z)
    ambientColor.w = 0.0f;
    else
    {
		ambientColor.w = 1.0f;
    }
    //outputColor = float4(ambientColor.x, ambientColor.y, ambientColor.z, 1.0f);
    outputColor = saturate(ambientColor + saturate(colors * lightIntensity));
    //outputColor = saturate(colors * lightIntensity);

	return outputColor;
}