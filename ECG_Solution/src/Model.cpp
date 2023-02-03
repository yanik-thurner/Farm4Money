#include "Model.h"
#define STB_IMAGE_IMPLEMENTATION
//#define STBI_ASSERT(x)
//#define STBI_MALLOC
//#define STBI_REALLOC 
//#define STBI_FREE
#include "stb\stb_image.h"

Model::~Model()
{
	for (auto &i : geometry) {
		//delete(i);
	}
}

std::vector<GeometryData> Model::getMeshes()
{
	return meshes;
}

std::vector<std::shared_ptr<Material>> Model::getMat()
{
	return matvec;
}

void Model::transform(glm::mat4 transformation)
{
	for (auto &i : geometry) {
		i->transform(transformation);
	}
}

void Model::setTransformMatrix(glm::mat4 transform)
{
	for (auto &i : geometry) {
		i->setTransformMatrix(transform);
	}
}

void Model::setMaterial(std::shared_ptr<Material> material) {
	return;
}

void Model::resetModelMatrix()
{
	for (auto &i : geometry) {
		i->resetModelMatrix();
	}
}

void Model::setMatVec(std::vector<std::shared_ptr<Material>> vec)
{
	matlist = true;
	//matvec = vec;
}

void Model::movehead(glm::mat4 trans)
{
	geometry[1]->transform(trans);
	geometry[6]->transform(trans);
	geometry[7]->transform(trans);
}

Model::Model(std::vector<GeometryData> &meshes, std::vector<std::shared_ptr<Material>> matvec, std::shared_ptr<Shader> shader, std::shared_ptr<Shader> shadercolor, bool gamma)
	: gammaCorrection(gamma), shader(shader), shadercolor(shadercolor), meshes(meshes), matvec(matvec)
{
	fillGeom();
}

// draws the model, and thus all its meshes
void Model::draw(glm::vec3 cam_pos, float view_frustum)
{
	for (unsigned int i = 0; i < geometry.size(); i++) {
		glm::vec3 _position =   geometry[i]->_transformMatrix * geometry[i]->_modelMatrix * glm::vec4(meshes[i].positions[0], 1);
		if (view_frustum &&  glm::distance(cam_pos, _position) > view_frustum)
			continue;
		geometry[i]->draw();
	}
}

void Model::fillGeom()
{
	for (unsigned int i = 0; i < meshes.size(); i++) {
		geometry.push_back(new Geometry(glm::mat4(1.0f), meshes[i], matvec[i]));
		//geometry.push_back(std::make_shared<Geometry>(Geometry(glm::mat4(1.0f), i, material)));
		//geometry.push_back(Geometry(glm::mat4(1.0f), i, material));
	}
}

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel(string const &path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));
	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
	fillGeom();
}


// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode *node, const aiScene *scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
		//geometry.push_back(Geometry(glm::mat4(1.0f), x, material));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}

}


GeometryData Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	// data to fill
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture2> textures;

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
						  // positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		/*// tangent
		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		vertex.Tangent = vector;
		// bitangent
		vector.x = mesh->mBitangents[i].x;
		vector.y = mesh->mBitangents[i].y;
		vector.z = mesh->mBitangents[i].z;
		vertex.Bitangent = vector;*/
		vertices.push_back(vertex);
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	std::vector<Texture2> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	//,std::vector<Texture2> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	//textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	//std::vector<Texture2> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	//textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	//std::vector<Texture2> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	//textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

 	if (diffuseMaps.size() > 0)
	{
		std::shared_ptr<NewTexture> text = std::make_shared<NewTexture>(diffuseMaps[0].path);
		std::shared_ptr<Material> mesh_material = std::make_shared<TextureMaterial>(shader, glm::vec3(0.9f, 0.8f, 0.4f), 5.0f, text);
		matvec.push_back(mesh_material);
	}
	else {
		aiColor4D colorRead;
		aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &colorRead);
		glm::vec3 color = glm::vec3(colorRead.r, colorRead.g, colorRead.b);
		std::shared_ptr<Material> mesh_material = std::make_shared<ColorMaterial>(shadercolor, glm::vec3(0.9f, 0.8f, 0.4f), 5.0f, color);
		matvec.push_back(mesh_material);
	}
	matlist = true;

	// return a mesh object created from the extracted mesh data
	return Geometry::createMesh(vertices, indices, textures);
}


// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::vector<Texture2> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture2> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			if (str.length > 2) {
				Texture2 texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory, false);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
	}
	return textures;
}



unsigned int Model::TextureFromFile(const char *path, const string &directory, bool gamma)
{
	string filename = string(path); 
	filename.erase(0,1);
	filename = directory + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

