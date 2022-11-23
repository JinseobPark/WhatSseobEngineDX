#include "ParticleInclude.hlsl"
#include "LightingUtil.hlsl"

#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif


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
};

struct VS_OUTPUT
{
	float3 Position		: POSITION;
	float Size			: SIZE;
	float4 Color		: COLOR;
};

StructuredBuffer<Particle> ParticlePool		: register(t0);
StructuredBuffer<ParticleDraw> DrawList		: register(t1);

VS_OUTPUT main(uint id : SV_VertexID)
{
	VS_OUTPUT output;

	ParticleDraw draw = DrawList.Load(id);
	Particle particle = ParticlePool.Load(draw.Index);
    
	output.Position = particle.Position;
	output.Size = particle.Size;
	output.Color = particle.Color;

	return output;
}