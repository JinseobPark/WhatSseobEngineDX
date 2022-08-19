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
	//�� ��ġ���� �ؽ�ó �ȼ��� ���ø��մϴ�.
	float4 textureColor = colorTexture.Sample(SampleType, input.tex);

	// ���� �ʿ��� �ȼ��� ���ø��մϴ�.
	float4 bumpMap = normalMapTexture.Sample(SampleType, input.tex);

	// ���� ���� ������ (0, +1)���� (-1, +1)�� Ȯ���մϴ�.
	bumpMap = (bumpMap * 2.0f) - 1.0f;

	// ���� ���� �����Ϳ��� ������ ����մϴ�.
	float3 bumpNormal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;
	
	// ��� ���� ������ ǥ��ȭ�մϴ�.
	bumpNormal = normalize(bumpNormal);

	// ����� ���� �� ������ ������ŵ�ϴ�.
	float3 lightDir = -lightDirection;

	// ���� �� �Ϲ� ���� ������� �ȼ��� ���� ���� ����մϴ�.
	float lightIntensity = saturate(dot(bumpNormal, lightDir));

	// Ȯ�� ���� �� ������ �翡 ���� ���� Ȯ�� ���� �����մϴ�.
	float4 color = saturate(diffuseColor * lightIntensity);

	// ���� ���� ����Ʈ ����� �ؽ�ó ������ �����մϴ�.
	color = color * textureColor;
	
	return color;
}