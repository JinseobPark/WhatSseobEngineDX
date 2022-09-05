////////////////////////////////////////////////////////////////////////////////
// Filename: deferredmultilight_ps.hlsl
////////////////////////////////////////////////////////////////////////////////

const int MAXDIRLIGHT = 3;
/////////////
// GLOBALS //
/////////////
Texture2D posTexture : register(t0);
Texture2D colorTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D depthTexture : register(t3);
Texture2D shadow[3] : register(t4);


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);
SamplerComparisonState gsamShadow : register(s2);
//SamplerState SampleTypePoint : register(s0);


struct DirLight
{
    float3 m_position;
    float padding1;
    float3 m_direction;
    float padding2;
    float4 m_ambient;
    float4 m_diffuse;
    matrix m_lightVP;
};

struct PointLight
{
    float3 m_position;
    float padding1;
    float4 m_diffuse;
    float4 m_ambient;
    float m_constant;
    float m_linear;
    float m_quadratic;
    float padding2;
};

struct SpotLight
{
    float3 m_position;
    float m_cutOff;
    float3 m_direction;
    float m_outerCutOff;
    float4 m_diffuse;
    float4 m_ambient;
    float m_constant;
    float m_linear;
    float m_quadratic;
    float padding;
};
//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer LightCountBuffer : register(b0)
{
    int dirCount;
    int pointCount;
    int spotCount;
    float padding;
};

cbuffer DirLightBuffer : register(b1)
{
    DirLight dirlight[3];
};
cbuffer PointLightBuffer : register(b2)
{
    PointLight pointlight[30];
};
cbuffer SpotLightBuffer : register(b3)
{
    SpotLight spotlight[30];
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 PosWorld : POSITION0;
};


// function prototypes
float3 CalcDirLight(DirLight light, float3 normal, float3 color);
float3 CalcPointLight(PointLight light, float3 normal, float3 pos, float3 color);
float3 CalcSpotLight(SpotLight light, float3 normal, float3 pos, float3 color);
float CalcShadowFactor(float4 shadowPosH);

float ShadowVariance(float2 moments, float d)
{
    float mean = moments.x;
    float variance = max(moments.y - moments.x * moments.x, 1e-5f);
    float md = mean - d;
    float chebychev = variance / (variance + md * md);

    chebychev = smoothstep(0.1f, 1.0f, chebychev);

	// NEVER forget that d > mean in Chebychev's inequality!!!
    return max(((d <= mean) ? 1.0f : 0.0f), chebychev);
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
    float4 positions;
	float3 colors;
	float3 normals;
    float depths;
    float4 shadows;
    
    
    // Get informatino from textures
    positions = posTexture.Sample(SampleTypeClamp, input.tex);
    colors = colorTexture.Sample(SampleTypeClamp, input.tex).xyz;
    normals = normalTexture.Sample(SampleTypeClamp, input.tex).xyz;
    normalize(normals);
    depths = depthTexture.Sample(SampleTypeClamp, input.tex).r;
    
    
    
    float3 result = float3(0, 0, 0);
  
    
    
    if(depths.r > 0.0f)
    {
         //dir light calculation
        for (int i = 0; i < dirCount; i++)
        {
            float4 lspos = mul(float4(positions.xyz, 1.0f), dirlight[i].m_lightVP);
            float d = lspos.z;
            float2 ptex = (lspos.xy / lspos.w) * float2(0.5f, -0.5f) + 0.5f;
    
            float depthV = shadow[i].Sample(SampleTypeWrap, ptex).x + 0.01f;
            depthV = (lspos.z < depthV) ? 1 : 0;
            
            //float2 depthV = shadow[i].Sample(SampleTypeWrap, ptex).xy;
            
            //float shadow = ShadowVariance(depthV, d);
            
            
            result += depthV * CalcDirLight(dirlight[i], normals, colors);
        }
        //{
        //    result += depthV * CalcDirLight(dirlight[i], normals, colors);
        //}
        
        // dir light with shadow
        //result += CalcDirLight(dirlight[1], normals, colors);
        
        //point light calculation
        for (int i = 0; i < pointCount; i++)
        {
            result += CalcPointLight(pointlight[i], normals, positions.xyz, colors);
        }
        
        //spot light calculation
        for (int i = 0; i < spotCount; i++)
        {
            result += CalcSpotLight(spotlight[i], normals, positions.xyz, colors);
        }
    
    }
    else
    {
        result = (0.0f, 0.0f, 0.0f);

    }
    
    //result += depthV.xxx;
    //return float4(shadows, shadows, shadows, 1.0f);
    //return float4(shadow, shadow, shadow, 1.0f);
    //return float4(shadowFactor, 1.0f);
    return float4(result, 1.0f);
    
    }


// calculates the color when using a directional light.
float3 CalcDirLight(DirLight light, float3 normal, float3 color)
{
    float3 lightDir = normalize(-light.m_direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // combine results
    float3 ambient = light.m_ambient.xyz * color;
    float3 diffuse = light.m_diffuse.xyz * diff * color;
    return (ambient + diffuse);
}

// calculates the color when using a point light.
float3 CalcPointLight(PointLight light, float3 normal, float3 pos, float3 color)
{
    float3 lightDir = normalize(light.m_position - pos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // attenuation
    float distance = length(light.m_position - pos);
    float attenuation = 1.0 / (light.m_constant + light.m_linear * distance+light.m_quadratic * (distance * distance));
    // combine results
    float3 ambient = light.m_ambient.xyz * color;
    float3 diffuse = light.m_diffuse.xyz * diff * color;
    
    ambient *= attenuation;
    diffuse *= attenuation;
    
    return (ambient + diffuse);
}

// calculates the color when using a spot light.
float3 CalcSpotLight(SpotLight light, float3 normal, float3 pos, float3 color)
{
    float3 lightDir = normalize(light.m_position - pos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // attenuation
    float distance = length(light.m_position - pos);
    float attenuation = 1.0 / (light.m_constant + light.m_linear * distance + light.m_quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.m_direction));
    float epsilon = light.m_cutOff - light.m_outerCutOff;
    float intensity = clamp((theta - light.m_outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    float3 ambient = light.m_ambient.xyz * color;
    float3 diffuse = light.m_diffuse.xyz * diff * color;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    return (ambient + diffuse);
}



float CalcShadowFactor(float4 shadowPosH)
{
    // Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;

    // Depth in NDC space.
    float depth = shadowPosH.z;

    uint width, height, numMips;
    shadow[0].GetDimensions(0, width, height, numMips);

    // Texel size.
    float dx = 1.0f / (float) width;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadow[0].SampleCmpLevelZero(gsamShadow,
            shadowPosH.xy + offsets[i], depth).r;
    }
    
    return percentLit / 9.0f;
}