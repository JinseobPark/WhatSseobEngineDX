////////////////////////////////////////////////////////////////////////////////
// Filename: projection_ps.hlsl
////////////////////////////////////////////////////////////////////////////////


//////////////
// TEXTURES //
//////////////
Texture2D shaderTexture : register(t0);
Texture2D projectionTexture : register(t1);


//////////////
// SAMPLERS //
//////////////
SamplerState SampleType;


//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer LightBuffer
{
	float4 ambientColor;
    float4 diffuseColor;
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
	float3 normal : NORMAL;
    float4 viewPosition : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ProjectionPixelShader(PixelInputType input) : SV_TARGET
{
	float2 projectTexCoord;

	// 모든 픽셀의 기본 출력 색상을 주변 광원 값으로 설정합니다.
	float4 color = ambientColor;

	// 계산을 위해 빛 방향을 반전시킵니다.
	float3 lightDir = -lightDirection;

    // 이 픽셀의 빛의 양을 계산합니다.
	float lightIntensity = saturate(dot(input.normal, lightDir));

    if(lightIntensity > 0.0f)
	{
		// 확산 색상과 빛의 양을 기준으로 빛의 색상을 결정합니다.
		color += (diffuseColor * lightIntensity);
	}

	// 밝은 색을 채웁니다.
	color = saturate(color);

	// 이 텍스처 좌표 위치에서 샘플러를 사용하여 텍스처에서 픽셀 색상을 샘플링합니다.
	float4 textureColor = shaderTexture.Sample(SampleType, input.tex);

	// 밝은 색상과 텍스처 색상을 결합합니다.
	color = color * textureColor;

	// 투영 된 텍스처 좌표를 계산합니다.
	projectTexCoord.x =  input.viewPosition.x / input.viewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.viewPosition.y / input.viewPosition.w / 2.0f + 0.5f;

	// 투영 된 좌표가 0에서 1 범위에 있는지 결정합니다. 그 경우이 픽셀은 투영 된 뷰 포트 안에 있습니다.
	if((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		// 투영 된 텍스처 좌표 위치에서 샘플러를 사용하여 투영 텍스처에서 색상 값을 샘플링합니다.
		float4 projectionColor = projectionTexture.Sample(SampleType, projectTexCoord);

		// 이 픽셀의 출력 색상을 일반 색상 값을 무시하는 투영 텍스처로 설정합니다.
        //color = projectionColor;
		// 이 픽셀의 출력 색상을 일반 색상 값을 무시하는 투영 텍스처로 설정합니다.
        color = saturate((color * projectionColor * textureColor) + (ambientColor * textureColor));
    }
	else
    {
        color = ambientColor * textureColor;
    }

    return color;
}