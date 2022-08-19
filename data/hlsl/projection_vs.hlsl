////////////////////////////////////////////////////////////////////////////////
// Filename: projection_vs.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix viewMatrix2;
	matrix projectionMatrix2;
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
    float4 viewPosition : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ProjectionVertexShader(VertexInputType input)
{
    PixelInputType output;
    

	// 적절한 행렬 계산을 위해 위치 벡터를 4 단위로 변경합니다.
    input.position.w = 1.0f;

	// 월드, 뷰 및 투영 행렬에 대한 정점의 위치를 ​​계산합니다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// 투영 뷰 포인트에서 보았을 때 꼭지점의 위치를 ​​별도의 변수에 저장합니다.
    output.viewPosition = mul(input.position, worldMatrix);
    output.viewPosition = mul(output.viewPosition, viewMatrix2);
    output.viewPosition = mul(output.viewPosition, projectionMatrix2);

	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
	output.tex = input.tex;
    
	// 월드 행렬에 대해서만 법선 벡터를 계산합니다.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // 법선 벡터를 정규화합니다.
    output.normal = normalize(output.normal);

    return output;
}