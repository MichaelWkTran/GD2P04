/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: TinyObjectLoader.cpp																							*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#include "TinyObjectLoader.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "../Include/TinyObjLoader/tiny_obj_loader.h"
#include "../Include/glm/glm.hpp"
#include <vector>
#include <string>
#include <iostream>

//------------------------------------------------------------------------------------------------------------------------
// Procedure: C3DTextureObject()
//	 Purpose: Assign a passed in mesh, _Mesh, data loaded from an OBJ file from the dirctory, _pOBJModelDirectory.

void GetObjModelData(CMesh<stVertex>& _Mesh, const char* _pOBJModelDirectory)
{
	std::vector<stVertex> vVertices;
	tinyobj::attrib_t Attrib; //Used to get the attributes like pos, normal, etc.
	std::vector<tinyobj::shape_t> vShapes; // Get all the shapes.
	std::vector<tinyobj::material_t> vMaterials; // Get any materials if any.
	std::string Warn, Err; //String for warnings and errors. 

	if (!tinyobj::LoadObj(&Attrib, &vShapes, &vMaterials, &Warn, &Err, _pOBJModelDirectory))
	{
		throw std::runtime_error(Warn + Err);
	}

	//Loop over shapes
	for (const auto& Shape : vShapes)
	{
		// access to vertex info per mesh indicies
		for (const auto& Index : Shape.mesh.indices)
		{
			stVertex Vertex{};

			Vertex.v3Position =
			{
				Attrib.vertices[3 * Index.vertex_index + 0],
				Attrib.vertices[3 * Index.vertex_index + 1],
				Attrib.vertices[3 * Index.vertex_index + 2]
			};

			Vertex.v3Normal =
			{
				Attrib.normals[3 * Index.normal_index + 0],
				Attrib.normals[3 * Index.normal_index + 1],
				Attrib.normals[3 * Index.normal_index + 2]
			};

			Vertex.v2TextureUV =
			{
				Attrib.texcoords[2 * Index.texcoord_index + 0],
				Attrib.texcoords[2 * Index.texcoord_index + 1]
			};

			vVertices.push_back(Vertex);
		}
	}

	//Set Vertices to Mesh
	_Mesh.SetVerticies(vVertices);
}