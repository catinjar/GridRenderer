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
	bool isActive = true;
	
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;
	
	std::vector<GLfloat> param1;
	std::vector<GLfloat> param2;
	std::vector<GLfloat> param3;

	std::vector<GLint> iIndices;
	std::vector<GLint> jIndices;
	std::vector<GLint> kIndices;

	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ibo = 0;

	int indexCount = 0;
};