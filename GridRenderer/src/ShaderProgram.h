#pragma once

#include <string>
#include <filesystem>

#include <GL/glew.h>

namespace fs = std::filesystem;

class ShaderProgram
{
public:
	ShaderProgram() {}
	ShaderProgram(const std::string& vertexSourceCode, const std::string& fragmentSourceCode);
	~ShaderProgram();

	void Use() const;
	void Compile();
	void SetShaders(const std::string& vertexSourceCode, const std::string& fragmentSourceCode);

	GLuint operator[](const char* name) const;
	GLuint operator[](const std::string name) const;

private:
	GLuint programId = 0;

	std::string vertexSourceCode;
	std::string fragmentSourceCode;

	std::uint64_t lastCompileTs = 0;

	fs::file_time_type vertexShaderTs;
	fs::file_time_type fragmentShaderTs;
};