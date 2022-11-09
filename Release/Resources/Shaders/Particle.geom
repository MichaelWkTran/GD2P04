#version 430 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec4 vs_v4Position[];
out vec2 v2TextureCoordinate;

uniform vec3 uni_cameraUp, uni_cameraRight;
uniform float uni_scale = 0.2f;
uniform mat4 uni_mat4CameraMatrix;

void main()
{
    //Top Left Vertex
    gl_Position.xyz = gl_in[0].gl_Position.xyz + ((-uni_cameraUp+uni_cameraRight) * uni_scale);
    gl_Position = uni_mat4CameraMatrix * vec4(gl_Position.xyz, 1.0f);
    v2TextureCoordinate = vec2(0.0f, 0.0f);
    EmitVertex();

    //Top Right Vertex
    gl_Position.xyz = gl_in[0].gl_Position.xyz + (( uni_cameraUp+uni_cameraRight) * uni_scale);
    gl_Position = uni_mat4CameraMatrix * vec4(gl_Position.xyz, 1.0f);
    v2TextureCoordinate = vec2(1.0f, 0.0f);
    EmitVertex();

    //Bottom Left Vertex
    gl_Position.xyz = gl_in[0].gl_Position.xyz + ((-uni_cameraUp-uni_cameraRight) * uni_scale);
    gl_Position = uni_mat4CameraMatrix * vec4(gl_Position.xyz, 1.0f);
    v2TextureCoordinate = vec2(0.0f, 1.0f);
    EmitVertex();

    //Bottom Right Vertex
    gl_Position.xyz = gl_in[0].gl_Position.xyz + (( uni_cameraUp-uni_cameraRight) * uni_scale);
    gl_Position = uni_mat4CameraMatrix * vec4(gl_Position.xyz, 1.0f);
    v2TextureCoordinate = vec2(1.0f, 1.0f);
    EmitVertex();

    EndPrimitive();
}
