#include "Grid.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <nfd/nfd.h>

#include "ShaderProgram.h"

const uint32_t COORDS_COUNT = 3;

const char* TECPLOT_TITLE = "TITLE";
const char* TECPLOT_VARIABLES = "VARIABLES";
const char* TECPLOT_ZONE = "ZONE";

std::vector<glm::vec3> loadGridData(const std::string& filePath)
{
	std::ifstream infile(filePath);

	std::string line;
	int lineNumber = 0;
	
	std::vector<glm::vec3> vertices;

	while (std::getline(infile, line))
	{
		if (line.find(TECPLOT_TITLE) != std::string::npos)
		{
			std::cout << "TITLE" << std::endl;
		}
		else if (line.find(TECPLOT_VARIABLES) != std::string::npos)
		{
			std::cout << "VARIABLES" << std::endl;
		}
		else if (line.find(TECPLOT_ZONE) != std::string::npos)
		{
			std::cout << "ZONE" << std::endl;
		}
		else
		{
			float x, y, z;

			std::istringstream iss(line);

			if (!(iss >> x >> y >> z))
			{
				std::cout << "Can't parse line number " << lineNumber << " of file " << filePath << std::endl;
				std::cout << line << std::endl;
			}

			vertices.push_back(glm::vec3(x, y, z));
		}

		++lineNumber;
	}

	return vertices;
}

Grid::Grid()
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, COORDS_COUNT, GL_FLOAT, GL_FALSE, COORDS_COUNT * sizeof(GLfloat), (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Grid::~Grid()
{
	glDeleteBuffers(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}

void Grid::draw() const
{
	if (m_vao <= 0 || m_vbo <= 0)
	{
		std::cout << "Grid's VAO and VBO are not initialized" << std::endl;
		return;
	}

	glBindVertexArray(m_vao);
	glPointSize(5.0f);
	glDrawArrays(GL_POINTS, 0, m_vertices.size());
	glBindVertexArray(0);
}

void Grid::load(const std::string& filename)
{
	m_filePath = filename;
	m_vertices = loadGridData(filename);

	glBindVertexArray(m_vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLfloat) * COORDS_COUNT, (GLfloat*)m_vertices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}