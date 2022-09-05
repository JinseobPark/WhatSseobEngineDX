////////////////////////////////////////////////////////////////////////////////
// Filename: DeferredInstanceTexturePS.hlsl
////////////////////////////////////////////////////////////////////////////////


//////////////
// TEXTURES //
//////////////
Texture2D shaderTexture : register(t0);


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType : register(s0);


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 worldpos : TEXTURE1;
    float4 depthPosition : TEXTURE2;
};

struct PixelOutputType
{
    float4 position : SV_Target0;
    float4 color : SV_Target1;
    float4 normal : SV_Target2;
    float4 depth : SV_Target3;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
PixelOutputType TexturePixelShader(PixelInputType input) : SV_TARGET
{
    PixelOutputType output;

    
    // ��ġ �ؽ�ó ����
    output.position = input.worldpos;
    // diffuse�ؽ�ó ����
    output.color = shaderTexture.Sample(SampleType, input.tex);
	
	// ��ߺ��� �ؽ�ó ����
    output.normal = float4(input.normal, 1.0f);
    
    // ���� �ؽ�ó ����
    float depthValue = input.depthPosition.z / input.depthPosition.w;
    output.depth = float4(depthValue, depthValue, depthValue, 1.0f);
    return output;
}
