#include "ssAnimatedModel.h"
#include "MathUtils.h"
#include "Texture.h"
#include "Camera.h"
#include "Main.h"

#define POSITION_LOCATION	  0
#define TEX_COORD_LOCATION	  1
#define NORMAL_LOCATION		  2
#define BONE_ID_LOCATION	  3
#define BONE_WEIGHT_LOCATION  4

ssAnimatedModel::ssAnimatedModel(std::string modelFilname, unsigned int _program)
{
	m_program = _program;
	m_VAO = 0;
	memset(m_buffers, 0, sizeof(m_buffers));
	m_numBones = 0;
	
	//Load the scene from the file
	m_scene = m_importer.ReadFile
	(
		modelFilname.c_str(),
		aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices
	);

	//Check whether the loaded scene is valid
	if (!m_scene)
	{
		std::cout << "ERROR: Failed to load scene from " <<
			modelFilname.c_str() << '\n' <<
			m_importer.GetErrorString() << "\n\n";
		
		assert(0);
	}

	//Initalise from scene
	#pragma region Initalise from scene

	//Load vertex, index, tex coord, material, bone and vertex data
	m_entries.resize(m_scene->mNumMeshes);
	m_textures.resize(m_scene->mNumMaterials);
	unsigned int numVertices = 0;
	unsigned int numIndices = 0;

	//Per mesh get number of materials, indices, base vertex and index
	//Get total number of vertices and indices
	for (size_t i = 0; i < m_entries.size(); i++)
	{
		m_entries[i].MaterialIndex = m_scene->mMeshes[i]->mMaterialIndex;
		m_entries[i].NumIndices = m_scene->mMeshes[i]->mNumFaces * 3;
		m_entries[i].BaseVertex = numVertices;
		m_entries[i].BaseIndex = numIndices;

		numVertices += m_scene->mMeshes[i]->mNumVertices;
		numIndices += m_entries[i].NumIndices;
	}

	//Reserve space in vectors for the vertex attributes and indices
	m_positions.reserve(numVertices);
	m_normals.reserve(numVertices);
	m_texCoords.reserve(numVertices);
	m_bones.resize(numVertices);
	m_indices.reserve(numIndices);

	//Initialize the meshes in the scene one by one
	for (unsigned int meshIndex = 0; meshIndex < m_entries.size(); meshIndex++)
	{
		aiMesh* mesh = m_scene->mMeshes[meshIndex];

		//Populate the vertex attribute vectors
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			const aiVector3D aiPos = (mesh->mVertices[i]);
			const aiVector3D aiNormal = (mesh->mNormals[i]);
			const aiVector3D aiTexCoord = mesh->HasTextureCoords(0) ? (mesh->mTextureCoords[0][i]) : aiVector3D(0.0f, 0.0f, 0.0f);

			m_positions.push_back(glm::vec3(aiPos.x, aiPos.y, aiPos.z));
			m_normals.push_back(glm::vec3(aiNormal.x, aiNormal.y, aiNormal.z));
			m_texCoords.push_back(glm::vec2(aiTexCoord.x, aiTexCoord.y));
		}

		//Populate the index buffer
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			const aiFace& face = mesh->mFaces[i];
			assert(face.mNumIndices == 3);

			m_indices.push_back(face.mIndices[0]);
			m_indices.push_back(face.mIndices[2]);
			m_indices.push_back(face.mIndices[1]);
		}

		//Per mesh load bones data
		loadPerVertexBoneData(meshIndex, mesh, m_bones);
	}

	#pragma region Set Buffer and Attributes

	//Create Vertex Array
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	//Create buffers for vertex attributes
	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_buffers), m_buffers);

	//Generate and populate the buffers with vertex attributes and indices
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_positions[0]) * m_positions.size(), &m_positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_texCoords[0]) * m_texCoords.size(), &m_texCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_normals[0]) * m_normals.size(), &m_normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_bones[0]) * m_bones.size(), &m_bones[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(BONE_ID_LOCATION);
	glVertexAttribIPointer(BONE_ID_LOCATION, NUM_BONES_PER_VERTEX, GL_INT, sizeof(PerVertexBoneData), (const GLvoid*)0);
	glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
	glVertexAttribPointer(BONE_WEIGHT_LOCATION, NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(PerVertexBoneData), (const GLvoid*)(NUM_BONES_PER_VERTEX*sizeof(int)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices[0]) * m_indices.size(), &m_indices[0], GL_STATIC_DRAW);

	//Make sure the VAO is not changed from the outside
	glBindVertexArray(0);

	#pragma endregion

	//Check whether an error has occoured
	assert(glGetError() == GL_NO_ERROR);

	#pragma endregion

	//Load materials/textures
	if (modelFilname.substr(modelFilname.length() - 3, 3).compare("fbx") != 0)
	{
		//Extract the directory part from the file name
		std::string::size_type SlashIndex = modelFilname.find_last_of("/");
		std::string Dir;

		if (SlashIndex == std::string::npos) Dir = ".";
		else if (SlashIndex == 0) Dir = "/";
		else Dir = modelFilname.substr(0, SlashIndex);

		// Initialize the materials
		for (unsigned int i = 0; i < m_scene->mNumMaterials; i++)
		{
			const aiMaterial* material = m_scene->mMaterials[i];
			m_textures[i] = 0;

			if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				aiString aiPath;
				if (material->GetTexture(aiTextureType_DIFFUSE, 0, &aiPath) == AI_SUCCESS)
				{
					std::string path(aiPath.data);
					std::string FullPath = Dir + "/" + path;

					unsigned int TextureID = LoadTexture(FullPath.c_str());
					m_textures[i] = TextureID;

					if (m_textures[i] == 0)  printf("Error loading texture '%s'\n", FullPath.c_str());
					else printf("[Animated Model] %d - loaded texture '%s'\n", i, FullPath.c_str());
				}
			}
		}
	}

	//Load Bone Hierarchy
	const aiAnimation* animations = m_scene->mAnimations[0];
	for (unsigned int channelIndex = 0; channelIndex < animations->mNumChannels; channelIndex++)
	{
		aiNodeAnim* nodeAnim = animations->mChannels[channelIndex];
		std::string nodeName = nodeAnim->mNodeName.data;

		//Store nodeAnim to m_nodeAnims if it does not exist in the map yet
		if (m_nodeAnims.find(nodeName) == m_nodeAnims.end())
			m_nodeAnims[nodeName] = nodeAnim;
	}

	//Animation Defaults
	m_startFrame = 31;
	m_endFrame = 50;
	m_animFps = m_scene->mAnimations[0]->mChannels[0]->mNumPositionKeys / m_scene->mAnimations[0]->mDuration;

	m_animStartTime = m_startFrame / float(m_animFps);
	m_animEndtime = m_endFrame / float(m_animFps);
	m_animationTime = m_animStartTime;
	m_animSpeed = 1.250f;

	m_animTick = (1.0f / float(m_animFps)) * m_animSpeed;
}

