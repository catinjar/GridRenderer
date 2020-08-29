#pragma once

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Tecplot.h"
#include "Grid.h"

class Zone
{
public:
	Zone();
	virtual ~Zone();

	void Draw(RenderMode renderMode) const;
	void SetData(const tecplot::TecplotZone& tecplotZone);
	void DrawUI(int index);

	glm::vec3 GetCenter() const;

private:
	bool m_isActive = true;
	
	std::vector<glm::vec3> m_vertices;
	std::vector<GLuint> m_indices;
	
	std::vector<GLfloat> m_param1;
	std::vector<GLfloat> m_param2;
	std::vector<GLfloat> m_param3;

	GLuint m_vao = 0;
	GLuint m_vbo = 0;
	GLuint m_ibo = 0;

	int m_indexCount = 0;
};