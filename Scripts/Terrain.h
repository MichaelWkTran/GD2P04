/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Terrain.h																										*/
/*  DATE: Aug 27th, 2022																								*/
/************************************************************************************************************************/

#pragma once
#include "GameObject.h"
#include "Mesh.h"

class CTerrain : public GameObject
{
public:
	CTerrain(const char* const _pHeightmapDirectory, const float _fHeightScaleFactor = 0.35f);
	template<typename T>
	CTerrain(const T* _pTextureData, const unsigned int _uiWidth, const unsigned int _uiHeight, const float _fHeightScaleFactor = 0.35f)
	{
		unsigned int uiVertexCount = _uiHeight * _uiWidth;

		//Smooth the Heightmap
		std::vector<float> vHeightmap(uiVertexCount);

		for (unsigned int i = 0; i < _uiHeight; i++)
			for (unsigned int j = 0; j < _uiWidth; j++)
			{
				//Calculate the new vertex height by averaging the surounding pixels in the current height map 
				float fAverage = 0.0f;
				unsigned int uiCount = 0;

				for (unsigned int m = i - 1; m <= i + 1; m++)
					for (unsigned int n = j - 1; n <= j + 1; n++)
						if ((m >= 0 && m < _uiHeight) && (n >= 0 && n < _uiWidth))
						{
							//If mn are valid indices, include in average calculation
							fAverage += (float)_pTextureData[(m * _uiWidth) + n];
							uiCount++;
						}

				vHeightmap[(i * _uiWidth) + j] = fAverage / (float)uiCount;
			}

		//Calculate Mesh Vertices
		std::vector<stVertex> vVertices(uiVertexCount);
		for (unsigned int x = 0; x < _uiWidth; x++)
			for (unsigned int y = 0; y < _uiHeight; y++)
			{
				unsigned int uiVectorIndex = (y * _uiWidth) + x;
				vVertices[uiVectorIndex] = stVertex
				{
					glm::vec3(x, (float)_pTextureData[uiVectorIndex] * _fHeightScaleFactor, y) - (glm::vec3(_uiWidth,0.0f,_uiHeight) / 2.0f),
					glm::vec3(0.0f, 1.0f, 1.0f),
					glm::vec2((float)x / (float)_uiWidth, (float)y / (float)_uiHeight)
				};
			}

		//Calculate Mesh Indices
		const unsigned int uiFaceNumber = (_uiHeight - 1) * (_uiWidth - 1) * 2;
		std::vector<unsigned int> vIndices(uiFaceNumber * 3);

		unsigned int k = 0;
		for (unsigned int i = 0; i < _uiHeight - 1; i++)
			for (unsigned int j = 0; j < _uiWidth - 1; j++)
			{
				vIndices[k] = (i * _uiWidth) + j;
				vIndices[k + 1] = ((i + 1) * _uiWidth) + j;
				vIndices[k + 2] = (i * _uiWidth) + j + 1;

				vIndices[k + 3] = ((i + 1) * _uiWidth) + j;
				vIndices[k + 4] = ((i + 1) * _uiWidth) + j + 1;
				vIndices[k + 5] = (i * _uiWidth) + j + 1;

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
				vVertices[vIndices[uiIndex + 1]].v3Position - vVertices[vIndices[uiIndex]].v3Position,
				vVertices[vIndices[uiIndex + 2]].v3Position - vVertices[vIndices[uiIndex]].v3Position
			);

			//Set vVertexNormalCount[i] and summed vertex normal
			for (unsigned int i = 0; i < 3; i++)
			{
				vVertices[vIndices[uiIndex + i]].v3Normal += v3FaceNormal;
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

	void Draw() override;
};