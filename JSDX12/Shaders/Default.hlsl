//***************************************************************************************
// Default.hlsl
//***************************************************************************************

#include "Common.hlsl"

struct VertexIn
{
	float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
    float2 TexC : TEXCOORD;
    float3 TangentU : TANGENT;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 ShadowPosH : POSITION0;
    float4 SsaoPosH : POSITION1;
    float3 PosW    : POSITION2;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
	float2 TexC    : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout = (VertexOut) 0.0f;

	// material data by index
    MaterialData matData = gMaterialData[gMaterialIndex];
	
    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;

    // normal, tangent transform to world space
    vout.NormalW = mul(vin.NormalL, (float3x3) gWorld);
	
    vout.TangentW = mul(vin.TangentU, (float3x3) gWorld);

    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);

    // Generate projective tex-coords to project SSAO map onto scene.
    vout.SsaoPosH = mul(posW, gViewProjTex);
	
	// Texture coordinate
    float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
    vout.TexC = mul(texC, matData.MatTransform).xy;

    // generate projective shadow map 
    vout.ShadowPosH = mul(posW, gShadowTransform);
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	// fetch material data.
	MaterialData matData = gMaterialData[gMaterialIndex];
	float4 diffuseAlbedo = matData.DiffuseAlbedo;
	float3 fresnelR0 = matData.FresnelR0;
	float  roughness = matData.Roughness;
    uint diffuseTexIndex = matData.DiffuseMapIndex;
    uint normalMapIndex = matData.NormalMapIndex;
    
    uint isNormal = matData.hasNormalMap;
    float4 normalMapSample = 1.0f;
    float3 bumpedNormalW;// = (1 - isNormal) * pin.NormalW;
    
    pin.NormalW = normalize(pin.NormalW);
    float3 normalW = pin.NormalW;
    //if have bump normal data, calculate normalmap
    if (isNormal)
    {
        normalMapSample = gTextureMaps[normalMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);
        bumpedNormalW = NormalSampleToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW);
        //normalMapSample = (isNormal) * gTextureMaps[normalMapIndex].Sample(gsamAnisotropicWrap, pin.TexC) + (1 - isNormal) * 1.0f;
        //bumpedNormalW = (isNormal) * NormalSampleToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW) + (1 - isNormal) * pin.NormalW;
    }
    else //bump = normal
    {
        bumpedNormalW =  pin.NormalW;
        
        normalMapSample = 1.0f;
        //bumpedNormalW = (isNormal) * NormalSampleToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW) + (1 - isNormal) * pin.NormalW;
    }

    
	// dynamic texture array
    diffuseAlbedo *= gTextureMaps[diffuseTexIndex].Sample(gsamAnisotropicWrap, pin.TexC);
	

    // Vector from point being lit to eye. 
    float3 toEyeW = normalize(gEyePosW - pin.PosW);
    
    // Finish texture projection and sample SSAO map.
    pin.SsaoPosH /= pin.SsaoPosH.w;
    float ambientAccess = gSsaoMap.Sample(gsamLinearClamp, pin.SsaoPosH.xy, 0.0f).r;

    // Light terms.
    float4 ambient = ambientAccess * gAmbientLight * diffuseAlbedo;
    
    // shadow calculate
    float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
    shadowFactor[0] = CalcShadowFactor(pin.ShadowPosH);
    
    const float shininess = (1.0f - roughness);// * normalMapSample.a;
    Material mat = { diffuseAlbedo, fresnelR0, shininess };
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
        bumpedNormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

	// Add in specular reflections.
    float3 r = reflect(-toEyeW, bumpedNormalW);
    float4 reflectionColor = gCubeMap.Sample(gsamLinearWrap, r);
    float3 fresnelFactor = SchlickFresnel(fresnelR0, bumpedNormalW, r);
    litColor.rgb += shininess * fresnelFactor * reflectionColor.rgb;
    
#ifdef FOG
    float distToEye = length(gEyePosW - pin.PosW);
	float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
	litColor = lerp(litColor, gFogColor, fogAmount);
#endif
    
    // Common convention to take alpha from diffuse albedo.
    litColor.a = diffuseAlbedo.a;
    return litColor;
}


