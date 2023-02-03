#include "AnimModel.h"

#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4


void AnimModel::VertexBoneData::AddBoneData(GLuint BoneID, float Weight)
{
	for (GLuint i = 0; i < NUM_BONES_PER_VEREX; i++) {
		if (Weights[i] == 0.0) {
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}

	// should never get here - more bones than we have space for
	assert(0);
}

AnimModel::AnimModel(std::shared_ptr<Shader> shader) : shader(shader)
{
	m_VAO = 0; 
	memset(m_Buffers, 0, sizeof(m_Buffers));
	m_NumBones = 0;
	m_pScene = NULL;
	m_withAdjacencies = false;
}


AnimModel::~AnimModel()
{
	Clear();
}


void AnimModel::Clear()
{
	//for (uint i = 0; i < m_Textures.size(); i++) {
		//SAFE_DELETE(m_Textures[i]);
	//}

	if (m_Buffers[0] != 0) {
		glDeleteBuffers(NUM_VBs, m_Buffers);
	}

	if (m_VAO != 0) {
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
}


bool AnimModel::LoadMesh(const string& Filename, bool WithAdjacencies)
{
	m_withAdjacencies = WithAdjacencies;

	// Release the previously loaded mesh (if it exists)
	Clear();

	// Create the VAO
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	// Create the buffers for the vertices attributes
	glGenBuffers(NUM_VBs, m_Buffers);

	bool Ret = false;

	m_pScene = m_Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_FlipUVs |
		aiProcess_JoinIdenticalVertices);
	if (m_pScene) {
		m_GlobalInverseTransform = aiMatrix4x4ToGlm(&m_pScene->mRootNode->mTransformation);
		m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
		Ret = InitFromScene(m_pScene, Filename);
	}
	else {
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
	}

	// Make sure the VAO is not changed from the outside
	glBindVertexArray(0);

	return Ret;
}


bool AnimModel::InitFromScene(const aiScene* pScene, const string& Filename)
{
	m_Entries.resize(pScene->mNumMeshes);
	m_Textures.resize(pScene->mNumMaterials);

	vector<glm::vec3> Positions;
	vector<glm::vec3> Normals;
	vector<glm::vec2> TexCoords;
	vector<VertexBoneData> Bones;
	vector<GLuint> Indices;

	GLuint NumVertices = 0;
	GLuint NumIndices = 0;

	GLuint VerticesPerPrim = m_withAdjacencies ? 6 : 3;

	// Count the number of vertices and indices
	for (GLuint i = 0; i < m_Entries.size(); i++) {
		m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * VerticesPerPrim;
		m_Entries[i].BaseVertex = NumVertices;
		m_Entries[i].BaseIndex = NumIndices;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += m_Entries[i].NumIndices;
	}

	// Reserve space in the vectors for the vertex attributes and indices
	Positions.reserve(NumVertices);
	Normals.reserve(NumVertices);
	TexCoords.reserve(NumVertices);
	Bones.resize(NumVertices);
	Indices.reserve(NumIndices);

	// Initialize the meshes in the scene one by one
	for (GLuint i = 0; i < m_Entries.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh, Positions, Normals, TexCoords, Bones, Indices);
	}

	if (!InitMaterials(pScene, Filename)) {
		return false;
	}

	// Generate and populate the buffers with vertex attributes and the indices
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(BONE_ID_LOCATION);
	glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
	glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
	glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	return GLCheckError();
}


static GLuint GetOppositeIndex(const aiFace& Face, const Edge& e)
{
	for (GLuint i = 0; i < 3; i++) {
		GLuint Index = Face.mIndices[i];

		if (Index != e.a && Index != e.b) {
			return Index;
		}
	}

	assert(0);

	return 0;
}


