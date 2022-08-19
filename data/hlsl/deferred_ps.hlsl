////////////////////////////////////////////////////////////////////////////////
// Filename: deferred_ps.hlsl
////////////////////////////////////////////////////////////////////////////////


//////////////
// TEXTURES //
//////////////
Texture2D shaderTexture : register(t0);


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypeWrap : register(s0);


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION0;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float4 worldpos : TEXCOORD1;
};

struct PixelOutputType
{
    float4 position : SV_Target0;
    float4 color : SV_Target1;
    float4 normal : SV_Target2;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
PixelOutputType DeferredPixelShader(PixelInputType input) : SV_TARGET
{
	PixelOutputType output;

    output.position = input.worldpos;
	// �ؽ�ó���� ������ ���ø��ϰ� ������ ��� ����ϱ� ���� �����մϴ�.
	output.color = shaderTexture.Sample(SampleTypeWrap, input.tex);
	
	// ���� Ÿ�ٿ� �ƿ�ǲ�� �����Ѵ�.
	output.normal = float4(input.normal, 1.0f);

    return output;
}