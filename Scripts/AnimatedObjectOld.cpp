//#include "AnimatedObject.h"
//
//CAnimatedObject::CAnimatedObject()
//{
//	m_NumBones = 0;
//	m_pScene = NULL;
//
//	bool ret = false;
//	std::string modelFilname = "";
//	m_pScene = m_Importer.ReadFile(modelFilname.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
//
//	if (m_pScene)
//	{
//		m_Entries.resize(m_pScene->mNumMeshes);
//		m_Textures.resize(m_pScene->mNumMaterials);
//		unsigned int numVertices = 0;
//		unsigned int numIndices = 0;
//
//		//Get mesh information from scene as well as total number of vertices and indices
//		for (size_t meshIndex = 0; meshIndex < m_Entries.size(); meshIndex++)
//		{
//			//Store mesh data
//			m_Entries[meshIndex].MaterialIndex = m_pScene->mMeshes[meshIndex]->mMaterialIndex;
//			m_Entries[meshIndex].NumIndices = m_pScene->mMeshes[meshIndex]->mNumFaces * 3;
//			m_Entries[meshIndex].BaseVertex = numVertices;
//			m_Entries[meshIndex].BaseIndex = numIndices;
//
//			//Total verticies and indices in scene
//			numVertices += m_pScene->mMeshes[meshIndex]->mNumVertices;
//			numIndices += m_Entries[meshIndex].NumIndices;
//		}
//
//		//Reserve space in vectors for the vertex attributes and indices
//		positions.reserve(numVertices);
//		normals.reserve(numVertices);
//		texcoords.reserve(numVertices);
//		bones.resize(numVertices);
//		indices.reserve(numIndices);
//
//		//Initialize the meshes in the scene
//		for (size_t meshIndex = 0; meshIndex < m_Entries.size(); meshIndex++)
//		{
//			const aiMesh* aiMesh = m_pScene->mMeshes[meshIndex];
//			const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
//
//			//Populate the vertex attribute vectors
//			for (size_t i = 0; i < aiMesh->mNumVertices; i++)
//			{
//				const aiVector3D aiPos = (aiMesh->mVertices[i]);
//				const aiVector3D aiNormal = (aiMesh->mNormals[i]);
//				const aiVector3D aiTexCoord = aiMesh->HasTextureCoords(0) ? (aiMesh->mTextureCoords[0][i]) : aiVector3D(0.0f, 0.0f, 0.0f);
//
//				positions.push_back(glm::vec3(aiPos.x, aiPos.y, aiPos.z));
//				normals.push_back(glm::vec3(aiNormal.x, aiNormal.y, aiNormal.z));
//				texcoords.push_back(glm::vec2(aiTexCoord.x, aiTexCoord.y));
//			}
//
//			//Populate the index buffer
//			for (size_t i = 0; i < aiMesh->mNumFaces; i++)
//			{
//				const aiFace& face = aiMesh->mFaces[i];
//				assert(face.mNumIndices == 3);
//
//				indices.push_back(face.mIndices[0]);
//				indices.push_back(face.mIndices[1]);
//				indices.push_back(face.mIndices[2]);
//			}
//
//			//per mesh load bones data
//			loadPerVertexBoneData(meshIndex, aiMesh, bones);
//
//		}
//
//		setBufferandAttributes();
//
//		return GLCheckError();
//
//
//		// load vertex, index, tex coord, material
//		// bone and vertex data
//
//		InitFromScene(m_pScene, modelFilname);
//	}
//	else
//	{
//		printf("Error parsing '%s': '%s'\n", modelFilname.c_str(), m_Importer.GetErrorString());
//	}
//}
