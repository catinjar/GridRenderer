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

void Grid::Draw(const ShaderProgram& shader) const
{
	glUniform3f(shader["defaultColor"], m_color.r, m_color.g, m_color.b);

	for (const auto& zone : m_zones)
	{
		zone.Draw(m_renderMode);
	}
}

void Grid::Load(const std::string& filename)
{
	m_filePath = filename;

	m_zones.clear();

	const auto tecplotGrid = tecplot::Parse(filename);

	for (const auto& tecplotZone : tecplotGrid.zones)
	{
		Zone zone;
		zone.SetData(tecplotZone);
		m_zones.push_back(zone);
	}

	CalculateCenter();

	m_isLoaded = true;
}

void Grid::CalculateCenter()
{
	glm::vec3 sum = glm::vec3();

	for (const auto& zone : m_zones)
	{
		sum += zone.GetCenter();
	}

	m_center = sum / (float)m_zones.size();
}

void Grid::DrawUI()
{
	if (!m_isLoaded)
	{
		ImGui::Text("No grid loaded");
		return;
	}

	ImGui::Text(m_filePath.c_str());

	ImGui::Text("Render Mode:");
	if (ImGui::RadioButton("Lines", m_renderMode == RenderMode::Lines))
		m_renderMode = RenderMode::Lines;
	ImGui::SameLine();
	if (ImGui::RadioButton("Points", m_renderMode == RenderMode::Points))
		m_renderMode = RenderMode::Points;

	ImGui::ColorEdit3("Color", &m_color[0]);

	ImGui::Text("Zones:");

	for (int i = 0; i < m_zones.size(); ++i)
	{
		m_zones[i].DrawUI(i);
	}
}