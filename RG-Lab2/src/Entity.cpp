#include "Entity.h"

#include <fstream>

#include "assimp/postprocess.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/gtc/type_ptr.hpp>

Entity::~Entity()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Entity::Load(const std::string& filename, float newScale, glm::u8vec4 color,
				  const std::string& normalMap)
{
	Assimp::Importer importer;
	const aiScene* scene =
		importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
										aiProcess_JoinIdenticalVertices);
	ProcessNode(scene, scene->mRootNode, m_Vertices, m_Indices);

	float xMin = 100000, xMax = -100000;
	float yMin = 100000, yMax = -100000;
	float zMin = 100000, zMax = -100000;
	for (const auto& vertex : m_Vertices)
	{
		xMin = std::min(xMin, vertex.pos[0]);
		xMax = std::max(xMax, vertex.pos[0]);
		yMin = std::min(yMin, vertex.pos[1]);
		yMax = std::max(yMax, vertex.pos[1]);
		zMin = std::min(zMin, vertex.pos[2]);
		zMax = std::max(zMax, vertex.pos[2]);
	}

	float centerX = (xMax + xMin) / 2;
	float centerY = (yMax + yMin) / 2;
	float centerZ = (zMax + zMin) / 2;
	glm::vec3 translate{-centerX, -centerY, -centerZ};
	float scaleX = xMax - xMin;
	float scaleY = yMax - yMin;
	float scaleZ = zMax - zMin;
	float scale = std::max(scaleX, std::max(scaleY, scaleZ));
	scale /= 2;

	for (auto& vertex : m_Vertices)
		vertex.pos = (vertex.pos + translate) / scale * newScale;

	if (m_Texture == 0)
	{
		int texWidth = 1, texHeight = 1;
		auto* pixels = reinterpret_cast<stbi_uc*>(&color);
		CreateTexture(m_Texture, pixels, texWidth, texHeight);
	}

	int texWidth, texHeight, texChannels;
	stbi_uc* pixels =
		stbi_load(normalMap.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	if (pixels)
	{
		CreateTexture(m_NormalTexture, pixels, texWidth, texHeight);
		stbi_image_free(pixels);
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(m_Vertices[0]), m_Vertices.data(),
				 GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(m_Indices[0]), m_Indices.data(),
				 GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Entity::Render()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	if (m_NormalTexture)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_NormalTexture);
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, 0);
}

void Entity::ProcessNode(const aiScene* scene, aiNode* node, std::vector<Vertex>& vertices,
						 std::vector<uint32_t>& indices)
{
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(scene, mesh, vertices, indices);
	}
	for (size_t i = 0; i < node->mNumChildren; i++)
		ProcessNode(scene, node->mChildren[i], vertices, indices);
}

static inline std::string GetFileName(const std::string& path)
{
	size_t lastDelimiter = -1;
	int i = 0;
	for (char c : path)
	{
		if (c == '\\' || c == '/') { lastDelimiter = i; }
		i++;
	}
	return path.substr(lastDelimiter + 1);
}

void Entity::ProcessMesh(const aiScene* scene, const aiMesh* mesh, std::vector<Vertex>& vertices,
						 std::vector<uint32_t>& indices)
{
	uint32_t meshSizeBefore = static_cast<uint32_t>(vertices.size());
	size_t newIndicesCount = 0;
	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		if (face.mNumIndices != 3) { continue; }
		for (size_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j] + meshSizeBefore);
			newIndicesCount++;
		}
	}
	if (newIndicesCount == 0) { return; }

	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex{};
		vertex.pos = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
		vertex.norm = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
		if (mesh->mTextureCoords[0])
		{ vertex.texCoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y}; }
		else
		{
			vertex.texCoord = {0.0f, 0.0f};
		}
		vertices.push_back(vertex);
	}

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString txt;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &txt);
		std::string texturePath = "textures/" + GetFileName(txt.C_Str());
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels =
			stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		assert(pixels);
		CreateTexture(m_Texture, pixels, texWidth, texHeight);
		stbi_image_free(pixels);
	}

	aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];
}

void Entity::CreateTexture(uint32_t& texture, unsigned char* pixels, GLsizei texWidth,
						   GLsizei texHeight)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
				 pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
}
