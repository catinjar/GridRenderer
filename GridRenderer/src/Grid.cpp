#include "Grid.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "third_party/imgui/imgui.h"

#include "Zone.h"
#include "Tecplot.h"

Grid::Grid()
{
}

Grid::~Grid()
{
}

void Grid::Draw() const
{
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
	if (!isLoaded)
	{
		ImGui::Text("No grid loaded");
		return;
	}

	ImGui::Text(filePath.c_str());

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
}