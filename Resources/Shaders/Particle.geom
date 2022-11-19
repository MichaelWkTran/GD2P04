#version 430 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in float vs_life[];

out vec2 gs_textureCoordinate;
out float gs_life;

uniform vec3 uni_cameraUp, uni_cameraRight;
uniform float uni_scale = 0.2f;
uniform mat4 uni_cameraMatrix;

void main()
{
    gs_life = vs_life[0];
    float scale = uni_scale;
    //float scale = uni_scale * gs_life;

    //Top Left Vertex
    gl_Position.xyz = gl_in[0].gl_Position.xyz + ((-uni_cameraUp+uni_cameraRight) * scale);
    gl_Position = uni_cameraMatrix * vec4(gl_Position.xyz, 1.0f);
    gs_textureCoordinate = vec2(0.0f, 0.0f);
    EmitVertex();

    //Top Right Vertex
    gl_Position.xyz = gl_in[0].gl_Position.xyz + (( uni_cameraUp+uni_cameraRight) * scale);
    gl_Position = uni_cameraMatrix * vec4(gl_Position.xyz, 1.0f);
    gs_textureCoordinate = vec2(1.0f, 0.0f);
    EmitVertex();

    //Bottom Left Vertex
    gl_Position.xyz = gl_in[0].gl_Position.xyz + ((-uni_cameraUp-uni_cameraRight) * scale);
    gl_Position = uni_cameraMatrix * vec4(gl_Position.xyz, 1.0f);
    gs_textureCoordinate = vec2(0.0f, 1.0f);
    EmitVertex();

    //Bottom Right Vertex
    gl_Position.xyz = gl_in[0].gl_Position.xyz + (( uni_cameraUp-uni_cameraRight) * scale);
    gl_Position = uni_cameraMatrix * vec4(gl_Position.xyz, 1.0f);
    gs_textureCoordinate = vec2(1.0f, 1.0f);
    EmitVertex();

    EndPrimitive();
}
