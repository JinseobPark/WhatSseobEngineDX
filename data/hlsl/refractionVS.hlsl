////////////////////////////////////////////////////////////////////////////////
// Filename: refractionVS.hlsl
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

cbuffer ClipPlaneBuffer
{
    float4 clipPlane;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float clip : SV_ClipDistance0;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType RefractionVertexShader(VertexInputType input)
{
    PixelInputType output;
    
	// 적절한 행렬 계산을 위해 위치 벡터를 4 단위로 변경합니다.
    input.position.w = 1.0f;

	// 월드, 뷰 및 투영 행렬에 대한 정점의 위치를 ​​계산합니다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
    output.tex = input.tex;

	// 월드 행렬에 대해서만 법선 벡터를 계산합니다.
    output.normal = mul(input.normal, (float3x3) worldMatrix);

	// 법선 벡터를 정규화합니다.
    output.normal = normalize(output.normal);

	// 클리핑면을 설정합니다.
    output.clip = dot(mul(input.position, worldMatrix), clipPlane);

    return output;
}