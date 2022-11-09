/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Diffuse.Frag																									*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#version 430 core

out vec4 fs_v4Colour;

in vec3 vs_v3Position;
in vec3 vs_v3Normal;
in vec2 vs_v2TextureCoord;

//Unique Uniforms---------------------------------------------------------
uniform vec4 uni_v4Colour = vec4(1.0);

//Texture Uniforms
uniform sampler2D uni_samp2DDiffuse0;
uniform sampler2D uni_samp2DSpecular0;
uniform sampler2D uni_samp2DReflect0;

//Light Uniforms
uniform float uni_fSpecularStrength = 0.3f;
uniform float uni_fShininess = 16.0f;

uniform float uni_fRimExponent = 16.0f;
uniform vec4 uni_v4RimColour = vec4(1.0, 1.0, 1.0, 0.0);

uniform float uni_fReflectionStrength = 0;

//Shared Uniforms---------------------------------------------------------
//Texture Uniforms
uniform samplerCube uni_sampCubeSkybox;

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
uniform vec4 uni_v4AmbientColour;

#define MAX_INF_POINT_LIGHT 10
uniform int uni_iInfPointLightNum = 0;
uniform stInfinitePointLight uni_InfinitePointLight[MAX_INF_POINT_LIGHT];

#define MAX_POINT_LIGHT 10
uniform int uni_iPointLightNum = 0;
uniform stPointLight uni_PointLight[MAX_POINT_LIGHT];

#define MAX_DIRECTIONAL_LIGHT 10
uniform int uni_iDirectionalLightNum = 0;
uniform stDirectionalLight uni_DirectionalLight[MAX_DIRECTIONAL_LIGHT];

#define MAX_SPOT_LIGHT 10
uniform int uni_iSpotLightNum = 0;
uniform stSpotLight uni_SpotLight[MAX_SPOT_LIGHT];

//Fog Uniforms
uniform vec3 uni_v3FogColour = vec3(0.0);
uniform float uni_fFogStart = 20.0f;
uniform float uni_fFogRange = 10.0f;

//Global Variables
vec3 g_v3Diffuse = vec3(0.0);
float g_fSpecular = 0.0f;
float g_fShadow = 0.0f;

void UpdateDiffuseSpecular(vec3 _v3LightColour, vec3 _v3LightDirection, float _fIntensity)
{
	//Update Diffuse
	g_v3Diffuse += _v3LightColour * max(dot(vs_v3Normal, _v3LightDirection), 0.0f);

	//Update Specular
	if (_fIntensity <= 0 || uni_fSpecularStrength <= 0) return;

	vec3 v3ViewDirection = normalize(uni_v3CameraPosition - vs_v3Position);
	vec3 v3HalfwayVector = normalize(-_v3LightDirection + v3ViewDirection);
	float fSpecularAmount = pow(max(dot(vs_v3Normal, v3HalfwayVector), 0.0f), uni_fShininess);

	g_fSpecular += fSpecularAmount * uni_fSpecularStrength * _fIntensity;
}

vec3 RimColour()
{
	if (uni_v4RimColour.w <= 0) return vec3(0.0);

	float fRimFactor = 1.0f - dot(vs_v3Normal, uni_v3CameraPosition - vs_v3Position);
	fRimFactor = smoothstep(0.0f, 1.0f, fRimFactor);
	fRimFactor = pow(fRimFactor, uni_fRimExponent);

	return fRimFactor * (uni_v4RimColour.rgb * uni_v4RimColour.a);
}

vec4 MixFogColour(vec4 _v4Colour)
{
	float fCameraDistance = distance(vs_v3Position, uni_v3CameraPosition);

	float fFogContribution = (fCameraDistance - uni_fFogStart)/uni_fFogRange;
	fFogContribution = clamp(fFogContribution, 0.0, 1.0);

	return mix(_v4Colour, vec4(uni_v3FogColour, 1.0f), fFogContribution);
}

void InfinitePointLight(stInfinitePointLight _Light)
{
	vec3 v3LightPosition = _Light.v3LightPosition;
	vec4 v4LightColour = _Light.v4LightColour;
	mat4 mat4VPMatrix = _Light.mat4VPMatrix;

	//Update g_v3Diffuse and g_fSpecular Colours
	UpdateDiffuseSpecular(v4LightColour.rgb, normalize(v3LightPosition - vs_v3Position), v4LightColour.w);
}

void PointLight(stPointLight _Light)
{
	vec3 v3LightPosition = _Light.v3LightPosition;
	vec4 v4LightColour = _Light.v4LightColour;
	float fAttenuationExponent = _Light.fAttenuationExponent;
	float fAttenuationLinear = _Light.fAttenuationLinear;
	float fAttenuationConstant = _Light.fAttenuationConstant;
	mat4 mat4VPMatrix = _Light.mat4VPMatrix;
	
	//---------------------------------------------------------------------------------
	vec3 v3LightVector = v3LightPosition - vs_v3Position;
	float fLightDistance = length(v3LightVector);
	float fIntensity = v4LightColour.w / ((fAttenuationExponent * fLightDistance * fLightDistance) + (fAttenuationLinear * fLightDistance) + fAttenuationConstant);
	
	//Update g_v3Diffuse and g_fSpecular Colours
	UpdateDiffuseSpecular(v4LightColour.rgb, normalize(v3LightVector), fIntensity);
}

