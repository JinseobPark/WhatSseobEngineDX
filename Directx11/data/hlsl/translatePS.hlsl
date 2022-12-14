////////////////////////////////////////////////////////////////////////////////
// Filename: translatePS.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;

cbuffer TranslationBuffer
{
    float textureTranslation;
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
float4 TranslatePixelShader(PixelInputType input) : SV_TARGET
{
    input.tex.x += textureTranslation;
    return shaderTexture.Sample(SampleType, input.tex);
}