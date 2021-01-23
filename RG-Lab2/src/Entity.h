#pragma once

#include <string>
#include <vector>

#include "glad/glad.h"
#include <glm/glm.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 texCoord;
};

class Entity
{
public:
	Entity() = default;
	~Entity();
	void Load(const std::string& filename, float newScale, glm::u8vec4 color,
			  const std::string& normalMap = "");
	void Render();

private:
	void ProcessNode(const aiScene* scene, aiNode* node, std::vector<Vertex>& vertices,
					 std::vector<uint32_t>& indices);
	void ProcessMesh(const aiScene* scene, const aiMesh* mesh, std::vector<Vertex>& vertices,
							std::vector<uint32_t>& indices);
	void CreateTexture(uint32_t& texture, unsigned char* pixels, GLsizei texWidth,
					   GLsizei texHeight);

private:
	std::vector<uint32_t> m_Indices{};
	std::vector<Vertex> m_Vertices{};
	uint32_t VBO{};
	uint32_t VAO{};
	uint32_t EBO{};
	uint32_t m_Texture{};
	uint32_t m_NormalTexture{};
};
