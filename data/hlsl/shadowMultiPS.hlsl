////////////////////////////////////////////////////////////////////////////////
// Filename: shadowMultiPS.hlsl
////////////////////////////////////////////////////////////////////////////////


//////////////
// TEXTURES //
//////////////
Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);
Texture2D depthMapTexture2 : register(t2);
Texture2D depthMapTexture3 : register(t3);


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap  : register(s1);


//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float4 diffuseColor2;
    float4 diffuseColor3;
    float3 lightDirection;
    float padding;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
    float4 lightViewPosition2 : TEXCOORD3;
    float3 lightPos2 : TEXCOORD4;
    float4 lightViewPosition3 : TEXCOORD5;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ShadowPixelShader(PixelInputType input) : SV_TARGET
{
	float bias;
    float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
    float lightIntensity;
    float4 textureColor;
    float3 lightDir;


	// �ε� �Ҽ��� ���е� ������ �ذ��� ���̾ ���� �����մϴ�.
	bias = 0.001f;

	// ��� �ȼ��� ���� �⺻ ��� ������ �ֺ� ���� ������ �����մϴ�.
    color = ambientColor;

	// ���� �� �ؽ�ó ��ǥ�� ����մϴ�.
	projectTexCoord.x =  input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	// ���� �� ��ǥ�� 0���� 1 ������ �ִ��� �����մϴ�. �׷��ٸ��� �ȼ��� ���� ������ �ֽ��ϴ�.
	if((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		// ���� �� �ؽ�ó ��ǥ ��ġ���� ���÷��� ����Ͽ� ���� �ؽ�ó���� ������ �� ���� ���� ���ø��մϴ�.
		depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;

		// ���� ���̸� ����մϴ�.
		lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

		// lightDepthValue���� ���̾�� ���ϴ�.
		lightDepthValue = lightDepthValue - bias;

		// ������ �� ���� ���̿� ���� ���̸� ���Ͽ��� �ȼ��� ���� ó������ �������� �����մϴ�.
		// ���� ��ü �տ� ������ �ȼ��� ���߰�, �׷��� ������ ��ü (��Ŭ ���)�� �׸��ڸ� �帮 ��� �������� �ȼ��� �׸��ڷ� �׸��ϴ�.
		if(lightDepthValue < depthValue)
		{
		    // �� �ȼ��� ���� ���� ����մϴ�.
			lightIntensity = saturate(dot(input.normal, input.lightPos));

		    if(lightIntensity > 0.0f)
			{
				// Ȯ�� ���� �� ������ �翡 ���� ���� Ȯ�� ���� �����մϴ�.
				color += (diffuseColor * lightIntensity);
			}
		}
	}

	// �ι�° ����
	projectTexCoord.x =  input.lightViewPosition2.x / input.lightViewPosition2.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition2.y / input.lightViewPosition2.w / 2.0f + 0.5f;

	if((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		depthValue = depthMapTexture2.Sample(SampleTypeClamp, projectTexCoord).r;

		lightDepthValue = input.lightViewPosition2.z / input.lightViewPosition2.w;
		lightDepthValue = lightDepthValue - bias;

		if(lightDepthValue < depthValue)
		{
			lightIntensity = saturate(dot(input.normal, input.lightPos2));
		    if(lightIntensity > 0.0f)
			{
				color += (diffuseColor2 * lightIntensity);
			}
		}
	}
	
	// ���� ������ ������ŵ�ϴ�.
    lightDir = -lightDirection;
	
	// ���� �� �ؽ�ó ��ǥ�� ����մϴ�.
    projectTexCoord.x = input.lightViewPosition3.x / input.lightViewPosition3.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.lightViewPosition3.y / input.lightViewPosition3.w / 2.0f + 0.5f;
	
	// ���� �� ��ǥ�� 0���� 1 ������ �ִ��� �����մϴ�. �׷��ٸ� �� �ȼ��� ���� ������ �ֽ��ϴ�.
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
		// ���� �� �ؽ�ó ��ǥ ��ġ���� ���÷��� ����Ͽ� ���� �ؽ�ó���� ������ �� ���� ���� ���ø��մϴ�.
        depthValue = depthMapTexture3.Sample(SampleTypeClamp, projectTexCoord).r;
		
		// ���� ���̸� ����մϴ�.
        lightDepthValue = input.lightViewPosition3.z / input.lightViewPosition3.w;
		
		// lightDepthValue���� ���̾�� ���ϴ�.
        lightDepthValue = lightDepthValue - bias;
		
		// ������ �� ���� ���̿� ���� ���̸� ���Ͽ��� �ȼ��� ���� ó������ �������� �����մϴ�.
		// ���� ��ü �տ� ������ �ȼ��� ���߰�, �׷��� ������ ��ü (��Ŭ ���)�� �׸��ڸ� �帮 ��� �������� �ȼ��� �׸��ڷ� �׸��ϴ�.
        if (lightDepthValue < depthValue)
        {
			// �� �ȼ��� ���� ���� ����մϴ�.
            lightIntensity = saturate(dot(input.normal, lightDir));
			
            if (lightIntensity > 0.0f)
            {
				// Ȯ�� ���� �� ������ �翡 ���� ���� Ȯ����� �����մϴ�.
                color += (diffuseColor3 * lightIntensity);
			
				// ���� ���� ������ ä��ϴ�.
                color = saturate(color);
            }
        }
    }
    else
    {
		// �̰��� �׸��� �� ������ ���� �ۿ� �ִٸ� ���� �������� ������ ���������� �׸��ϴ�.
        lightIntensity = saturate(dot(input.normal, lightDir));
        if (lightIntensity > 0.0f)
        {
            color += (diffuseColor3 * lightIntensity);
            color = saturate(color);
        }
    }
	
	// ���� ���� ������ ä��ϴ�.
	color = saturate(color);

	// �� �ؽ�ó ��ǥ ��ġ���� ���÷��� ����Ͽ� �ؽ�ó���� �ȼ� ������ ���ø��մϴ�.
	textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);

	// ���� �ؽ�ó ������ �����մϴ�.
	color = color * textureColor;

    return color;
}