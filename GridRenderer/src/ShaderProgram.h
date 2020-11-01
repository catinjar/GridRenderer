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
	bool HotloadChanges();
	void SetShaders(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename);

	GLuint operator[](const char* name) const;
	GLuint operator[](const std::string name) const;

private:
	GLuint programId = 0;

	std::string vertexShaderFilename;
	std::string fragmentShaderFilename;

	std::uint64_t lastCompileTs = 0;

	fs::file_time_type vertexShaderTs;
	fs::file_time_type fragmentShaderTs;
};