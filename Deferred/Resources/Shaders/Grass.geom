#version 430 core

layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_GS_VERTEX
{
    in vec4 v3Position;
    in mat4 mat4CameraMatrix;
} gs_in[];

void main()
{
    gl_Position = gs_in[0].v3Position + (gs_in[0].mat4CameraMatrix * vec4(-2.0f, 0.0f, 0.0f, 0.0f)); EmitVertex();
    gl_Position = gs_in[0].v3Position + (gs_in[0].mat4CameraMatrix * vec4( 2.0f, 0.0f, 0.0f, 0.0f)); EmitVertex();
    gl_Position = gs_in[0].v3Position + (gs_in[0].mat4CameraMatrix * vec4( 0.0f, 2.0f, 0.0f, 0.0f)); EmitVertex();
 
    EndPrimitive();
}
