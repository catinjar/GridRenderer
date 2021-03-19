#include "Zone.h"

#include <iostream>
#include <sstream>

#include "third_party/imgui/imgui.h"

Zone::Zone()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Zone::~Zone()
{
	glDeleteBuffers(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

glm::vec3 Zone::GetCenter() const
{
	glm::vec3 sum = glm::vec3();

	for (const auto vertex : vertices)
	{
		sum += vertex;
	}

	return sum / (float)vertices.size();
}

void Zone::Draw(RenderMode renderMode) const
{
	if (!isActive)
	{
		return;
	}

	if (vao <= 0 || vbo <= 0)
	{
		std::cout << "Grid's VAO and VBO are not initialized" << std::endl;
		return;
	}

	glBindVertexArray(vao);
	
	switch (renderMode)
	{
	case RenderMode::Lines:
		glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, 0);
		break;

	case RenderMode::Points:
		glDrawArrays(GL_POINTS, 0, vertices.size());
		break;
	}

	glBindVertexArray(0);
}

std::vector<GLuint> CalculateIndices(const tecplot::TecplotZone& tecplotZone);

void Zone::SetData(const tecplot::TecplotZone& tecplotZone)
{
	vertices = tecplotZone.vertices;
	
	indices = CalculateIndices(tecplotZone);
	indexCount = indices.size();

	param1 = tecplotZone.param1;
	param2 = tecplotZone.param2;
	param3 = tecplotZone.param3;

	uint32_t vertexCount = vertices.size();

	iIndices.clear();
	jIndices.clear();
	kIndices.clear();

	for (int i = 0; i < indices.size() - 2; i += 3)
	{
		iIndices.push_back(indices[i]);
		jIndices.push_back(indices[i + 1]);
		kIndices.push_back(indices[i + 2]);
	}

	uint32_t coordsSize = vertexCount * sizeof(GLfloat) * 3;
	uint32_t paramSize = vertexCount * sizeof(GLfloat);
	uint32_t indexSize = vertexCount * sizeof(GLint);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(GLfloat) * 6, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0,												coordsSize,							(GLfloat*)vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, coordsSize,									param1.size() * sizeof(GLfloat),	(GLfloat*)param1.data());
	glBufferSubData(GL_ARRAY_BUFFER, coordsSize + paramSize,						param2.size() * sizeof(GLfloat),	(GLfloat*)param2.data());
	glBufferSubData(GL_ARRAY_BUFFER, coordsSize + paramSize * 2,					param3.size() * sizeof(GLfloat),	(GLfloat*)param3.data());
	glBufferSubData(GL_ARRAY_BUFFER, coordsSize + paramSize * 3,					iIndices.size() * sizeof(GLint),	(GLint*)iIndices.data());
	glBufferSubData(GL_ARRAY_BUFFER, coordsSize + paramSize * 3 + indexSize,		jIndices.size() * sizeof(GLint),	(GLint*)jIndices.data());
	glBufferSubData(GL_ARRAY_BUFFER, coordsSize + paramSize * 3 + indexSize * 2,	kIndices.size() * sizeof(GLint),	(GLint*)kIndices.data());

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (void*)(vertexCount * sizeof(GLfloat) * 3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (void*)(vertexCount * sizeof(GLfloat) * 4));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (void*)(vertexCount * sizeof(GLfloat) * 5));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_INT, GL_FALSE, sizeof(GLint), (void*)(vertexCount * sizeof(GLfloat) * 6));

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 1, GL_INT, GL_FALSE, sizeof(GLint), (void*)(vertexCount * sizeof(GLfloat) * 6 + vertexCount * sizeof(GLint)));

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 1, GL_INT, GL_FALSE, sizeof(GLint), (void*)(vertexCount * sizeof(GLfloat) * 6 + vertexCount * sizeof(GLint) * 2));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Zone::DrawUI(int index)
{
	ImGui::PushID(index);

	ImGui::Text("Zone #%i", index);
	ImGui::SameLine(ImGui::GetWindowWidth() - 60);
	ImGui::Checkbox("##isActive", &isActive);

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