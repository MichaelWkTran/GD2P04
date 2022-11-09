#version 430 core

layout (triangles, equal_spacing, ccw) in;

uniform mat4 uni_mat4CameraMatrix;
uniform sampler2D uni_samp2DHeightMap;

in vec2 TextureCoord[];

//out sampler2D samp2DHeightMap;
out vec2 uvs;
out vec3 vs_v3Position;
out vec3 vs_v3Normal;

void main(void)
{
	//Calculate Texture Coordinates
	uvs = (gl_TessCoord.x * TextureCoord[0]) + 
		  (gl_TessCoord.y * TextureCoord[1]) + 
		  (gl_TessCoord.z * TextureCoord[2]);

	//Calculate Triangle Coordinate
	gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) + 
				  (gl_TessCoord.y * gl_in[1].gl_Position) + 
				  (gl_TessCoord.z * gl_in[2].gl_Position);

	//Displace Verticies
	float fHeightScale = 50.0f;
	gl_Position += vec4(0.0f, texture(uni_samp2DHeightMap, uvs).y * fHeightScale, 0.0f, 0.0f);

	//-----------------------
	vec3 v3VertexPos[3];
	v3VertexPos[0] = gl_in[0].gl_Position.xyz + (texture(uni_samp2DHeightMap, TextureCoord[0]).y * fHeightScale);
	v3VertexPos[1] = gl_in[1].gl_Position.xyz + (texture(uni_samp2DHeightMap, TextureCoord[1]).y * fHeightScale);
	v3VertexPos[2] = gl_in[2].gl_Position.xyz + (texture(uni_samp2DHeightMap, TextureCoord[2]).y * fHeightScale);

	vs_v3Position = gl_Position.xyz;
	vs_v3Normal = normalize(cross(v3VertexPos[1] - v3VertexPos[0], v3VertexPos[2] - v3VertexPos[0]));
	gl_Position = uni_mat4CameraMatrix * gl_Position;
}