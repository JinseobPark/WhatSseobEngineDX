////////////////////////////////////////////////////////////////////////////////
// Filename: deferredmultilight_ps.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D posTexture : register(t0);
Texture2D colorTexture : register(t1);
Texture2D normalTexture : register(t2);


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypePoint : register(s0);


struct DirLight
{
    float3 m_position;
    float padding1;
    float3 m_direction;
    float padding2;
    float4 m_ambient;
    float4 m_diffuse;
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
};


// function prototypes
float3 CalcDirLight(DirLight light, float3 normal, float3 color);
float3 CalcPointLight(PointLight light, float3 normal, float3 pos, float3 color);
float3 CalcSpotLight(SpotLight light, float3 normal, float3 pos, float3 color);


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
    float4 positions;
	float3 colors;
	float3 normals;
    
    // Get informatino from textures
    positions = posTexture.Sample(SampleTypePoint, input.tex);
    colors = colorTexture.Sample(SampleTypePoint, input.tex);
	normals = normalTexture.Sample(SampleTypePoint, input.tex).xyz;
    
    normalize(normals);
    
    float3 result = float3(0, 0, 0);
    
    //dir light calculation
    for (int i = 0; i < dirCount; i++)
    {
       result += CalcDirLight(dirlight[i], normals, colors);
    }
    
    for (int i = 0; i < pointCount; i++)
    {
        result += CalcPointLight(pointlight[i], normals, positions.xyz, colors);
    }
    
    for (int i = 0; i < spotCount; i++)
    {
        result += CalcSpotLight(spotlight[i], normals, positions.xyz, colors);
    }
    
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