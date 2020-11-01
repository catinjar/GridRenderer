#include "ShaderProgram.h"

#include <fstream>
#include <iostream>

#include "Misc.h"

void CheckShaderErrors(const std::string& filename, const GLuint shader)
{
	int compileResult = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);

	if (compileResult == GL_FALSE)
	{
		int infoLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);

		std::vector<char> shaderLog(infoLength);
		glGetShaderInfoLog(shader, infoLength, nullptr, &shaderLog[0]);

		std::cout << "ShaderProgram: Shader compile error: " << filename << &shaderLog[0] << std::endl;
	}
}

GLuint CreateShader(const GLuint shaderType, const std::string& filename)
{
	const GLuint shader = glCreateShader(shaderType);

	const std::string shaderSource = misc::ReadAllText(filename);
	const GLchar* shaderSourcePtr = shaderSource.c_str();
	const int shaderSourceSize = shaderSource.size();

	glShaderSource(shader, 1, &shaderSourcePtr, &shaderSourceSize);
	glCompileShader(shader);

	CheckShaderErrors(filename, shader);

	return shader;
}

void CheckProgramErrors(GLuint program)
{
	int linkResult = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &linkResult);

	if (linkResult == GL_FALSE)
	{
		int infoLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLength);

		std::vector<char> programLog(infoLength);
		glGetProgramInfoLog(program, infoLength, nullptr, &programLog[0]);

		std::cout << "ShaderProgram: Shader link error: " << &programLog[0] << std::endl;
	}
}

ShaderProgram::ShaderProgram(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename)
{
	this->vertexShaderFilename = vertexShaderFilename;
	this->fragmentShaderFilename = fragmentShaderFilename;

	vertexShaderTs = fs::last_write_time(vertexShaderFilename);
	fragmentShaderTs = fs::last_write_time(fragmentShaderFilename);

	Compile();
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(programId);
}

void ShaderProgram::Use() const
{
	if (programId == 0)
		return;

	glUseProgram(programId);
}

void ShaderProgram::Compile()
{
	if (programId > 0)
		glDeleteProgram(programId);

	programId = glCreateProgram();

	const GLuint vertexShaderId = CreateShader(GL_VERTEX_SHADER, vertexShaderFilename);
	glAttachShader(programId, vertexShaderId);
	glDeleteShader(vertexShaderId);
	
	const GLuint fragmentShaderId = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderFilename);
	glAttachShader(programId, fragmentShaderId);
	glDeleteShader(fragmentShaderId);

	glLinkProgram(programId);

	CheckProgramErrors(programId);

	lastCompileTs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); // TODO: Move to misc
}

GLuint ShaderProgram::operator[](const char* name) const
{
	return glGetUniformLocation(programId, name);
}

GLuint ShaderProgram::operator[](const std::string name) const
{
	return glGetUniformLocation(programId, name.c_str());
}

bool ShaderProgram::HotloadChanges()
{
	const uint32_t HOTLOAD_MIN_DELAY_MILLIS = 1000;

	const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	if (now - lastCompileTs < HOTLOAD_MIN_DELAY_MILLIS)
		return false;

	bool changed = false;

	{
		const auto ts = fs::last_write_time(vertexShaderFilename);
		if (ts > vertexShaderTs)
		{
			vertexShaderTs = ts;
			changed = true;
		}
	}

	{
		const auto ts = fs::last_write_time(fragmentShaderFilename);
		if (ts > fragmentShaderTs)
		{
			fragmentShaderTs = ts;
			changed = true;
		}
	}

	if (changed)
		Compile();

	return changed;
}

void ShaderProgram::SetShaders(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename)
{
	this->vertexShaderFilename = vertexShaderFilename;
	this->fragmentShaderFilename = fragmentShaderFilename;

	Compile();
}

