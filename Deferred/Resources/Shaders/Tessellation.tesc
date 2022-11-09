#version 430 core
layout (vertices = 3) out;
in vec2 TexCoord[];
out vec2 TextureCoord[];
uniform vec3 uni_v3CameraPosition;

void main()
{
	int MIN_TESS_LEVEL = 1;
    int MAX_TESS_LEVEL = 32;
    float MIN_DISTANCE = 2.0f;
    float MAX_DISTANCE = 80.0f;

	float fDistances[3];
	for (int i = 0; i < 3; i++)
	{
		fDistances[i] = distance(gl_in[i].gl_Position.xyz, uni_v3CameraPosition);
		fDistances[i] = clamp((fDistances[i]-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
	}

	float fTessLevels[3];
	fTessLevels[0] = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(fDistances[2], fDistances[0]));
	fTessLevels[1] = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(fDistances[0], fDistances[1]));
	fTessLevels[2] = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(fDistances[1], fDistances[2]));

	gl_TessLevelOuter[0] = fTessLevels[0];
	gl_TessLevelOuter[1] = fTessLevels[1];
	gl_TessLevelOuter[2] = fTessLevels[2];
	gl_TessLevelInner[0] = max(max(gl_TessLevelOuter[0], gl_TessLevelOuter[1]), gl_TessLevelOuter[2]);

	TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}