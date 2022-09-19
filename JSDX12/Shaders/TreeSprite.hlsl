//***************************************************************************************
// TreeSprite.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

// Include structures and functions for lighting.
#include "Common.hlsl"
//#include "LightingUtil.hlsl"

//struct MaterialData
//{
//    float4 DiffuseAlbedo;
//    float3 FresnelR0;
//    float Roughness;
//    float4x4 MatTransform;
//    uint DiffuseMapIndex;
//    uint MatPad0;
//    uint MatPad1;
//    uint MatPad2;
//};


//TextureCube gCubeMap : register(t0);
//Texture2D gDiffuseMap[10] : register(t1);
////Texture2DArray gTreeMapArray : register(t0);

//StructuredBuffer<MaterialData> gMaterialData : register(t0, space1);

//SamplerState gsamPointWrap : register(s0);
//SamplerState gsamPointClamp : register(s1);
//SamplerState gsamLinearWrap : register(s2);
//SamplerState gsamLinearClamp : register(s3);
//SamplerState gsamAnisotropicWrap : register(s4);
//SamplerState gsamAnisotropicClamp : register(s5);

//// Constant data that varies per frame.
//cbuffer cbPerObject : register(b0)
//{
//    float4x4 gWorld;
//    float4x4 gTexTransform;
//    uint gMaterialIndex;
//    uint gObjPad0;
//    uint gObjPad1;
//    uint gObjPad2;
//};

//// Constant data that varies per material.
//cbuffer cbPass : register(b1)
//{
//    float4x4 gView;
//    float4x4 gInvView;
//    float4x4 gProj;
//    float4x4 gInvProj;
//    float4x4 gViewProj;
//    float4x4 gInvViewProj;
//    float3 gEyePosW;
//    float cbPerObjectPad1;
//    float2 gRenderTargetSize;
//    float2 gInvRenderTargetSize;
//    float gNearZ;
//    float gFarZ;
//    float gTotalTime;
//    float gDeltaTime;
//    float4 gAmbientLight;

//    float4 gFogColor;
//    float gFogStart;
//    float gFogRange;
//    float2 cbPerObjectPad2;

//    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
//    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
//    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
//    // are spot lights for a maximum of MaxLights per object.
//    Light gLights[MaxLights];
//};

 
struct VertexIn
{
    float3 PosW : POSITION;
    float2 SizeW : SIZE;
};

struct VertexOut
{
    float3 CenterW : POSITION;
    float2 SizeW : SIZE;
};

struct GeoOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 TexC : TEXCOORD;
    uint PrimID : SV_PrimitiveID;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    
	// Fetch the material data.
    MaterialData matData = gMaterialData[gMaterialIndex];
	// Just pass data over to geometry shader.
    vout.CenterW = vin.PosW;
    vout.SizeW = vin.SizeW;

    return vout;
}
 
 // We expand each point into a quad (4 vertices), so the maximum number of vertices
 // we output per geometry shader invocation is 4.
[maxvertexcount(4)]
void GS(point VertexOut gin[1],
        uint primID : SV_PrimitiveID,
        inout TriangleStream<GeoOut> triStream)
{
	//
	// Compute the local coordinate system of the sprite relative to the world
	// space such that the billboard is aligned with the y-axis and faces the eye.
	//

    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 look = gEyePosW - gin[0].CenterW;
    look.y = 0.0f; // y-axis aligned, so project to xz-plane
    look = normalize(look);
    float3 right = cross(up, look);

	//
	// Compute triangle strip vertices (quad) in world space.
	//
    float halfWidth = 0.5f * gin[0].SizeW.x;
    float halfHeight = 0.5f * gin[0].SizeW.y;
	
    float4 v[4];
    v[0] = float4(gin[0].CenterW + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(gin[0].CenterW + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(gin[0].CenterW - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(gin[0].CenterW - halfWidth * right + halfHeight * up, 1.0f);

	//
	// Transform quad vertices to world space and output 
	// them as a triangle strip.
	//
	
    float2 texC[4] =
    {
        float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
    };
	
    GeoOut gout;
	[unroll]
    for (int i = 0; i < 4; ++i)
    {
        gout.PosH = mul(v[i], gViewProj);
        gout.PosW = v[i].xyz;
        gout.NormalW = look;
        gout.TexC = texC[i];
        gout.PrimID = primID;
		
        triStream.Append(gout);
    }
}

float4 PS(GeoOut pin) : SV_Target
{
	// Fetch the material data.
    MaterialData matData = gMaterialData[gMaterialIndex];
    float4 diffuseAlbedo = matData.DiffuseAlbedo;
    float3 fresnelR0 = matData.FresnelR0;
    float roughness = matData.Roughness;
    uint diffuseTexIndex = matData.DiffuseMapIndex;
    float3 uvw = float3(pin.TexC, pin.PrimID % 3);
    diffuseAlbedo *= gDiffuseMap[8].Sample(gsamAnisotropicWrap, uvw.xy);
	
#ifdef ALPHA_TEST
	// Discard pixel if texture alpha < 0.1.  We do this test as soon 
	// as possible in the shader so that we can potentially exit the
	// shader early, thereby skipping the rest of the shader code.
	clip(diffuseAlbedo.a - 0.1f);
#endif

    // Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);

    // Vector from point being lit to eye. 
    float3 toEyeW = gEyePosW - pin.PosW;
    float distToEye = length(toEyeW);
    toEyeW /= distToEye; // normalize

    // Light terms.
    float4 ambient = gAmbientLight * diffuseAlbedo;
    
    const float shininess = 1.0f - roughness;
    Material mat = { diffuseAlbedo, fresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
        pin.NormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

#ifdef FOG
	float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
	litColor = lerp(litColor, gFogColor, fogAmount);
#endif

    // Common convention to take alpha from diffuse albedo.
    litColor.a = diffuseAlbedo.a;

    return litColor;
}


