#include "Grid.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <glm\gtc\matrix_transform.hpp>

#include "third_party/imgui/imgui.h"

#include "Zone.h"
#include "Tecplot.h"
#include "NFDHelper.h"

Grid::Grid()
{
}

Grid::~Grid()
{
}

void Grid::Update()
{
	material.Update();
}

void Grid::Draw(const CameraController& camera) const
{
	material.Render(camera, *this);

	for (const auto& zone : zones)
	{
		zone.Draw(renderMode);
	}
}

void Grid::Load(const std::string& filename)
{
	filePath = filename;

	zones.clear();

	const auto tecplotGrid = tecplot::Parse(filename);

	for (const auto& tecplotZone : tecplotGrid.zones)
	{
		Zone zone;
		zone.SetData(tecplotZone);
		zones.push_back(zone);
	}

	CalculateCenter();

	isLoaded = true;
}

void Grid::CalculateCenter()
{
	glm::vec3 sum = glm::vec3();

	for (const auto& zone : zones)
	{
		sum += zone.GetCenter();
	}

	center = sum / (float)zones.size();
}

void Grid::DrawUI()
{
	DrawGridWindow();
	DrawMaterialWindow();
}

void Grid::DrawGridWindow()
{
	ImGui::Begin("Grid");

	if (ImGui::Button("Load Grid"))
	{
		std::string filePath;
		if (NFD_ChooseFile(filePath))
			Load(filePath);
	}

	if (!isLoaded)
	{
		ImGui::Text("No grid loaded");
		ImGui::End();
		return;
	}

	ImGui::Text("Path %s", filePath.c_str());

	ImGui::DragFloat3("Position", &position[0], 0.01f);
	ImGui::DragFloat3("Rotation", &rotation[0], 0.01f);
	ImGui::DragFloat3("Scale", &scale[0], 0.01f);

	ImGui::Text("Render Mode:");

	if (ImGui::RadioButton("Lines", renderMode == RenderMode::Lines))
		renderMode = RenderMode::Lines;

	ImGui::SameLine();

	if (ImGui::RadioButton("Points", renderMode == RenderMode::Points))
		renderMode = RenderMode::Points;

	ImGui::Text("Zones:");

	for (int i = 0; i < zones.size(); ++i)
	{
		zones[i].DrawUI(i);
	}

	ImGui::End();
}

void Grid::DrawMaterialWindow()
{
	material.DrawUI();
}

glm::mat4 Grid::GetTransform() const
{
	glm::mat4 transform(1.0f);

	transform = glm::translate(transform, position);
	transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, scale);

	return transform;
}