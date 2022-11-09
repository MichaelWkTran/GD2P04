#include "ssAnimatedModel.h"
#include "Camera.h"
#include "Main.h"
#include "Texture.h"

#define POSITION_LOCATION	  0
#define TEX_COORD_LOCATION	  1
#define NORMAL_LOCATION		  2
#define BONE_ID_LOCATION	  3
#define BONE_WEIGHT_LOCATION  4 

//void ssAnimatedModel::VertexBoneData::addBoneData(int BoneID, float Weight) {
//
//	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
//		if (Weights[i] == 0.0) {
//			IDs[i] = BoneID;
//			Weights[i] = Weight;
//			return;
//		}
//	}
//
//	//assert(0); //more bones than we have space for
//}

ssAnimatedModel::ssAnimatedModel(std::string modelFilname, /*Camera *_camera, */unsigned int _program/*, Light * _light*/){

	//camera = _camera;
	//light = _light;

	program = _program;


	//this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
	//this->position = glm::vec3(0.0, 0.0, 0.0);
	//this->rotation = glm::vec3(1.0f, 0.0f, 0.0f);

	m_VAO = 0;
	ZERO_MEM(m_Buffers);
	m_NumBones = 0;
	m_pScene = NULL;


	//loadMesh(modelFilname);

	Clear();

	bool ret = false;

	m_pScene = m_Importer.ReadFile(modelFilname.c_str(), ASSIMP_LOAD_FLAGS);

	if (m_pScene) {

		// load vertex, index, tex coord, material
		// bone and vertex data

		InitFromScene(m_pScene, modelFilname);
	}
	else {
		printf("Error parsing '%s': '%s'\n", modelFilname.c_str(), m_Importer.GetErrorString());
	}


	std::string fileExt = modelFilname.substr(modelFilname.length() - 3, 3);

	printf("+++++++++++++ model file extension: %s +++++++++++++++++++", fileExt.c_str());


	if (fileExt.compare("fbx") != 0) {

		if (!initMaterials(m_pScene, modelFilname)) {
			//return false;
			printf("!!!!!!!!!! UNABLE TO LOAD MATERIAL !!!!!!!!!!!!!!!!!!! \n");
		}

	}


	loadBoneHierarchy();


	//animation defaults
	startFrame = 31;
	endFrame = 50;
	animFps = m_pScene->mAnimations[0]->mChannels[0]->mNumPositionKeys / m_pScene->mAnimations[0]->mDuration;

	animStartTime = startFrame / float(animFps);
	animEndtime = endFrame / float(animFps);
	animationTime = animStartTime;
	animSpeed = 0.250f;

	animTick = 1.0f / float(animFps)  * animSpeed;

}


ssAnimatedModel::~ssAnimatedModel(){

	Clear();
}

bool ssAnimatedModel::loadMesh(std::string fileName){
	
	//Clear();


	//return ret;

	return 0;
}

void ssAnimatedModel::loadBoneHierarchy() {

	// load animation data
	printf(" ----------- bone hierarchy start-------- \n");

	const aiAnimation* pAnimation = m_pScene->mAnimations[0];

	printf("AnimationName : %s \n", m_pScene->mAnimations[0]->mName.C_Str());

	for (size_t i = 0; i < pAnimation->mNumChannels; i++) {

		aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
		std::string nodeName = pNodeAnim->mNodeName.data;

		printf("ChannelName : %s \n", nodeName.c_str());

		// boneAiNodeMapping map -> maps nodeAnim to nodeName (same as bone name)
		// nodeAnim has rot, pos and scale info
		// it is with respect to parent node

		if (m_BoneNameToAiNodeAnimMap.find(nodeName) == m_BoneNameToAiNodeAnimMap.end()) {
			m_BoneNameToAiNodeAnimMap[nodeName] = pNodeAnim;
		}
	}

	printf(" ----------- bone hierarchy end -------- \n");



}

bool ssAnimatedModel::InitFromScene(const aiScene* pScene, std::string Filename){
	
	m_Entries.resize(pScene->mNumMeshes);
	m_Textures.resize(pScene->mNumMaterials);

	unsigned int numVertices = 0;
	unsigned int numIndices = 0;

	// Per mesh get number of materials, indices, base vertex and index
	// Also get total number of vertices and indices
	for (size_t i = 0; i < m_Entries.size(); i++) {
	
		m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_Entries[i].BaseVertex = numVertices;
		m_Entries[i].BaseIndex = numIndices;

		numVertices += pScene->mMeshes[i]->mNumVertices;
		numIndices += m_Entries[i].NumIndices;
	
		printf("----- numVertices: %d --------- \n", pScene->mMeshes[i]->mNumVertices);
	}

	//reserve space in vectors for the vertex attributes and indices
	positions.reserve(numVertices);
	normals.reserve(numVertices);
	texcoords.reserve(numVertices);
	bones.resize(numVertices);
	indices.reserve(numIndices);

	// initialize the meshes in the scene one by one
	for (size_t i = 0; i < m_Entries.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		initMesh(i, paiMesh, positions, normals, texcoords, bones, indices);
	}

	setBufferandAttributes();

	return GLCheckError();
}

