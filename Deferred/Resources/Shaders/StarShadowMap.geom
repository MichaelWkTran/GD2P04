#version 430 core

layout (points) in;
layout (triangle_strip, max_vertices = 20) out;

in vec4 vs_v4Position[];
out vec2 v2TextureCoordinate;

struct GeometryVertex
{
    vec3 v3Position;
    vec2 v2TextureCoordinate;
};

uniform mat4 uni_mat4LightProjection;

void main()
{
    float fStarScale = 1.0f/3.0f;
    GeometryVertex GeometryVerticies[11];
    
    //Set vertex positions
    GeometryVerticies[0].v3Position  = vec3( 0.0f, 8.0f, 0.0f) * fStarScale;
    GeometryVerticies[1].v3Position  = vec3( 2.5f, 3.0f, 0.0f) * fStarScale;
    GeometryVerticies[2].v3Position  = vec3( 7.5f, 2.5f, 0.0f) * fStarScale;
    GeometryVerticies[3].v3Position  = vec3( 4.0f,-1.0f, 0.0f) * fStarScale;
    GeometryVerticies[4].v3Position  = vec3( 5.5f,-6.0f, 0.0f) * fStarScale;
    GeometryVerticies[5].v3Position  = vec3( 0.0f,-3.5f, 0.0f) * fStarScale;
    GeometryVerticies[6].v3Position  = vec3(-5.5f,-6.0f, 0.0f) * fStarScale;
    GeometryVerticies[7].v3Position  = vec3(-4.0f,-1.0f, 0.0f) * fStarScale;
    GeometryVerticies[8].v3Position  = vec3(-7.5f, 2.5f, 0.0f) * fStarScale;
    GeometryVerticies[9].v3Position  = vec3(-2.5f, 3.0f, 0.0f) * fStarScale;
    GeometryVerticies[10].v3Position = vec3( 0.0f, 0.0f, 0.0f) * fStarScale;
    
    //Set texture coordinates
    GeometryVerticies[0].v2TextureCoordinate  = vec2( 0.5f,   1.0f);
    GeometryVerticies[1].v2TextureCoordinate  = vec2( 0.667f, 0.643f);
    GeometryVerticies[2].v2TextureCoordinate  = vec2( 1.0f,   0.607f);
    GeometryVerticies[3].v2TextureCoordinate  = vec2( 0.768f, 0.357f);
    GeometryVerticies[4].v2TextureCoordinate  = vec2( 0.868f, 0.0f);
    GeometryVerticies[5].v2TextureCoordinate  = vec2( 0.5f,   0.178f);
    GeometryVerticies[6].v2TextureCoordinate  = vec2( 0.132f, 0.0f);
    GeometryVerticies[7].v2TextureCoordinate  = vec2( 0.232f, 0.357f);
    GeometryVerticies[8].v2TextureCoordinate  = vec2( 0.0f,   0.607f);
    GeometryVerticies[9].v2TextureCoordinate  = vec2( 0.333f, 0.643f);
    GeometryVerticies[10].v2TextureCoordinate = vec2( 0.5f,   0.429f);

    //Create Triangle Segment
    for (int iSegmentIndex = 0; iSegmentIndex < 5; iSegmentIndex++)
    {
        int iVertexIndex = iSegmentIndex * 2;
        gl_Position = uni_mat4LightProjection * vec4(GeometryVerticies[iVertexIndex].v3Position, 0.0f);
        gl_Position += vs_v4Position[0];
        v2TextureCoordinate = GeometryVerticies[iVertexIndex].v2TextureCoordinate;
        EmitVertex();

        if (iSegmentIndex == 0) { iVertexIndex = 9; }
        else { iVertexIndex = (iSegmentIndex * 2) - 1; }
        gl_Position = uni_mat4LightProjection * vec4(GeometryVerticies[iVertexIndex].v3Position, 0.0f);
        gl_Position += vs_v4Position[0];
        v2TextureCoordinate = GeometryVerticies[iVertexIndex].v2TextureCoordinate;
        EmitVertex();

        iVertexIndex = (iSegmentIndex * 2) + 1;
        gl_Position = uni_mat4LightProjection * vec4(GeometryVerticies[iVertexIndex].v3Position, 0.0f);
        gl_Position += vs_v4Position[0];
        v2TextureCoordinate = GeometryVerticies[iVertexIndex].v2TextureCoordinate;
        EmitVertex();

        iVertexIndex = 10;
        gl_Position = uni_mat4LightProjection * vec4(GeometryVerticies[iVertexIndex].v3Position, 0.0f);
        gl_Position += vs_v4Position[0];
        v2TextureCoordinate = GeometryVerticies[iVertexIndex].v2TextureCoordinate;
        EmitVertex();

        EndPrimitive();
    }
}