ssAnimatedModel::~ssAnimatedModel()
{
	for (unsigned int i = 0; i < m_textures.size(); i++) {
		//SAFE_DELETE(m_Textures[i]);
	}

	if (m_buffers[0] != 0) {
		glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_buffers), m_buffers);
	}

	if (m_VAO != 0) {
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
}

void ssAnimatedModel::loadPerVertexBoneData(int meshIndex, const aiMesh * pMesh, std::vector<PerVertexBoneData>& bones)
{
	// For each bone 
	// In BoneTransformationInfo vector -> gets the offset matrix for each bone
	// In m_BoneNameToIDMapping map -> maps bone name to bone id
	// In VertexBoneData Vector -> stores per vertex which bone affects with what weightage

	for (int i = 0; i < pMesh->mNumBones; i++)
	{ 
		int BoneIndex = 0;
		
		// get bone name for current bone index
		std::string BoneName(pMesh->mBones[i]->mName.data);

		// gets the initial offset matrix for each bone
		// stored in the m_boneinfo vector 
		if (m_boneNameToIDMap.find(BoneName) == m_boneNameToIDMap.end())
		{
			// Allocate an index for a new bone
			BoneIndex = m_numBones; // intitially 0
			m_numBones++;

			// boneInfo stores offset and final transformation for bone
			BoneTransformationInfo bi;
			m_boneTransformInfo.push_back(bi);

			// offset is relative to nodes parent
			// offset is the matrix relation between mesh and bone in bind pose

			// The mOffsetMatrix member is what's commonly referred to as the Inverse Bind Pose matrix
			// Transforming a bind-pose vertex by that matrix, 
			// Moves the vertex to the joint's local coordinate space.
			m_boneTransformInfo[BoneIndex].BoneOffset = glm::AIToGLM(pMesh->mBones[i]->mOffsetMatrix);// is the inverse bind pose matrix
			
			// map bone name to index
			// bonemapping maps bone id to bone name
			m_boneNameToIDMap[BoneName] = BoneIndex;
		}
		else
		{
			BoneIndex = m_boneNameToIDMap[BoneName];
		}

		// Gets the bone index and bone weight of each bone affecting a vertex
		// stored in vertexBoneData (bone) -> stores per vertex bone data
		for (int j = 0; j < pMesh->mBones[i]->mNumWeights; j++)
		{
			int VertexID = m_entries[meshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;

			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			
			// for each vertex 
			// get what bone affects it and with what weightage
			bones[VertexID].addBoneData(BoneIndex, Weight);
		}
	}

}

unsigned int ssAnimatedModel::LoadTexture(std::string texFileName)
{
	unsigned int mtexture;

	//Load texture
	glGenTextures(1, &mtexture);
	glBindTexture(GL_TEXTURE_2D, mtexture);

	//Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//** loadImage and create texture
	// Load image, create texture and generate mipmaps
	int width, height;
	unsigned char* image = stbi_load(texFileName.c_str(), &width, &height, 0, 0);
	//SOIL_load_image(texFileName.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	//SOIL_free_image_data(image);

	printf("\n [loadTexture] fileName %s \n", texFileName.c_str());

	return mtexture;

}

void ssAnimatedModel::Draw()
{
	glUseProgram(m_program);

	glm::mat4 vp = GetMainCamera().GetCameraMatrix();
	GLint vpLoc = glGetUniformLocation(m_program, "vp");
	glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(vp));

	glUniformMatrix4fv
	(
		glGetUniformLocation(m_program, "model"),
		1, GL_FALSE, glm::value_ptr(m_transform.GetModel())
	);

	// get uniform location for transforms
	unsigned int boneLocation[100];
	for (unsigned int i = 0; i < ARRAY_SIZE(boneLocation); i++)
	{
		char name[128];
		memset(name, 0, sizeof(name));
		sprintf_s(name, "jointTransforms[%d]", i);
		boneLocation[i] = glGetUniformLocation(m_program, name);
	}

	std::vector<glm::mat4> transforms;

	boneTransforms(transforms);

	for (int i = 0; i < transforms.size(); i++)
	{
		glUniformMatrix4fv(boneLocation[i], 1, GL_TRUE, glm::value_ptr(transforms[i]));
	}

	glBindVertexArray(m_VAO);

	for (unsigned int i = 0; i < m_entries.size(); i++)
	{
		unsigned int MaterialIndex = m_entries[i].MaterialIndex;

		assert(MaterialIndex < m_textures.size());

		if (m_textures[MaterialIndex])
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_textures[MaterialIndex]);
			glUniform1i(glGetUniformLocation(m_program, "uni_samp2DDiffuse0"), MaterialIndex);
		}

		glDrawElementsBaseVertex
		(
			GL_TRIANGLES,
			m_entries[i].NumIndices,
			GL_UNSIGNED_INT,
			(void*)(sizeof(unsigned int) * m_entries[i].BaseIndex),
			m_entries[i].BaseVertex
		);
	}

	// Make sure the VAO is not changed from the outside    
	glBindVertexArray(0);
}

