#include "Grid.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>

#include <nfd/nfd.h>

#include "third_party/imgui/imgui.h"

#include "ShaderProgram.h"

const uint32_t COORDS_COUNT = 3;

const char* TECPLOT_TITLE = "TITLE";
const char* TECPLOT_VARIABLES = "VARIABLES";
const char* TECPLOT_ZONE = "ZONE";

Zone::Zone()
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, COORDS_COUNT, GL_FLOAT, GL_FALSE, COORDS_COUNT * sizeof(GLfloat), (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Zone::~Zone()
{
	glDeleteBuffers(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}

void Zone::Draw() const
{
	if (m_vao <= 0 || m_vbo <= 0)
	{
		std::cout << "Grid's VAO and VBO are not initialized" << std::endl;
		return;
	}

	glBindVertexArray(m_vao);
	//glDrawArrays(GL_POINTS, 0, m_vertices.size());
	glDrawElements(GL_LINES, m_indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Zone::SetData(const std::vector<glm::vec3>& vertices, const std::vector<GLuint>& indices)
{
	m_vertices = vertices;
	m_indices = indices;

	m_indexCount = m_indices.size();

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLfloat) * COORDS_COUNT, (GLfloat*)m_vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Zone::DrawUI(int index)
{
	bool changed = ImGui::InputInt("Index Count" + index, &m_indexCount);

	if (m_indexCount < 0)
		m_indexCount = 0;

	if (m_indexCount >= m_indices.size())
		m_indexCount = m_indices.size();
}

void TrimSpacesL(std::string& str)
{
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int c) { return !std::isspace(c); }));
}

void TrimWordL(std::string& str)
{
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int c) { return std::isspace(c) || std::isdigit(c); }));
}

void TrimNumberL(std::string& str)
{
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int c) { return !std::isdigit(c); }));
}

Grid::Grid()
{
}

Grid::~Grid()
{
}

void Grid::Draw() const
{
	for (const auto& zone : m_zones)
	{
		zone.Draw();
	}
}

void Grid::Load(const std::string& filename)
{
	m_filePath = filename;

	m_zones.clear();

	std::ifstream infile(filename);
	std::string line;
	
	int lineNumber = 0;

	while (infile.peek() != EOF)
	{
		while (std::getline(infile, line) && line.find("TITLE") == std::string::npos)
		{
		}

		if (infile.peek() == EOF)
			break;

		// Read title
		std::cout << "TITLE" << std::endl;
		//std::getline(infile, line);
		++lineNumber;

		// Read variables
		std::cout << "VARIABLES" << std::endl;
		std::getline(infile, line);
		++lineNumber;

		// Read zone
		std::cout << "ZONE" << std::endl;
		std::getline(infile, line);
		++lineNumber;

		TrimSpacesL(line);
		TrimWordL(line);
		TrimSpacesL(line);
		TrimWordL(line);
		TrimSpacesL(line);

		uint32_t I, J, K;
		
		{
			std::vector<char> numberChars;
			for (char c : line)
			{
				if (!std::isdigit(c))
					break;

				numberChars.push_back(c);
			}

			I = std::stoi(std::string(numberChars.data()));
		}

		TrimNumberL(line);
		TrimWordL(line);
		TrimSpacesL(line);
		TrimWordL(line);
		TrimSpacesL(line);
		
		{
			std::vector<char> numberChars;
			for (char c : line)
			{
				if (!std::isdigit(c))
					break;

				numberChars.push_back(c);
			}

			J = std::stoi(std::string(numberChars.data()));
		}

		TrimNumberL(line);
		TrimWordL(line);
		TrimSpacesL(line);
		TrimWordL(line);
		TrimSpacesL(line);

		{
			std::vector<char> numberChars;
			for (char c : line)
			{
				if (!std::isdigit(c))
					break;

				numberChars.push_back(c);
			}

			K = std::stoi(std::string(numberChars.data()));
		}

		// Read vertices
		std::vector<glm::vec3> vertices;
		std::vector<GLuint> indices;
		
		int vertexCount = I * J * K;
		for (int index = 0; index < vertexCount; ++index)
		{
			std::getline(infile, line);
			++lineNumber;

			float x, y, z;

			std::istringstream iss(line);

			if (!(iss >> x >> y >> z))
			{
				std::cout << "Can't parse line number " << lineNumber << " of file " << filename << std::endl;
				std::cout << line << std::endl;
				--index;
				continue;
			}

			vertices.push_back(glm::vec3(x, y, z));
		}

		for (int k = 0; k < K; ++k)
		{
			for (int j = 0; j < J; ++j)
			{
				for (int i = 0; i < I; ++i)
				{
					if (i != I - 1)
					{
						indices.push_back(k * J * I + j * I + i);
						indices.push_back(k * J * I + j * I + i + 1);
					}

					if (j != J - 1)
					{
						indices.push_back(k * J * I + j * I + i);
						indices.push_back(k * J * I + (j + 1) * I + i);
					}

					if (k != K - 1)
					{
						indices.push_back(k * J * I + j * I + i);
						indices.push_back((k + 1) * J * I + j * I + i);
					}
				}	
			}
		}
		
		Zone zone;
		zone.SetData(vertices, indices);

		m_zones.push_back(zone);
	}

	infile.close();
}

void Grid::DrawUI()
{
	for (int i = 0; i < m_zones.size(); ++i)
	{
		m_zones[i].DrawUI(i);
	}
}