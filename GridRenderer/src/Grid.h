#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "ShaderProgram.h"

class ShaderProgram;

class Zone
{
public:
	Zone();
	virtual ~Zone();

	void Draw() const;
	void SetData(const std::vector<glm::vec3>& vertices, const std::vector<GLuint>& indices);
	void DrawUI(int index);

private:
	std::vector<glm::vec3> m_vertices;
	std::vector<GLuint> m_indices;

	GLuint m_vao = 0;
	GLuint m_vbo = 0;
	GLuint m_ibo = 0;

	int m_indexCount = 0;
};

class Grid
{
public:
	Grid();
	virtual ~Grid();

	void Draw() const;
	void Load(const std::string& filename);
	void DrawUI();

	std::string GetFilePath() { return m_filePath; }

private:
	std::string m_filePath;
	std::vector<Zone> m_zones;
	//std::vector<glm::vec3> m_vertices;
	//std::vector<GLuint> m_indices;

	//GLuint m_vao = 0;
	//GLuint m_vbo = 0;
	//GLuint m_ibo = 0;

	//uint32_t m_vertexDrawCount = 0;
};