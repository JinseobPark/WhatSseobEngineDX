////////////////////////////////////////////////////////////////////////////////
// Filename: color_hs.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer TessellationBuffer
{
    float tessellationAmount;
    float3 padding;
};


//////////////
// TYPEDEFS //
//////////////
struct HullInputType
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct HullOutputType
{
    float3 position : POSITION;
    float4 color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Patch Constant Function
////////////////////////////////////////////////////////////////////////////////
ConstantOutputType ColorPatchConstantFunction(InputPatch<HullInputType, 3> inputPatch, uint patchId : SV_PrimitiveID)
{    
    ConstantOutputType output;

	// 삼각형의 세 모서리에 대한 모따기 인수를 설정합니다.
    output.edges[0] = tessellationAmount;
    output.edges[1] = tessellationAmount;
    output.edges[2] = tessellationAmount;

	// 트라이앵글 내부를 테셀레이션하기위한 테셀레이션 계수를 설정합니다.
    output.inside = tessellationAmount;

    return output;
}


////////////////////////////////////////////////////////////////////////////////
// Hull Shader
////////////////////////////////////////////////////////////////////////////////
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ColorPatchConstantFunction")]

HullOutputType ColorHullShader(InputPatch<HullInputType, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    HullOutputType output;


	// 이 제어점의 위치를 ​​출력 위치로 설정합니다.
    output.position = patch[pointId].position;

	// 입력 색상을 출력 색상으로 설정합니다.
    output.color = patch[pointId].color;

    return output;
}
