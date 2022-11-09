/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Terrain.cpp																									*/
/*  DATE: Aug 27th, 2022																								*/
/************************************************************************************************************************/

#include "Terrain.h"
#include <fstream>
#include "Texture.h"

//------------------------------------------------------------------------------------------------------------------------
// Procedure: CTerrain()
//	 Purpose: To generate a terrain from the raw file, _pHeightmapDirectory. _uiNumRows is the height of the image or the number of verticies vertically,
//			  and _uiNumCols is the width of the image or the number of vertices horizontally

CTerrain::CTerrain(const char* const _pHeightmapDirectory, const float _fHeightScaleFactor)
{
	//Load an image
	int iWidth = 0, iHeight = 0, iChannels = 0;
	unsigned char* pData = stbi_load(_pHeightmapDirectory, &iWidth, &iHeight, &iChannels, 1);
	unsigned int uiVertexCount = iWidth * iHeight;
	
	//Get Heightmap from image
	std::vector<float> vHeightmap(uiVertexCount);

	//Assign to vHeightmap
	for (unsigned int i = 0; i < uiVertexCount; i++)
	{
		vHeightmap[i] = (float)pData[i] * _fHeightScaleFactor;
	}

	//Free Image Data
	stbi_image_free(pData);

	//Smooth the Heightmap
	{
		std::vector<float> vDest(vHeightmap.size());

		for (unsigned int i = 0; i < iHeight; i++)
			for (unsigned int j = 0; j < iWidth; j++)
			{
				//Calculate the new vertex height by averaging the surounding pixels in the current height map 
				float fAverage = 0.0f;
				unsigned int uiCount = 0;

				for (unsigned int m = i - 1; m <= i + 1; m++)
					for (unsigned int n = j - 1; n <= j + 1; n++)
						if ((m >= 0 && m < iHeight) && (n >= 0 && n < iWidth))
						{
							//If mn are valid indices, include in average calculation
							fAverage += vHeightmap[(m * iWidth) + n];
							uiCount++;
						}

				vDest[(i * iWidth) + j] = fAverage / (float)uiCount;
			}

		//Replace the old heightmap with the filtered one
		vHeightmap = vDest;
	}

	//Calculate Mesh Vertices
	std::vector<stVertex> vVertices(uiVertexCount);
	for (unsigned int x = 0; x < iWidth; x++)
		for (unsigned int y = 0; y < iHeight; y++)
		{
			unsigned int uiVectorIndex = (y * iWidth) + x;
			vVertices[uiVectorIndex] = stVertex
			{
				glm::vec3(x, vHeightmap[uiVectorIndex], y) - (glm::vec3(iWidth,0.0f,iHeight) / 2.0f),
				glm::vec3(0.0f, 1.0f, 1.0f),
				glm::vec2((float)x / (float)iWidth, (float)y / (float)iHeight)
			};
		}

	//Calculate Mesh Indices
	const unsigned int uiFaceNumber = (iHeight - 1) * (iWidth - 1) * 2;
	std::vector<unsigned int> vIndices(uiFaceNumber * 3);

	unsigned int k = 0;
	for (unsigned int i = 0; i < iHeight - 1; i++)
		for (unsigned int j = 0; j < iWidth - 1; j++)
		{
			vIndices[k] = (i * iWidth) + j;
			vIndices[k + 1] = ((i + 1) * iWidth) + j;
			vIndices[k + 2] = (i * iWidth) + j + 1;

			vIndices[k + 3] = ((i + 1) * iWidth) + j;
			vIndices[k + 4] = ((i + 1) * iWidth) + j + 1;
			vIndices[k + 5] = (i * iWidth) + j + 1;

			k += 6; // next quad
		}

	//Set Mesh Normals----------------------------------------------------------------------
	//Now many normals to average in a vertex
	std::vector<unsigned int> vVertexNormalCount(vVertices.size());
	
	//Loop through every triange in the mesh
	for (unsigned int uiIndex = 0; uiIndex < vIndices.size(); uiIndex += 3)
	{
		//Calculate Face Normal
		glm::vec3 v3FaceNormal = glm::cross
		(
			vVertices[vIndices[uiIndex+1]].v3Position - vVertices[vIndices[uiIndex]].v3Position,
			vVertices[vIndices[uiIndex+2]].v3Position - vVertices[vIndices[uiIndex]].v3Position
		);

		//Set vVertexNormalCount[i] and summed vertex normal
		for (unsigned int i = 0; i < 3; i++)
		{
			vVertices[vIndices[uiIndex+i]].v3Normal += v3FaceNormal;
			vVertexNormalCount[vIndices[uiIndex + i]]++;
		}
	}

	for (unsigned int i = 0; i < vVertices.size(); i++)
	{
		//Average Vertex Normals
		vVertices[i].v3Normal /= (float)vVertexNormalCount[i];
	}

	//Set Mesh Vertices and Indices---------------------------------------------------------
	m_Mesh.SetVerticies(vVertices);
	m_Mesh.SetIndicies(vIndices);
}

void CTerrain::Draw()
{
	//m_Mesh.Draw(GetMainCamera());
}
