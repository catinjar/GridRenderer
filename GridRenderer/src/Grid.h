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

	glm::vec3 GetCenter() const { return center; }

private:
	void CalculateCenter();

	bool isLoaded = false;
	std::string filePath;
	std::vector<Zone> zones;

	RenderMode renderMode = RenderMode::Lines;

	glm::vec3 center = glm::vec3();
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
};