void ssAnimatedModel::boneTransforms(std::vector<glm::mat4>& transforms)
{
		m_animationTime += m_animTick * e_deltaTime;
		while (m_animationTime >= m_animEndtime) m_animationTime -= (m_animEndtime - m_animStartTime);

		// Go through the bone hierachy and set the final transformation for each bone starting from the root node
		ReadNodeHeirarchy(m_animationTime, m_scene->mRootNode, glm::mat4(1));
		transforms.resize(m_numBones);

		//Set the trasnformation matrix for each bone
		for (unsigned int i = 0; i < m_numBones; i++) transforms[i] = m_boneTransformInfo[i].FinalTransformation;
}

void ssAnimatedModel::setCurrentAnimation(int startFrameNum, int endFramNum){
	
	m_animStartTime = startFrameNum / float(m_animFps);
	m_animEndtime = endFramNum / float(m_animFps);

	m_animationTime = m_animStartTime;
}

//////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////

void ssAnimatedModel::ReadNodeHeirarchy(float _animationTime, const aiNode* _node, const glm::mat4& _parentTransform)
{
	//Initially _node is the root node and parent transform is identity
	std::string nodeName(_node->mName.data);
	const aiAnimation* animation = m_scene->mAnimations[-1];
	
	// scales the object dependending upong unit of 
	// measurement if meters then .0254
	// transformation relative to nodes parent
	glm::mat4 nodeTransformation = glm::AIToGLM(_node->mTransformation);

	//Can initialize with identity but then model is 40 times bigger
	//NodeTransformation.InitIdentity(); 
	
	// If node has animations
	if (const aiNodeAnim* nodeAnim = FindNodeAnim(animation, nodeName))
	{
		// if there is animation then calculate current transform

		// Get position, scale and rotation
		// All are w.r.t parent bone

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, _animationTime, nodeAnim);
		glm::mat4 TranslationM = glm::translate(glm::vec3(Translation.x, Translation.y, Translation.z));
		
		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, _animationTime, nodeAnim);
		glm::mat4 RotationM = glm::toMat4(glm::quat(RotationQ.x, RotationQ.y, RotationQ.z, RotationQ.w));

		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, _animationTime, nodeAnim);
		glm::mat4 ScalingM = glm::scale(glm::vec3(Scaling.x, Scaling.y, Scaling.z));
		
		// Combine the above transformations
		// and store in nodeTransform
		nodeTransformation = TranslationM * RotationM * ScalingM;
	}
	

	glm::mat4 modalAnimParentTransform = _parentTransform * //modalSpace parentBone animTransform
										nodeTransformation; //boneSpace currentbone animTransform

	if (m_boneNameToIDMap.find(nodeName) != m_boneNameToIDMap.end())
	{
		//Get global inverse transform
		glm::mat4 globalInverseTransform = glm::AIToGLM(m_scene->mRootNode->mTransformation);
		globalInverseTransform = glm::inverse(globalInverseTransform);

		// get ID of current bone name to get bone offset matrix for current bone
		unsigned int boneIndex = m_boneNameToIDMap[nodeName];
											 
		//calculate final bone transformation info
		m_boneTransformInfo[boneIndex].FinalTransformation = globalInverseTransform * // sceneSpace transform 
															 modalAnimParentTransform *  // modelspace anim transform for current bone
															 m_boneTransformInfo[boneIndex].BoneOffset;  // Inverse Bind Pose matrix - transforms from bind pose to local
	}

	//Do the same for all the children in the hierarchy by calling itself
	for (unsigned int i = 0; i < _node->mNumChildren; i++) ReadNodeHeirarchy(_animationTime, _node->mChildren[i], modalAnimParentTransform);
}