void ssAnimatedModel::initMesh(unsigned int meshIndex,
								const aiMesh * paiMesh,
								std::vector<glm::vec3>& positions,
								std::vector<glm::vec3>& normals,
								std::vector<glm::vec2>& texcoords,
								std::vector<PerVertexBoneData>& bones,
								std::vector<unsigned int>& indices){


	printf("---------- aiMeshname: %s ---------- \n", paiMesh->mName.C_Str());

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	//populate the vertex attribute vectors
	for (size_t i = 0; i < paiMesh->mNumVertices; i++) {
	
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormals = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
		normals.push_back(glm::vec3(pNormals->x, pNormals->y, pNormals->z));
		texcoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
	}

	// populate the index buffer
	for (size_t i = 0; i < paiMesh->mNumFaces; i++) {
	
		const aiFace& face = paiMesh->mFaces[i];
		assert(face.mNumIndices == 3);

		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}


	//per mesh load bones data
	loadPerVertexBoneData(meshIndex, paiMesh, bones);
}

void ssAnimatedModel::loadPerVertexBoneData(int meshIndex,
								const aiMesh * pMesh,
								std::vector<PerVertexBoneData>& bones){
									
	// For each bone 
	// In BoneTransformationInfo vector -> gets the offset matrix for each bone
	// In m_BoneNameToIDMapping map -> maps bone name to bone id
	// In VertexBoneData Vector -> stores per vertex which bone affects with what weightage

	for (int i = 0; i < pMesh->mNumBones; i++) { // for each bone
		
		int BoneIndex = 0;
		
		// get bone name for current bone index
		std::string BoneName(pMesh->mBones[i]->mName.data);

		//gets the initial offset matrix for each bone
		// stored in the m_boneinfo vector 
		if (m_BoneNameToIDMap.find(BoneName) == m_BoneNameToIDMap.end()) {
			// Allocate an index for a new bone
			BoneIndex = m_NumBones; // intitially 0
			m_NumBones++;

			// boneInfo stores offset and final transformation for bone
			BoneTransformationInfo bi;
			m_BoneTransformInfo.push_back(bi);

			// offset is relative to nodes parent
			// offset is the matrix relation between mesh and bone in bind pose

			// The mOffsetMatrix member is what's commonly referred to as the Inverse Bind Pose matrix
			// Transforming a bind-pose vertex by that matrix, 
			// Moves the vertex to the joint's local coordinate space.
			m_BoneTransformInfo[BoneIndex].BoneOffset = glm::mat4_cast(pMesh->mBones[i]->mOffsetMatrix);// is the inverse bind pose matrix
			
			// map bone name to index
			// bonemapping maps bone id to bone name
			m_BoneNameToIDMap[BoneName] = BoneIndex;
		}
		else {
			BoneIndex = m_BoneNameToIDMap[BoneName];
		}

		
		// Gets the bone index and bone weight of each bone affecting a vertex
		// stored in vertexBoneData (bone) -> stores per vertex bone data
		for (int j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {

			int VertexID = m_Entries[meshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;

			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			
			// for each vertex 
			// get what bone affects it and with what weightage
			bones[VertexID].addBoneData(BoneIndex, Weight);
		}

	}// for each bone

}

bool ssAnimatedModel::initMaterials(const aiScene * pScene, const std::string filename)
{
	// Extract the directory part from the file name
	std::string::size_type SlashIndex = filename.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = filename.substr(0, SlashIndex);
	}

	bool Ret = true;

	// Initialize the materials
	for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		m_Textures[i] = NULL;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string p(Path.data);

				if (p.substr(0, 2) == ".//") {
					p = p.substr(2, p.size() - 2);
				}

				std::string FullPath = Dir + "/" + p;

				unsigned int TextureID = loadTexture(FullPath.c_str());
					m_Textures[i] = TextureID;

				if (m_Textures[i] == 0) {
					printf("Error loading texture '%s'\n", FullPath.c_str());
				}else {
					printf("[Animated Model] %d - loaded texture '%s'\n", i, FullPath.c_str());
				}
				
			}
		}
	}

	return Ret;
}

void ssAnimatedModel::Draw()
{
	render(e_deltaTime);
}

