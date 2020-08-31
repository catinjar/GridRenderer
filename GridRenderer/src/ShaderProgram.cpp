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
	m_vertexShaderFilename = vertexShaderFilename;
	m_fragmentShaderFilename = fragmentShaderFilename;

	m_vertexShaderTs = fs::last_write_time(vertexShaderFilename);
	m_fragmentShaderTs = fs::last_write_time(fragmentShaderFilename);

	Compile();
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_programId);
}

void ShaderProgram::Use() const
{
	if (m_programId == 0)
		return;

	glUseProgram(m_programId);
}

void ShaderProgram::Compile()
{
	if (m_programId > 0)
		glDeleteProgram(m_programId);

	m_programId = glCreateProgram();

	const GLuint vertexShaderId = CreateShader(GL_VERTEX_SHADER, m_vertexShaderFilename);
	glAttachShader(m_programId, vertexShaderId);
	glDeleteShader(vertexShaderId);
	
	const GLuint fragmentShaderId = CreateShader(GL_FRAGMENT_SHADER, m_fragmentShaderFilename);
	glAttachShader(m_programId, fragmentShaderId);
	glDeleteShader(fragmentShaderId);

	glLinkProgram(m_programId);

	CheckProgramErrors(m_programId);

	m_lastCompileTs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); // TODO: Move to misc
}

GLuint ShaderProgram::operator[](const char* name) const
{
	return glGetUniformLocation(m_programId, name);
}

void ShaderProgram::HotloadChanges()
{
	const uint32_t HOTLOAD_MIN_DELAY_MILLIS = 1000;

	const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	if (now - m_lastCompileTs < HOTLOAD_MIN_DELAY_MILLIS)
		return;

	bool changed = false;

	{
		const auto ts = fs::last_write_time(m_vertexShaderFilename);
		if (ts > m_vertexShaderTs)
		{
			m_vertexShaderTs = ts;
			changed = true;
		}
	}

	{
		const auto ts = fs::last_write_time(m_fragmentShaderFilename);
		if (ts > m_fragmentShaderTs)
		{
			m_fragmentShaderTs = ts;
			changed = true;
		}
	}

	if (changed)
		Compile();
}
