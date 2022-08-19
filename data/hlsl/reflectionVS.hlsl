////////////////////////////////////////////////////////////////////////////////
// Filename: reflectionVS.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer ReflectionBuffer
{
    matrix reflectionMatrix;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ReflectionVertexShader(VertexInputType input)
{
    PixelInputType output;
    matrix reflectProjectWorld;
	

	// 적절한 행렬 계산을 위해 위치 벡터를 4 단위로 변경합니다.
    input.position.w = 1.0f;

	// 월드, 뷰 및 투영 행렬에 대한 정점의 위치를 ​​계산합니다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
    output.tex = input.tex;

    // 반사 투영 세계 행렬을 만듭니다.
    reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
    reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

	// reflectProjectWorld 행렬에 대한 입력 위치를 계산합니다.
    output.reflectionPosition = mul(input.position, reflectProjectWorld);

    return output;
}