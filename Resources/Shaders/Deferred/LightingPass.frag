/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Diffuse.Frag																									*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#version 430 core

out vec4 fs_colour;
in vec2 vs_v2TextureCoord;

//Texture Uniforms
uniform sampler2D uni_gPosition;
uniform sampler2D uni_gNormal;
uniform sampler2D uni_gAlbedoSpec;
uniform sampler2D uni_gDepth;

//Light Structs
struct stInfinitePointLight
{
	vec3 v3LightPosition;
	vec4 v4LightColour;
	mat4 mat4VPMatrix;
};

struct stPointLight
{
	vec3 v3LightPosition;
	vec4 v4LightColour;
	float fAttenuationExponent;
	float fAttenuationLinear;
	float fAttenuationConstant;
	mat4 mat4VPMatrix;
};

struct stDirectionalLight
{
	vec3 v3LightDirection;
	vec4 v4LightColour;
	mat4 mat4VPMatrix;
	sampler2D samp2DShadowMap;
};

struct stSpotLight
{
	vec3 v3LightPosition;
	vec3 v3LightDirection;
	vec4 v4LightColour;
	float fOuterCone;
	float fInnerCone;
	mat4 mat4VPMatrix;
	sampler2D samp2DShadowMap;
};

//Light Uniforms
uniform vec3 uni_v3CameraPosition;
uniform float uni_ambientStrength = 0.1f;

#define MAX_INF_POINT_LIGHT 10
uniform int uni_iInfPointLightNum = 0;
uniform stInfinitePointLight uni_InfinitePointLight[MAX_INF_POINT_LIGHT];

#define MAX_POINT_LIGHT 10
uniform int uni_iPointLightNum = 0;
uniform stPointLight uni_PointLight[MAX_POINT_LIGHT];

#define MAX_DIRECTIONAL_LIGHT 2
uniform int uni_iDirectionalLightNum = 0;
uniform stDirectionalLight uni_DirectionalLight[MAX_DIRECTIONAL_LIGHT];

#define MAX_SPOT_LIGHT 10
uniform int uni_iSpotLightNum = 0;
uniform stSpotLight uni_SpotLight[MAX_SPOT_LIGHT];

//Global Variables
vec3 g_position;
vec3 g_normal;
vec4 g_albedoSpec;
vec4 g_depth;
float uni_fShininess = 1.0f;

void GetIlluminatedColour(vec3 _lightColour, vec3 _lightDirection, float _intensity)
{
	vec3 ambient = _lightColour * g_albedoSpec.rgb * uni_ambientStrength;
	
	vec3 diffuse = vec3(0);
	if (_intensity > 0) diffuse = _lightColour * _intensity * max(dot(g_normal, _lightDirection), 0.0f);
	
	vec3 specular = vec3(0);
	if (_intensity > 0 && g_albedoSpec.a > 0 && _lightColour != vec3(0))
	{
		vec3 viewDirection = normalize(uni_v3CameraPosition - g_position);
		vec3 halfwayVector = normalize(-_lightDirection + viewDirection);
		float specularAmount = pow(max(dot(g_normal, halfwayVector), 0.0f), uni_fShininess);
		specular = g_albedoSpec.a * _lightColour * specularAmount * _intensity;
	}

	fs_colour += vec4(ambient + diffuse + specular, 0.0f);
}

void InfinitePointLight(stInfinitePointLight _Light)
{
	vec3 v3LightPosition = _Light.v3LightPosition;
	vec4 v4LightColour = _Light.v4LightColour;
	mat4 mat4VPMatrix = _Light.mat4VPMatrix;

	GetIlluminatedColour(v4LightColour.rgb, normalize(v3LightPosition - g_position), v4LightColour.w);
}

void PointLight(stPointLight _Light)
{
	vec3 v3LightPosition = _Light.v3LightPosition;
	vec4 v4LightColour = _Light.v4LightColour;
	float fAttenuationExponent = _Light.fAttenuationExponent;
	float fAttenuationLinear = _Light.fAttenuationLinear;
	float fAttenuationConstant = _Light.fAttenuationConstant;
	mat4 mat4VPMatrix = _Light.mat4VPMatrix;
	
	//Calculate light intensity
	vec3 v3LightVector = v3LightPosition - g_position;
	float fLightDistance = length(v3LightVector);
	float fIntensity = v4LightColour.w / ((fAttenuationExponent * fLightDistance * fLightDistance) + (fAttenuationLinear * fLightDistance) + fAttenuationConstant);
	
	GetIlluminatedColour(v4LightColour.rgb, normalize(v3LightVector), fIntensity);
}

void DirectionalLight(stDirectionalLight _Light)
{
	vec3 v3LightDirection = -normalize(_Light.v3LightDirection);
	vec4 v4LightColour = _Light.v4LightColour;
	mat4 mat4VPMatrix = _Light.mat4VPMatrix;
	
	GetIlluminatedColour(v4LightColour.rgb, v3LightDirection, v4LightColour.w);
}

void SpotLight(stSpotLight _Light)
{
	vec3 v3LightPosition = _Light.v3LightPosition;
	vec3 v3SpotLightDirection = normalize(_Light.v3LightDirection);
	vec4 v4LightColour = _Light.v4LightColour;
	float fOuterCone = _Light.fOuterCone;
	float fInnerCone = _Light.fInnerCone;
	mat4 mat4VPMatrix = _Light.mat4VPMatrix;

	//Calculate light intensity
	vec3 v3LightDirection = normalize(v3LightPosition - g_position);
	float fAngle = dot(v3SpotLightDirection, -v3LightDirection);
	float fIntensity = v4LightColour.w * clamp((fAngle - fOuterCone) / (fInnerCone - fOuterCone), 0.0f, 1.0f);

	GetIlluminatedColour(v4LightColour.rgb, v3LightDirection, fIntensity);
}

void main()
{
	fs_colour = vec4(0.0f,0.0f,0.0f,1.0f);
	g_position = textureLod(uni_gPosition, vs_v2TextureCoord, 0.0f).xyz;
	g_normal = texture(uni_gNormal, vs_v2TextureCoord).xyz;
	g_albedoSpec = texture(uni_gAlbedoSpec, vs_v2TextureCoord);
	g_depth = texture(uni_gDepth, vs_v2TextureCoord);

	//Calculate Diffuse and Specular Colours
	for (int i = 0; i < uni_iInfPointLightNum; i++) InfinitePointLight(uni_InfinitePointLight[i]);
	for (int i = 0; i < uni_iPointLightNum; i++) PointLight(uni_PointLight[i]);
	for (int i = 0; i < uni_iDirectionalLightNum; i++) DirectionalLight(uni_DirectionalLight[i]);
	for (int i = 0; i < uni_iSpotLightNum; i++) SpotLight(uni_SpotLight[i]);
}