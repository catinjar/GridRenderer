#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "ShaderProgram.h"
#include "CameraController.h"
#include "Material.h"

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

	void Update();
	void Draw(const CameraController& camera);
	void Load(const std::string& filename);
	void DrawUI();
	void DrawGridWindow();
	void DrawMaterialWindow();

	const char* GetName() const { return filePath.c_str(); }
	glm::vec3 GetCenter() const { return center; }
	glm::mat4 GetTransform() const;

	Material* GetMaterial() { return &material; }
	NodeGraph* GetNodeGraph() { return &nodeGraph; }

private:
	void CalculateCenter();

	bool isLoaded = false;
	std::string filePath = "Not loaded";
	std::vector<Zone> zones;

	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

	RenderMode renderMode = RenderMode::Lines;

	Material material;
	NodeGraph nodeGraph;

	glm::vec3 center = glm::vec3();
};