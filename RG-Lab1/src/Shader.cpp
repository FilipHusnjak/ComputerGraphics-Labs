#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

Shader::~Shader()
{
	glDeleteProgram(m_ID);
}

void Shader::Load(const std::string& vertexPath, const std::string& fragmentPath)
{
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	std::string vertexShaderSource = ReadFile(vertexPath);
	const char* c = vertexShaderSource.c_str();
	glShaderSource(vertexShader, 1, &c, NULL);
	glCompileShader(vertexShader);
	CheckCompileErrors(vertexShader, "VERTEX");

	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragmentShaderSource = ReadFile(fragmentPath);
	c = fragmentShaderSource.c_str();
	glShaderSource(fragmentShader, 1, &c, NULL);
	glCompileShader(fragmentShader);
	CheckCompileErrors(fragmentShader, "FRAGMENT");

	m_ID = glCreateProgram();
	glAttachShader(m_ID, vertexShader);
	glAttachShader(m_ID, fragmentShader);
	glLinkProgram(m_ID);
	CheckCompileErrors(m_ID, "PROGRAM");

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

std::string Shader::ReadFile(const std::string& file)
{
	std::ostringstream sstream;
	std::ifstream fs(file);
	sstream << fs.rdbuf();
	fs.close();
	return sstream.str();
}

void Shader::CheckCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
					  << infoLog << "\n -- --------------------------------------------------- -- "
					  << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
					  << infoLog << "\n -- --------------------------------------------------- -- "
					  << std::endl;
		}
	}
}