void AnimModel::FindAdjacencies(const aiMesh* paiMesh, vector<unsigned int>& Indices)
{
	// Step 1 - find the two triangles that share every edge
	for (GLuint i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& face = paiMesh->mFaces[i];

		Face Unique;

		// If a position vector is duplicated in the VB we fetch the 
		// index of the first occurrence.
		for (GLuint j = 0; j < 3; j++) {
			GLuint Index = face.mIndices[j];
			aiVector3D& v = paiMesh->mVertices[Index];

			if (m_posMap.find(v) == m_posMap.end()) {
				m_posMap[v] = Index;
			}
			else {
				Index = m_posMap[v];
			}

			Unique.Indices[j] = Index;
		}

		m_uniqueFaces.push_back(Unique);

		Edge e1(Unique.Indices[0], Unique.Indices[1]);
		Edge e2(Unique.Indices[1], Unique.Indices[2]);
		Edge e3(Unique.Indices[2], Unique.Indices[0]);

		m_indexMap[e1].AddNeigbor(i);
		m_indexMap[e2].AddNeigbor(i);
		m_indexMap[e3].AddNeigbor(i);
	}

	// Step 2 - build the index buffer with the adjacency info
	for (GLuint i = 0; i < paiMesh->mNumFaces; i++) {
		const Face& face = m_uniqueFaces[i];

		for (GLuint j = 0; j < 3; j++) {
			Edge e(face.Indices[j], face.Indices[(j + 1) % 3]);
			assert(m_indexMap.find(e) != m_indexMap.end());
			Neighbors n = m_indexMap[e];
			GLuint OtherTri = n.GetOther(i);

			assert(OtherTri != -1);

			const Face& OtherFace = m_uniqueFaces[OtherTri];
			GLuint OppositeIndex = OtherFace.GetOppositeIndex(e);

			Indices.push_back(face.Indices[j]);
			Indices.push_back(OppositeIndex);
		}
	}
}


void AnimModel::InitMesh(GLuint MeshIndex,
	const aiMesh* paiMesh,
	vector<glm::vec3> &Positions,
	vector<glm::vec3> &Normals,
	vector<glm::vec2> &TexCoords,
	vector<VertexBoneData>& Bones,
	vector<GLuint>& Indices)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	// Populate the vertex attribute vectors
	for (GLuint i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		Positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
		Normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
		TexCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
	}

	LoadBones(MeshIndex, paiMesh, Bones);

	if (m_withAdjacencies) {
		FindAdjacencies(paiMesh, Indices);
	}
	else {
		// Populate the index buffer
		for (GLuint i = 0; i < paiMesh->mNumFaces; i++) {
			const aiFace& Face = paiMesh->mFaces[i];
			assert(Face.mNumIndices == 3);
			Indices.push_back(Face.mIndices[0]);
			Indices.push_back(Face.mIndices[1]);
			Indices.push_back(Face.mIndices[2]);
		}
	}
}


void AnimModel::LoadBones(GLuint MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones)
{
	for (GLuint i = 0; i < pMesh->mNumBones; i++) {
		GLuint BoneIndex = 0;
		string BoneName(pMesh->mBones[i]->mName.data);

		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
			// Allocate an index for a new bone
			BoneIndex = m_NumBones;
			m_NumBones++;
			BoneInfo bi;
			m_BoneInfo.push_back(bi);
			m_BoneInfo[BoneIndex].BoneOffset = aiMatrix4x4ToGlm(&pMesh->mBones[i]->mOffsetMatrix);
			m_BoneMapping[BoneName] = BoneIndex;
		}
		else {
			BoneIndex = m_BoneMapping[BoneName];
		}

		for (GLuint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
			GLuint VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			Bones[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}


bool AnimModel::InitMaterials(const aiScene* pScene, const string& Filename)
{
	// Extract the directory part from the file name
	string::size_type SlashIndex = Filename.find_last_of("/");
	string Dir;

	if (SlashIndex == string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = Filename.substr(0, SlashIndex);
	}

	bool Ret = true;

	// Initialize the materials
	for (GLuint i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		m_Textures[i] = NULL;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				string p(Path.data);

				if (p.substr(0, 2) == ".\\") {
					p = p.substr(2, p.size() - 2);
				}

				string FullPath = Dir + "/" + p;
				
				FullPath.erase(0,8);
				m_Textures[i] = std::make_shared<NewTexture>(FullPath.c_str());

				/*
				if (!m_Textures[i]->Load()) {
					printf("Error loading texture '%s'\n", FullPath.c_str());
					delete m_Textures[i];
					m_Textures[i] = NULL;
					Ret = false;
				}
				else {
					printf("%d - loaded texture '%s'\n", i, FullPath.c_str());
				}*/
			}
		}
	}

	return Ret;
}


void AnimModel::Render()
{
	shader->use();
	glBindVertexArray(m_VAO);

	GLuint Topology = m_withAdjacencies ? GL_TRIANGLES_ADJACENCY : GL_TRIANGLES;

	for (GLuint i = 0; i < m_Entries.size(); i++) {
		const GLuint MaterialIndex = m_Entries[i].MaterialIndex;

		assert(MaterialIndex < m_Textures.size());

		if (m_Textures[MaterialIndex]) {
			m_Textures[MaterialIndex]->bind(0);
		}
		shader->setUniform("diffuseTexture",0);

		glDrawElementsBaseVertex(Topology,
			m_Entries[i].NumIndices,
			GL_UNSIGNED_INT,
			(void*)(sizeof(GLuint) * m_Entries[i].BaseIndex),
			m_Entries[i].BaseVertex);
	}

	// Make sure the VAO is not changed from the outside    
	glBindVertexArray(0);
}


