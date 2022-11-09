#pragma once

#include "glew/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "Transform.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <fstream>
#include "UpdatedObject.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }
#define INVALID_MATERIAL 0xFFFFFFFF

struct MeshEntry
{
	unsigned int NumIndices;
	unsigned int BaseVertex;
	unsigned int BaseIndex;
	unsigned int MaterialIndex;

	MeshEntry()
	{
		NumIndices = 0U;
		BaseVertex = 0U;
		BaseIndex = 0U;
		MaterialIndex = INVALID_MATERIAL;
	}
};

enum VB_TYPES
{
	INDEX_BUFFER,
	POS_VB,
	NORMAL_VB,
	TEXCOORD_VB,
	BONE_VB,
	NUM_VBs
};

class ssAnimatedModel : public CUpdatedObject
{
public:
	ssAnimatedModel(std::string modelFilname, unsigned int _program);
	~ssAnimatedModel();

	#define NUM_BONES_PER_VERTEX 4

	#pragma region Structs
	
	//stores offset matrix and final transforms matrix for each bone
	struct BoneTransformationInfo
	{
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransformation;

		BoneTransformationInfo()
		{
			BoneOffset = glm::mat4(0);
			FinalTransformation = glm::mat4(0);
		}
	};

	//ids and bones affecting each vertex
	struct PerVertexBoneData
	{
		int IDs[NUM_BONES_PER_VERTEX];
		float Weights[NUM_BONES_PER_VERTEX];

		PerVertexBoneData()
		{
			reset();
		}

		void reset()
		{
			memset(IDs, 0, sizeof(IDs));
			memset(Weights, 0, sizeof(Weights));
		}

		void addBoneData(int BoneID, float Weight)
		{
			for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++)
			{
				if (Weights[i] != 0.0) continue;
				
				IDs[i] = BoneID;
				Weights[i] = Weight;
				return;
			}
		}
	};

	#pragma endregion

	virtual void Draw() override;
	void boneTransforms(std::vector<glm::mat4> &transforms);
	void setCurrentAnimation(int startFrameNum, int endFramNum);
	void setAnimSpeed(float _animSpeed);
	
	CTransform m_transform;

private:
	unsigned int m_program;
	
	#pragma region Rendering Variables

	unsigned int m_VAO;
	unsigned int m_buffers[NUM_VBs];
	
	std::vector<MeshEntry> m_entries;
	std::vector<unsigned int> m_textures;

	std::vector<glm::vec3> m_positions;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec2> m_texCoords;
	std::vector<PerVertexBoneData> m_bones;
	std::vector<unsigned int> m_indices;

	std::map<std::string, aiNodeAnim*> m_nodeAnims;
	std::map<std::string, unsigned int> m_boneNameToIDMap;
	std::vector<BoneTransformationInfo> m_boneTransformInfo;
	unsigned int m_numBones;

	const aiScene* m_scene;
	Assimp::Importer m_importer;
	
	#pragma endregion

	#pragma region Animation Variables

	//animation variables
	float m_animTick;
	int m_startFrame;
	int m_endFrame;
	int m_currentFrame;
	float m_animSpeed;

	int m_animFps;
	float m_animStartTime;
	float m_animEndtime;
	
	float m_animationTime;

	float m_currentPlayerSpeed;
	float m_currentRotationSpeed;

	#pragma endregion

	void loadPerVertexBoneData(int _meshIndex, const aiMesh* _mesh, std::vector<PerVertexBoneData>& _bones);
	unsigned int LoadTexture(std::string texFileName);

	// Helper functions 
	// Data conversion and nodefinder 
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

	void CalcInterpolatedPosition(aiVector3D& out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedScaling(aiVector3D& out, float AnimationTime, const aiNodeAnim* pNodeAnim);

	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, std::string NodeName);

	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);
};