#pragma once

#include <map>
#include <vector>
#include "Utils.h"
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "NewTexture.h"
#include <memory>
#include "Shader.h"
#include "PlayerCamera.h"
#include "Light.h"
#include <stdio.h>
#include <glm/gtx/string_cast.hpp>
//#include <Importer.hpp>      // C++ importer interface
//#include <scene.h>       // Output data structure
//#include <postprocess.h> // Post processing flags

//#include "ogldev_util.h"
//#include "ogldev_math_3d.h"
//#include "ogldev_texture.h"

using namespace std;

#define GLCheckError() (glGetError() == GL_NO_ERROR)

// Stores an edge by its vertices and force an order between them
struct Edge
{
	Edge(GLuint _a, GLuint _b)
	{
		assert(_a != _b);

		if (_a < _b)
		{
			a = _a;
			b = _b;
		}
		else
		{
			a = _b;
			b = _a;
		}
	}

	void Print()
	{
		printf("Edge %d %d\n", a, b);
	}

	GLuint a;
	GLuint b;
};

struct Neighbors
{
	GLuint n1;
	GLuint n2;

	Neighbors()
	{
		n1 = n2 = (GLuint)-1;
	}

	void AddNeigbor(GLuint n)
	{
		if (n1 == -1) {
			n1 = n;
		}
		else if (n2 == -1) {
			n2 = n;
		}
		else {
			assert(0);
		}
	}

	GLuint GetOther(GLuint me) const
	{
		if (n1 == me) {
			return n2;
		}
		else if (n2 == me) {
			return n1;
		}
		else {
			assert(0);
		}

		return 0;
	}
};

struct CompareEdges
{
	bool operator()(const Edge& Edge1, const Edge& Edge2) const
	{
		if (Edge1.a < Edge2.a) {
			return true;
		}
		else if (Edge1.a == Edge2.a) {
			return (Edge1.b < Edge2.b);
		}
		else {
			return false;
		}
	}
};


struct CompareVectors
{
	bool operator()(const aiVector3D& a, const aiVector3D& b) const
	{
		if (a.x < b.x) {
			return true;
		}
		else if (a.x == b.x) {
			if (a.y < b.y) {
				return true;
			}
			else if (a.y == b.y) {
				if (a.z < b.z) {
					return true;
				}
			}
		}

		return false;
	}
};


struct Face
{
	GLuint Indices[3];

	GLuint GetOppositeIndex(const Edge& e) const
	{
		for (GLuint i = 0; i < 3; i++) {
			GLuint Index = Indices[i];

			if (Index != e.a && Index != e.b) {
				return Index;
			}
		}

		assert(0);

		return 0;
	}
};


class AnimModel
{
public:
	AnimModel(std::shared_ptr<Shader> shader);

	~AnimModel();

	bool LoadMesh(const string& Filename, bool WithAdjacencies);

	void Render();

	GLuint NumBones() const
	{
		return m_NumBones;
	}

	void BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms);
	void SetBoneTransform(GLuint Index, const glm::mat4& Transform);
	void setuniforms(PlayerCamera& camera, DirectionalLight& dirL, PointLight& pointL);
	void setmatrix(glm::mat4 matrix);
private:
#define NUM_BONES_PER_VEREX 4

	struct BoneInfo
	{
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransformation;

		BoneInfo()
		{
			BoneOffset = glm::mat4(0);
			FinalTransformation = glm::mat4(0);
		}
	};

	struct VertexBoneData
	{
		GLuint IDs[NUM_BONES_PER_VEREX];
		float Weights[NUM_BONES_PER_VEREX];

		VertexBoneData()
		{
			Reset();
		};

		void Reset()
		{
			memset(IDs, 0, sizeof(IDs));
			memset(Weights, 0, sizeof(Weights));
		}

		void AddBoneData(GLuint BoneID, float Weight);
	};

	std::shared_ptr<Shader> shader;
	glm::mat4 tmatrix;

	glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	GLuint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	GLuint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	GLuint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);
	bool InitFromScene(const aiScene* pScene, const string& Filename);
	void FindAdjacencies(const aiMesh* paiMesh, vector<unsigned int>& Indices);
	void InitMesh(GLuint MeshIndex,
		const aiMesh* paiMesh,
		vector<glm::vec3> &Positions,
		vector<glm::vec3> &Normals,
		vector<glm::vec2> &TexCoords,
		vector<VertexBoneData>& Bones,
		vector<unsigned int>& Indices);
	void LoadBones(GLuint MeshIndex, const aiMesh* paiMesh, vector<VertexBoneData>& Bones);
	bool InitMaterials(const aiScene* pScene, const string& Filename);
	void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

	enum VB_TYPES {
		INDEX_BUFFER,
		POS_VB,
		NORMAL_VB,
		TEXCOORD_VB,
		BONE_VB,
		NUM_VBs
	};

	GLuint m_VAO;
	GLuint m_Buffers[NUM_VBs];

	struct MeshEntry {
		MeshEntry()
		{
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = INVALID_MATERIAL;
		}

		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int MaterialIndex;
	};

	vector<MeshEntry> m_Entries;
	vector<std::shared_ptr<NewTexture>> m_Textures;

	map<string, GLuint> m_BoneMapping; // maps a bone name to its index
	GLuint m_NumBones;
	vector<BoneInfo> m_BoneInfo;
	glm::mat4 m_GlobalInverseTransform;

	std::map<Edge, Neighbors, CompareEdges> m_indexMap;
	std::map<aiVector3D, GLuint, CompareVectors> m_posMap;
	std::vector<Face> m_uniqueFaces;
	bool m_withAdjacencies;

	const aiScene* m_pScene;
	Assimp::Importer m_Importer;
};