void DirectionalLight(stDirectionalLight _Light)
{
	vec3 v3LightDirection = -normalize(_Light.v3LightDirection);
	vec4 v4LightColour = _Light.v4LightColour;
	mat4 mat4VPMatrix = _Light.mat4VPMatrix;

	//Update g_v3Diffuse and g_fSpecular Colours
	UpdateDiffuseSpecular(v4LightColour.rgb, v3LightDirection, v4LightColour.w);

	//Calculate Shadows
	vec4 fragPosLightSpace = mat4VPMatrix * vec4(vs_v3Position, 1.0f);

	// Perspective Divide To get NDC [-1, 1]
	vec3 ndcSpace = fragPosLightSpace.xyz/fragPosLightSpace.w;

	// Sets ndcSpace to cull space
	if(ndcSpace.z <= 1.0f)
	{
		float fShadow = 0.0f;

		// Get from [-1, 1] range to [0, 1] range just like the shadow map
		ndcSpace = (ndcSpace + 1.0f) / 2.0f;
		float currentDepth = ndcSpace.z;
		// Prevents shadow acne
		float bias = max(0.025f * (1.0f - dot(vs_v3Normal, v3LightDirection)), 0.0005f);

		// Smoothens out the shadows
		int sampleRadius = 2;
		vec2 pixelSize = 1.0 / textureSize(_Light.samp2DShadowMap, 0);
		for(int y = -sampleRadius; y <= sampleRadius; y++)
		    for(int x = -sampleRadius; x <= sampleRadius; x++)
		    {
		        float closestDepth = texture(_Light.samp2DShadowMap, ndcSpace.xy + vec2(x, y) * pixelSize).r;
				if (currentDepth > closestDepth + bias) fShadow += 1.0f;     
		    }    
		
		// Get average shadow
		fShadow /= pow((sampleRadius * 2 + 1), 2);
		
		//Combine shadow
		g_fShadow += fShadow;
	}
}

void SpotLight(stSpotLight _Light)
{
	vec3 v3LightPosition = _Light.v3LightPosition;
	vec3 v3SpotLightDirection = normalize(_Light.v3LightDirection);
	vec4 v4LightColour = _Light.v4LightColour;
	float fOuterCone = _Light.fOuterCone;
	float fInnerCone = _Light.fInnerCone;
	mat4 mat4VPMatrix = _Light.mat4VPMatrix;

	//---------------------------------------------------------------------------------
	vec3 v3LightDirection = normalize(v3LightPosition - vs_v3Position);

	float fAngle = dot(v3SpotLightDirection, -v3LightDirection);
	float fIntensity = v4LightColour.w * clamp((fAngle - fOuterCone) / (fInnerCone - fOuterCone), 0.0f, 1.0f);

	//Update g_v3Diffuse and g_fSpecular Colours
	UpdateDiffuseSpecular(v4LightColour.rgb, v3LightDirection, fIntensity);
}

void main()
{
	//Get Texture Colours
	vec4 v4DiffuseTexture = vec4(1.0f);
	if (textureSize(uni_samp2DDiffuse0, 0) != vec2(1,1)) v4DiffuseTexture *= texture(uni_samp2DDiffuse0, vs_v2TextureCoord);
	vec4 v4SpecularTexture = vec4(1.0f);
	if (textureSize(uni_samp2DSpecular0, 0) != vec2(1,1)) v4SpecularTexture = texture(uni_samp2DSpecular0, vs_v2TextureCoord);
	
	//Calculate Diffuse and Specular Colours	
	for (int i = 0; i < uni_iInfPointLightNum; i++) InfinitePointLight(uni_InfinitePointLight[i]);
	for (int i = 0; i < uni_iPointLightNum; i++) PointLight(uni_PointLight[i]);
	for (int i = 0; i < uni_iDirectionalLightNum; i++) DirectionalLight(uni_DirectionalLight[i]);
	for (int i = 0; i < uni_iSpotLightNum; i++) SpotLight(uni_SpotLight[i]);

	//Calculate Reflection
	if (textureSize(uni_sampCubeSkybox, 0) != ivec2(1,1) && uni_fReflectionStrength > 0)
	{
		vec3 v3ReflectDirection = reflect(normalize(vs_v3Position - uni_v3CameraPosition), vs_v3Normal);
		vec4 v4ReflectTexture = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		if (textureSize(uni_samp2DSpecular0, 0) != vec2(1,1)) v4ReflectTexture = texture(uni_samp2DReflect0, vs_v2TextureCoord);
		
		//Mix with v4DiffuseTexture
		v4DiffuseTexture = mix
		(
			v4DiffuseTexture, 
			texture(uni_sampCubeSkybox, v3ReflectDirection),
			uni_fReflectionStrength * v4ReflectTexture.r
		);
	}

	//Calculate Final Fragment Colour
	fs_v4Colour = uni_v4Colour * vec4(v4DiffuseTexture.rgb * g_v3Diffuse, 1.0f);
	fs_v4Colour += vec4(v4SpecularTexture.rgb * g_fSpecular, 1.0f);
	fs_v4Colour += vec4(RimColour(), 1.0f);
	fs_v4Colour *= max(1.0f - g_fShadow, 0.0f);
	fs_v4Colour += uni_v4AmbientColour.rgb * uni_v4AmbientColour.w;
	fs_v4Colour = vec4(MixFogColour(fs_v4Colour).rgb, 1.0f);
}