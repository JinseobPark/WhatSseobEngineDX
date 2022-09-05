////////////////////////////////////////////////////////////////////////////////
// Filename: shadowMultiVS2.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
	matrix lightViewMatrix2;
	matrix lightProjectionMatrix2;
};


//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer LightBuffer2
{
    float3 lightPosition;
	float padding1;
    float3 lightPosition2;
	float padding2;
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
    float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
    float4 lightViewPosition2 : TEXCOORD3;
	float3 lightPos2 : TEXCOORD4;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ShadowVertexShader(VertexInputType input)
{
    PixelInputType output;
	float4 worldPosition;
    
    
	// 적절한 행렬 계산을 위해 위치 벡터를 4 단위로 변경합니다.
    input.position.w = 1.0f;

	// 월드, 뷰 및 투영 행렬에 대한 정점의 위치를 ​​계산합니다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// 광원에 의해 보았을 때 vertice의 위치를 ​​계산합니다.
    output.lightViewPosition = mul(input.position, worldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

	// 두 번째 광원에서 보았을 때 vertice의 위치를 ​​계산합니다.
    output.lightViewPosition2 = mul(input.position, worldMatrix);
    output.lightViewPosition2 = mul(output.lightViewPosition2, lightViewMatrix2);
    output.lightViewPosition2 = mul(output.lightViewPosition2, lightProjectionMatrix2);

	//픽셀 쉐이더의 텍스처 좌표를 저장한다.
    output.tex = input.tex;
    
	// 월드 행렬에 대해서만 법선 벡터를 계산합니다.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // 법선 벡터를 정규화합니다.
    output.normal = normalize(output.normal);

    // 세계의 정점 위치를 계산합니다.
    worldPosition = mul(input.position, worldMatrix);

    // 빛의 위치와 세계의 정점 위치를 기반으로 빛의 위치를 ​​결정합니다.
    output.lightPos = lightPosition.xyz - worldPosition.xyz;

    // 라이트 위치 벡터를 정규화합니다.
    output.lightPos = normalize(output.lightPos);

    // 빛의 위치와 세계의 정점 위치를 기반으로 두 번째 빛의 위치를 ​​결정합니다.
    output.lightPos2 = lightPosition2.xyz - worldPosition.xyz;

    // 두 번째 라이트 위치 벡터를 정규화합니다.
    output.lightPos2 = normalize(output.lightPos2);

	return output;
}