const aiNodeAnim* ssAnimatedModel::FindNodeAnim(const aiAnimation* _animation, std::string _nodeName)
{
	/*for (unsigned int i = 0; i < _animation->mNumChannels; i++)
	{
		//A channel is a bone with all tranformation information
		const aiNodeAnim* nodeAnim = _animation->mChannels[i];

		if (std::string(nodeAnim->mNodeName.data) == _nodeName)
			return nodeAnim;
	}*/

	//optimized implementation of above code
	if (m_nodeAnims.find(_nodeName) != m_nodeAnims.end())
	{
		const aiNodeAnim* pNodeAnim = m_nodeAnims[_nodeName];
		return pNodeAnim;
	}

	return nullptr;
}

void ssAnimatedModel::CalcInterpolatedPosition(aiVector3D& _out, float _animationTime, const aiNodeAnim* _nodeAnim)
{
	if (_nodeAnim->mNumPositionKeys == 1) { _out = _nodeAnim->mPositionKeys[0].mValue; return; }

	unsigned int positionIndex = FindPosition(_animationTime, _nodeAnim);
	unsigned int nextPositionIndex = (positionIndex + 1);
	assert(nextPositionIndex < _nodeAnim->mNumPositionKeys);

	float deltaTime = (float)(_nodeAnim->mPositionKeys[nextPositionIndex].mTime - _nodeAnim->mPositionKeys[positionIndex].mTime);
	float factor = (_animationTime - (float)_nodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime;

	assert(factor >= 0.0f && factor <= 1.0f);

	const aiVector3D& start = _nodeAnim->mPositionKeys[positionIndex].mValue;
	const aiVector3D& end = _nodeAnim->mPositionKeys[nextPositionIndex].mValue;
	aiVector3D delta = end - start;

	_out = start + factor * delta;
}

unsigned int ssAnimatedModel::FindPosition(float AnimationTime, const aiNodeAnim * pNodeAnim) {

	for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
			return i;
	
	assert(0);
	return 0;
}

void ssAnimatedModel::CalcInterpolatedRotation(aiQuaternion & out, float AnimationTime, const aiNodeAnim * pNodeAnim){

	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	//printf("nodename: %s \n", pNodeAnim->mNodeName.C_Str());

	unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	
	assert(Factor >= 0.0f && Factor <= 1.0f);
	
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	
	aiQuaternion::Interpolate(out, StartRotationQ, EndRotationQ, Factor);
	out = out.Normalize();
}

unsigned int ssAnimatedModel::FindRotation(float AnimationTime, const aiNodeAnim * pNodeAnim) {

	//assert(pNodeAnim->mRotationKeys > 0);

	for (unsigned int i = 0; pNodeAnim->mNumRotationKeys - 1; i++)
	{
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
			return i;
	}

	//assert(0);
	return 0;
}

void ssAnimatedModel::CalcInterpolatedScaling(aiVector3D & out, float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	
	assert(Factor >= 0.0f && Factor <= 1.0f);
	
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	
	aiVector3D Delta = End - Start;
	out = Start + Factor * Delta;	
}

unsigned int ssAnimatedModel::FindScaling(float AnimationTime, const aiNodeAnim * pNodeAnim) {

	assert(pNodeAnim->mNumScalingKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);
	return 0;
}

///////////////// getter and setters //////////////////////

void ssAnimatedModel::setAnimSpeed(float _animSpeed){
	 
	m_animSpeed = _animSpeed;
	m_animTick = 1.0f / float(m_animFps)  * m_animSpeed;
}