GLuint AnimModel::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (GLuint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


GLuint AnimModel::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (GLuint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


GLuint AnimModel::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (GLuint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


void AnimModel::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	GLuint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	GLuint NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void AnimModel::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	GLuint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	GLuint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}


void AnimModel::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	GLuint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	GLuint NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void AnimModel::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform)
{
	string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = m_pScene->mAnimations[0];

	glm::mat4 NodeTransformation(aiMatrix4x4ToGlm(&pNode->mTransformation));

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		glm::mat4 ScalingM = glm::scale(glm::mat4(1), glm::vec3(Scaling.x, Scaling.y, Scaling.z));
		

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		glm::mat4 RotationM = aiMatrix4x4ToGlm(&aiMatrix4x4(RotationQ.GetMatrix()));

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		glm::mat4 TranslationM = glm::translate(glm::mat4(1), glm::vec3(Translation.x, Translation.y, Translation.z));
		//TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);
		

		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM * ScalingM;
		//NodeTransformation = glm::translate(NodeTransformation, glm::vec3(Translation.x, Translation.y, Translation.z));
		//NodeTransformation = glm::rotate(NodeTransformation,)
	}

	glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
		GLuint BoneIndex = m_BoneMapping[NodeName];
		m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
	}

	for (GLuint i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}


void AnimModel::BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms)
{
	glm::mat4 Identity = glm::mat4(1.0);
	//Identity.InitIdentity();

	float TicksPerSecond = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)m_pScene->mAnimations[0]->mDuration);

	ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);

	Transforms.resize(m_NumBones);

	for (GLuint i = 0; i < m_NumBones; i++) {
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
	}
}


const aiNodeAnim* AnimModel::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
{
	for (GLuint i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}

void AnimModel::setuniforms(PlayerCamera& camera, DirectionalLight& dirL, PointLight& pointL)
{
	shader->use();
	shader->setUniform("gWVP", camera.getViewProjectionMatrix());
	shader->setUniform("camera_world", camera.getPosition());
	//shader->setUniform("gWorld", glm::mat4(1));

	shader->setUniform("gWorld", tmatrix);
	shader->setUniform("normx", glm::mat3(glm::transpose(glm::inverse(tmatrix))));

	shader->setUniform("dirL.color", dirL.color);
	shader->setUniform("dirL.direction", dirL.direction);
	shader->setUniform("dirL.amb", dirL.amb);

	shader->setUniform("pointL.color", pointL.color);
	shader->setUniform("pointL.position", pointL.position);
	shader->setUniform("pointL.attenuation", pointL.attenuation);

}

void AnimModel::setmatrix(glm::mat4 matrix)
{
	tmatrix = matrix;
}

//https://stackoverflow.com/questions/29184311/how-to-rotate-a-skinned-models-bones-in-c-using-assimp
glm::mat4 AnimModel::aiMatrix4x4ToGlm(const aiMatrix4x4* from)
{
	glm::mat4 to;


	to[0][0] = (GLfloat)from->a1; to[0][1] = (GLfloat)from->b1;  to[0][2] = (GLfloat)from->c1; to[0][3] = (GLfloat)from->d1;
	to[1][0] = (GLfloat)from->a2; to[1][1] = (GLfloat)from->b2;  to[1][2] = (GLfloat)from->c2; to[1][3] = (GLfloat)from->d2;
	to[2][0] = (GLfloat)from->a3; to[2][1] = (GLfloat)from->b3;  to[2][2] = (GLfloat)from->c3; to[2][3] = (GLfloat)from->d3;
	to[3][0] = (GLfloat)from->a4; to[3][1] = (GLfloat)from->b4;  to[3][2] = (GLfloat)from->c4; to[3][3] = (GLfloat)from->d4;

	return to;
}

void AnimModel::SetBoneTransform(GLuint Index, const glm::mat4& Transform)
{
	shader->use();
	std::cout << glm::to_string(Transform) << std::endl;

	char Name[128];
	memset(Name, 0, sizeof(Name));
	snprintf(Name, sizeof(Name), "gBones[%d]", Index);

	//assert(Index < MAX_BONES);
	//Transform.Print();
	//glUniformMatrix4fv(m_boneLocation[Index], 1, GL_TRUE, (const GLfloat*)Transform);
	shader->setUniform(Name, Transform);
}