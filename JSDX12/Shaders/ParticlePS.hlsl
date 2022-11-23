#include "ParticleInclude.hlsl"

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
// Constant data that varies per frame.
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
    float4x4 gTexTransform;
    uint gMaterialIndex;
    uint gObjPad0;
    uint gObjPad1;
    uint gObjPad2;
};

// Constant data that varies per material.
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

struct GS_OUTPUT
{
	float4 Position		: SV_POSITION;
	float4 Color		: COLOR;
	float2 UV			: TEXCOORD;
};

float4 main(GS_OUTPUT input) : SV_TARGET
{
	input.UV = input.UV * 2 - 1;

	float fade = saturate(distance(float2(0, 0), input.UV));
	float3 color = lerp(input.Color.rgb, float3(0, 0, 0), fade * fade);

	return float4(color, 1);
}