void ssAnimatedModel::setShaderEffectVariables(float dt)
{
	glUseProgram(this->program);
	glUniformMatrix4fv(glGetUniformLocation(program, "vp"), 1, GL_FALSE, glm::value_ptr(GetMainCamera().GetCameraMatrix()));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(m_transform.GetModel()));

	// get uniform location for transforms
	for (unsigned int i = 0; i < ARRAY_SIZE(m_boneLocation); i++) {
		char name[128];
		memset(name, 0, sizeof(name));
		sprintf_s(name, "jointTransforms[%d]", i);
		m_boneLocation[i] = glGetUniformLocation(program, name);
	}

	std::vector<glm::mat4> transforms; // = getJointTransforms();

	boneTransforms(dt, transforms);

	for (int i = 0; i < transforms.size(); i++)
	{
		glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, glm::value_ptr(transforms[i]));
	}
}


void ssAnimatedModel::render(float dt/*, Terrain* terrain*/){
	
	setShaderEffectVariables(dt/*, terrain*/);

	glBindVertexArray(m_VAO);

	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		
		unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

		assert(MaterialIndex < m_Textures.size());

		if (m_Textures[MaterialIndex]) {
			//m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, m_Textures[MaterialIndex]);
						
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_Textures[MaterialIndex]);
			//glUniform1i(glGetUniformLocation(program, "Texture"), MaterialIndex);
		}

		glDrawElementsBaseVertex(GL_TRIANGLES,
								m_Entries[i].NumIndices, 
								GL_UNSIGNED_INT,
								(void*)(sizeof(unsigned int) * m_Entries[i].BaseIndex),
								m_Entries[i].BaseVertex);

		//glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 0);
	}

	// Make sure the VAO is not changed from the outside    
	glBindVertexArray(0);

}




void ssAnimatedModel::boneTransforms(float timeInSeconds, std::vector<glm::mat4>& transforms){
			

		//float animDuration = (float)m_pScene->mAnimations[0]->mDuration;
		//float TicksPerSecond = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
		//float TimeInTicks = (timeInSeconds) * TicksPerSecond;
		//float AnimationTime = fmod(TimeInTicks, animDuration);

		animationTime += animTick;

		if (animationTime >= animEndtime) {
			animationTime = animStartTime;
		}

		// Go through the bone hierachy
		// And set the final transformation for each bone
		// Starting from the root node
		ReadNodeHeirarchy(animationTime, m_pScene->mRootNode, glm::mat4(1));

		transforms.resize(m_NumBones);

		// set the trasnformation matrix for each bone
		for (unsigned int i = 0; i < m_NumBones; i++) {
			transforms[i] = m_BoneTransformInfo[i].FinalTransformation.GLMCast();
		}

}

void ssAnimatedModel::setCurrentAnimation(int startFrameNum, int endFramNum){
	
	animStartTime = startFrameNum / float(animFps);
	animEndtime = endFramNum / float(animFps);

	animationTime = animStartTime;
}

//////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////


void printMatrix(Matrix4f matrix) {

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			printf("%f ", (matrix.m[x][y]));
		}
		printf("\n");
	}

}

void ssAnimatedModel::ReadNodeHeirarchy(float AnimationTime, const aiNode * pNode, glm::mat4 ParentTransform) {

	// initially pNode is the root node
	// and parent transform is identity

	std::string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = m_pScene->mAnimations[-1];
	
	// scales the object dependending upong unit of 
	// measurement if meters then .0254
	// transformation relative to nodes parent
	glm::mat4 NodeTransformation = glm::mat4_cast(pNode->mTransformation); 

	//Can initialize with identity but then model is 40 times bigger
	//NodeTransformation.InitIdentity(); 
	
	// returns aiNodeAnim, for a bone name 
	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	// If node has animations
	if (pNodeAnim)
	{
		// if there is animation then calculate current transform

		// Get position, scale and rotation
		// All are w.r.t parent bone

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		glm::mat4 TranslationM = glm::translate(glm::vec3(Translation.x, Translation.y, Translation.z));

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		glm::mat4 RotationM = glm::toMat4(glm::quat(RotationQ.w, RotationQ.x, RotationQ.y, RotationQ.z));

		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		glm::mat4 ScalingM = glm::scale(glm::vec3(Scaling.x, Scaling.y, Scaling.z));

		// Combine the above transformations
		// and store in nodeTransform
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}
	

	glm::mat4 modalAnimParentTransform = ParentTransform * //modalSpace parentBone animTransform
										NodeTransformation; //boneSpace currentbone animTransform

	if (m_BoneNameToIDMap.find(NodeName) != m_BoneNameToIDMap.end()) {
		
		// Global transform
		glm::mat4 globalInverseTransform = glm::inverse(glm::mat4_cast(m_pScene->mRootNode->mTransformation));
		
		//m_GlobalInverseTransform = m_pScene->mRootNode->mTransformation; // scene space


		

		// inverse Global transform
		// inverse of the root transform -- .025
		//m_GlobalInverseTransform.Inverse();  //scene inverse space


		//printMatrix(m_GlobalInverseTransform.Inverse());

		// get ID of current bone name to get bone offset matrix for current bone
		size_t BoneIndex = m_BoneNameToIDMap[NodeName];
											 
		//calculate final bone transformation info
		m_BoneTransformInfo[BoneIndex].FinalTransformation = Matrix4f(globalInverseTransform) * // sceneSpace transform 
															 modalAnimParentTransform *  // modelspace anim transform for current bone
															 m_BoneTransformInfo[BoneIndex].BoneOffset;  // Inverse Bind Pose matrix - transforms from bind pose to local
		
		//printMatrix(m_BoneTransformInfo[BoneIndex].BoneOffset);

		//printf("hello bind matrix");
	}


	// Do the same for all the children in the hierarchy
	// By calling itself

	for (size_t i = 0; i < pNode->mNumChildren; i++) {

		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], modalAnimParentTransform);

	}
}


