//#pragma once
//#include "GameObject.h"
//#include "assimp/Importer.hpp"
//#include "assimp/scene.h"
//#include "assimp/postprocess.h"
//#include <vector>
//
//#define INVALID_MATERIAL 0xFFFFFFFF
//
//class CAnimatedObject : public CUpdatedObject
//{
//public:
//	CTransform m_transform;
//	CAnimatedObject();
//	
//	#pragma region Structs
//
//	struct MeshEntry
//	{
//		unsigned int NumIndices;
//		unsigned int BaseVertex;
//		unsigned int BaseIndex;
//		unsigned int MaterialIndex;
//
//		MeshEntry()
//		{
//			NumIndices = 0;
//			BaseVertex = 0;
//			BaseIndex = 0;
//			MaterialIndex = INVALID_MATERIAL;
//		}
//	};
//
//	#define NUM_BONES_PER_VERTEX 4
//	struct PerVertexBoneData
//	{
//		int IDs[NUM_BONES_PER_VERTEX];
//		float Weights[NUM_BONES_PER_VERTEX];
//
//		PerVertexBoneData()
//		{
//			reset();
//		}
//
//		void reset()
//		{
//			memset(IDs, 0, sizeof(IDs));
//			memset(Weights, 0, sizeof(Weights));
//		}
//
//		void addBoneData(int BoneID, float Weight)
//		{
//			int IDCount = sizeof(IDs) / sizeof(IDs[0]);
//			for (int i = 0; i < IDCount; i++)
//			{
//				if (Weights[i] != 0.0f) continue;
//
//				IDs[i] = BoneID;
//				Weights[i] = Weight;
//				return;
//			}
//		}
//	};
//
//	struct stAnimatedVertex
//	{
//		glm::vec3 position;
//		glm::vec3 normal;
//		glm::vec2 textureUV;
//		PerVertexBoneData bones;
//
//		static void LinkAttributes()
//		{
//			glEnableVertexAttribArray(0 /*m_v3Position*/);
//			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(stVertex), (void*)(offsetof(stVertex, stVertex::v3Position)));
//
//			glEnableVertexAttribArray(1 /*m_v3Normal*/);
//			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(stVertex), (void*)(offsetof(stVertex, stVertex::v3Normal)));
//
//			glEnableVertexAttribArray(2 /*m_v2TextureCoord*/);
//			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(stVertex), (void*)(offsetof(stVertex, stVertex::v2TextureUV)));
//		}
//	};
//
//	#pragma endregion
//
//private:
//	#pragma region Rendering Variables
//
//	unsigned int m_VAO;
//	unsigned int m_Buffers[NUM_VBs];
//
//	std::vector<MeshEntry> m_Entries;
//	std::vector<unsigned int> m_Textures;
//
//	std::vector<glm::vec3> positions;
//	std::vector<glm::vec3> normals;
//	std::vector<glm::vec2> texcoords;
//	std::vector<PerVertexBoneData> bones;
//	std::vector<unsigned int> indices;
//
//	std::map<std::string, aiNodeAnim*> m_BoneNameToAiNodeAnimMap;
//	std::map<std::string, unsigned int> m_BoneNameToIDMap;
//	std::vector<BoneTransformationInfo> m_BoneTransformInfo;
//	unsigned int m_NumBones;
//
//	glm::mat4 m_GlobalInverseTransform;
//	unsigned int m_boneLocation[100];
//
//	const aiScene* m_pScene;
//	Assimp::Importer m_Importer;
//	std::vector<MeshEntry> m_Entries;
//
//	unsigned int numMaterialsCounter;
//
//	#pragma endregion
//
//	#pragma region Animation Variables
//	
//	float animTick;
//	int startFrame;
//	int endFrame;
//	int currentFrame;
//	float animSpeed;
//
//	int animFps;
//	float animStartTime;
//	float animEndtime;
//
//	float animationTime;
//
//	glm::vec3 scale;
//
//	float currentPlayerSpeed;
//	float currentRotationSpeed;
//	float playerSpeed;
//	float playerRotSpeed;
//
//	#pragma endregion
//};