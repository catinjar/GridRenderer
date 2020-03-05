#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "ShaderProgram.h"

class ShaderProgram;

class Grid
{
public:
	Grid();
	virtual ~Grid();

	void draw() const;
	void load(const std::string& filename);

	std::string getFilePath() { return m_filePath; }

private:
	std::string m_filePath;
	std::vector<glm::vec3> m_vertices;

	GLuint m_vao = 0;
	GLuint m_vbo = 0;
};