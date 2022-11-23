
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

#include "LightingUtil.hlsl"

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
    float4x4 gTexTransform;
    uint gMaterialIndex;
    uint gObjPad0;
    uint gObjPad1;
    uint gObjPad2;
};


cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float4x4 gViewProjTex;
    float4x4 gShadowTransform;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float3 gCameraRight;
    float cbPerObjectPad3;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
    float4 gAmbientLight;
    
    float4 gFogColor;
    float gFogStart;
    float gFogRange;
    float2 cbPerObjectPad2;
    Light gLights[MaxLights];
};

cbuffer particleData : register(b2)
{
	int emitCount;
	int maxParticles;
	int gridSize;
	float3 velocity;
	float3 acceleration;
	float4 startColor;
	float4 endColor;
	float lifeTime;
}

struct VS_OUTPUT
{
	float3 Position		: POSITION;
	float Size			: SIZE;
	float4 Color		: COLOR;
};

struct GS_OUTPUT
{
	float4 Position		: SV_POSITION;
	float4 Color		: COLOR;
	float2 UV			: TEXCOORD;
};

[maxvertexcount(4)]
void main(point VS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> outStream)
{
	GS_OUTPUT output;

	float2 offsets[4];
	offsets[0] = float2(-1.0f, -1.0f);
	offsets[1] = float2(-1.0f, +1.0f);
	offsets[2] = float2(+1.0f, -1.0f);
	offsets[3] = float2(+1.0f, +1.0f);

	//matrix mvp = mul(mul(world, view), projection);

	[unroll]
	for (int i = 0; i < 4; i++)
	{
        output.Position = mul(mul(float4(input[0].Position, 1.0f), gWorld), gViewProj);

		float depthChange = output.Position.z / output.Position.w;

		// Adjust based on depth (prevents particles from getting too large)
		//offsets[i].y *= aspectRatio;
		output.Position.xy += offsets[i] * depthChange * input[0].Size;
		output.Color = input[0].Color;
		output.UV = saturate(offsets[i]);

		// Done
		outStream.Append(output);
	}
}