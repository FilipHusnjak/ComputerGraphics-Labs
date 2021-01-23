#pragma once

#include <cassert>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glad/glad.h"

class Shader
{
public:
	Shader() = default;
	~Shader();
	uint32_t GetID() const
	{
		return m_ID;
	}
	void Load(const std::string& vertexPath, const std::string& fragmentPath);
	void Use()
	{
		assert(m_ID);
		glUseProgram(m_ID);
	}
	void SetBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
	}
	void SetInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
	}
	void SetFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
	}
	void SetVec3f(const std::string& name, const glm::vec3 value)
	{
		glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, glm::value_ptr(value));
	}
	void SetMat4f(const std::string& name, const glm::mat4 value)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE,
						   glm::value_ptr(value));
	}

private:
	std::string ReadFile(const std::string& file);
	void CheckCompileErrors(unsigned int shader, std::string type);

private:
	uint32_t m_ID{};
};
