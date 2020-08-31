#pragma once

#include <string>
#include <filesystem>

#include <GL/glew.h>

namespace fs = std::filesystem;

class ShaderProgram
{
public:
	ShaderProgram(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename);
	~ShaderProgram();

	void Use() const;
	void Compile();
	void HotloadChanges();

	GLuint operator[](const char* name) const;

private:
	GLuint m_programId = 0;

	std::string m_vertexShaderFilename;
	std::string m_fragmentShaderFilename;

	std::uint64_t m_lastCompileTs = 0;

	fs::file_time_type m_vertexShaderTs;
	fs::file_time_type m_fragmentShaderTs;
};