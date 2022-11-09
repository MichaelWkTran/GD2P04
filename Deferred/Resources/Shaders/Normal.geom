#version 430 core

layout (points) in;
layout (line_strip, max_vertices = 2) out;

in VS_GS_VERTEX
{
    in vec4 v4Position;
    in vec4 v4Normal;
} gs_in[];

void main()
{
    gl_Position = gs_in[0].v4Position; 
    EmitVertex();

    gl_Position = gs_in[0].v4Position + gs_in[0].v4Normal; 
    EmitVertex();
    
    EndPrimitive();
}
