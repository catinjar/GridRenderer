#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "ShaderProgram.h"

class Zone;

enum class RenderMode
{
	Points,
	Lines
};

class Grid
{
public:
	Grid();
	virtual ~Grid();

	void Draw(const ShaderProgram& shader) const;
	void Load(const std::string& filename);
	void DrawUI();

	glm::vec3 GetCenter() const { return m_center; }

private:
	void CalculateCenter();

	bool m_isLoaded = false;
	std::string m_filePath;
	std::vector<Zone> m_zones;

	RenderMode m_renderMode = RenderMode::Lines;

	glm::vec3 m_center = glm::vec3();
	glm::vec3 m_color = glm::vec3(1.0f, 1.0f, 1.0f);
};