const aiNodeAnim * ssAnimatedModel::FindNodeAnim(const aiAnimation* pAnimation, std::string NodeName) {

	/*
	for (size_t i = 0; i < pAnimation->mNumChannels; i++) {

		// a channel is a bone with all tranformation information
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}
	*/

	//optimized implementation of above code
	if (m_BoneNameToAiNodeAnimMap.find(NodeName) != m_BoneNameToAiNodeAnimMap.end()) {
		const aiNodeAnim* pNodeAnim = m_BoneNameToAiNodeAnimMap[NodeName];
		return pNodeAnim;
	}

	return NULL;
}

void ssAnimatedModel::CalcInterpolatedPosition(aiVector3D & out, float AnimationTime, const aiNodeAnim * pNodeAnim){
	
	if (pNodeAnim->mNumPositionKeys == 1) {
		out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	size_t positionIndex = FindPosition(AnimationTime, pNodeAnim);
	size_t NextPositionIndex = (positionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);

	float deltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[positionIndex].mTime);
	float factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime;

	assert(factor >= 0.0f && factor <= 1.0f);

	const aiVector3D& start = pNodeAnim->mPositionKeys[positionIndex].mValue;
	const aiVector3D& end = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D delta = end - start;

	out = start + factor * delta;

}

unsigned int ssAnimatedModel::FindPosition(float AnimationTime, const aiNodeAnim * pNodeAnim) {

	for (size_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

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

	size_t RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	size_t NextRotationIndex = (RotationIndex + 1);
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

	for (size_t i = 0; pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);
	return 0;
}

void ssAnimatedModel::CalcInterpolatedScaling(aiVector3D & out, float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	{
		if (pNodeAnim->mNumScalingKeys == 1) {
			out = pNodeAnim->mScalingKeys[0].mValue;
			return;
		}

		size_t ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
		size_t NextScalingIndex = (ScalingIndex + 1);
		assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
		
		float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
		
		assert(Factor >= 0.0f && Factor <= 1.0f);
		
		const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
		const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
		
		aiVector3D Delta = End - Start;
		out = Start + Factor * Delta;

	}
}


unsigned int ssAnimatedModel::FindScaling(float AnimationTime, const aiNodeAnim * pNodeAnim) {

	assert(pNodeAnim->mNumScalingKeys > 0);

	for (size_t i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);
	return 0;
}


void ssAnimatedModel::Clear()
{
	for (size_t i = 0; i < m_Textures.size(); i++) {
		//SAFE_DELETE(m_Textures[i]);
	}

	if (m_Buffers[0] != 0) {
		glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
	}

	if (m_VAO != 0) {
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
}

void ssAnimatedModel::setBufferandAttributes()
{
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	//create buffers for vertex attributes
	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	//generate and populate the buffers with vertex attributes and indices
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords[0]) * texcoords.size(), &texcoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bones[0]) * bones.size(), &bones[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(BONE_ID_LOCATION);
	glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(PerVertexBoneData), (const GLvoid*)0);

	glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
	glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(PerVertexBoneData), (const GLvoid*)16);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// Make sure the VAO is not changed from the outside
	glBindVertexArray(0);
}

unsigned int ssAnimatedModel::loadTexture(std::string texFileName)
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);

	printf("\n [loadTexture] fileName %s \n", texFileName.c_str());

	return mtexture;
}

void ssAnimatedModel::setAnimSpeed(float _animSpeed)
{
	this->animSpeed = _animSpeed;
	animTick = 1.0f / float(animFps)  * animSpeed;
}

