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

	void use() const;
	void compile();

#ifdef _DEBUG
	void hotloadChanges();
#endif

private:
	GLuint m_programId = 0;

	std::string m_vertexShaderFilename;
	std::string m_fragmentShaderFilename;

	std::uint64_t m_lastCompileTs = 0;

	fs::file_time_type m_vertexShaderTs;
	fs::file_time_type m_fragmentShaderTs;
};