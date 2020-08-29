#include "Zone.h"

#include <iostream>
#include <sstream>

#include "third_party/imgui/imgui.h"

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

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Zone::~Zone()
{
	glDeleteBuffers(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}

glm::vec3 Zone::GetCenter() const
{
	glm::vec3 sum = glm::vec3();

	for (const auto vertex : m_vertices)
	{
		sum += vertex;
	}

	return sum / (float)m_vertices.size();
}

void Zone::Draw(RenderMode renderMode) const
{
	if (!m_isActive)
	{
		return;
	}

	if (m_vao <= 0 || m_vbo <= 0)
	{
		std::cout << "Grid's VAO and VBO are not initialized" << std::endl;
		return;
	}

	glBindVertexArray(m_vao);
	
	switch (renderMode)
	{
	case RenderMode::Lines:
		glDrawElements(GL_LINES, m_indexCount, GL_UNSIGNED_INT, 0);
		break;

	case RenderMode::Points:
		glDrawArrays(GL_POINTS, 0, m_vertices.size());
		break;
	}

	glBindVertexArray(0);
}

std::vector<GLuint> CalculateIndices(const tecplot::TecplotZone& tecplotZone);

void Zone::SetData(const tecplot::TecplotZone& tecplotZone)
{
	m_vertices = tecplotZone.vertices;
	
	m_indices = CalculateIndices(tecplotZone);
	m_indexCount = m_indices.size();

	m_param1 = tecplotZone.param1;
	m_param2 = tecplotZone.param2;
	m_param3 = tecplotZone.param3;

	uint32_t vertexCount = m_vertices.size();
	uint32_t coordsSize = vertexCount * sizeof(GLfloat) * 3;
	uint32_t paramSize = vertexCount * sizeof(GLfloat);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(GLfloat) * 6, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0,								coordsSize,							(GLfloat*)m_vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, coordsSize,					m_param1.size() * sizeof(GLfloat),	(GLfloat*)m_param1.data());
	glBufferSubData(GL_ARRAY_BUFFER, coordsSize + paramSize,		m_param2.size() * sizeof(GLfloat),	(GLfloat*)m_param2.data());
	glBufferSubData(GL_ARRAY_BUFFER, coordsSize + paramSize * 2,	m_param3.size() * sizeof(GLfloat),	(GLfloat*)m_param3.data());

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (void*)(vertexCount * sizeof(GLfloat) * 3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (void*)(vertexCount * sizeof(GLfloat) * 4));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (void*)(vertexCount * sizeof(GLfloat) * 5));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Zone::DrawUI(int index)
{
	ImGui::PushID(index);

	ImGui::Text("Zone #%i", index);
	ImGui::SameLine(ImGui::GetWindowWidth() - 60);
	ImGui::Checkbox("##isActive", &m_isActive);

	ImGui::PopID();
}

std::vector<GLuint> CalculateIndices(const tecplot::TecplotZone& tecplotZone)
{
	std::vector<GLuint> indices;

	uint32_t I = tecplotZone.I;
	uint32_t J = tecplotZone.J;
	uint32_t K = tecplotZone.K;

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

	return indices;
}