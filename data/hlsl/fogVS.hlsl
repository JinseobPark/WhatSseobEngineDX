////////////////////////////////////////////////////////////////////////////////
// Filename: fogVS.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer PerFrameBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer FogBuffer
{
	float fogStart;
	float fogEnd;
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
	float fogFactor : FOG;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType FogVertexShader(VertexInputType input)
{
    PixelInputType output;
	float4 cameraPosition;
    

	// 적절한 행렬 계산을 위해 위치 벡터를 4 단위로 변경합니다.
    input.position.w = 1.0f;

	// 월드, 뷰 및 투영 행렬에 대한 정점의 위치를 ​​계산합니다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// 픽셀 쉐이더의 텍스처 좌표를 저장한다.
	output.tex = input.tex;
    
    // 카메라 위치를 계산합니다.
    cameraPosition = mul(input.position, worldMatrix);
    cameraPosition = mul(cameraPosition, viewMatrix);

    // 선형 안개를 계산합니다.      
    output.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart));

    return output;
}