#include "Material.h"

#include "third_party/imgui/imgui.h"
#include "NFDHelper.h"

void Material::Update()
{
	if (isHotloadEnabled)
		shaderProgram.HotloadChanges();
}

void Material::Render(const CameraController& camera, const Grid& grid) const
{
	shaderProgram.Use();

	const auto view = camera.GetView();
	const auto projection = camera.GetProjection();

	glUniformMatrix4fv(shaderProgram["view"], 1, false, &view[0][0]);
	glUniformMatrix4fv(shaderProgram["projection"], 1, false, &projection[0][0]);

	glUniform3f(shaderProgram["defaultColor"], color.r, color.g, color.b);
}

void Material::DrawUI()
{
	ImGui::Begin("Material");

	ImGui::Checkbox("Shader Hotloading", &isHotloadEnabled);

	ImGui::Text(vertexShaderFilename.c_str());
	ImGui::SameLine();

	// TODO: Check file extension
	if (ImGui::Button("Select##Vertex"))
	{
		if (NFD_ChooseFile(vertexShaderFilename))
			RecompileShaderProgram();
	}

	ImGui::Text(fragmentShaderFilename.c_str());
	ImGui::SameLine();

	if (ImGui::Button("Select##Fragment"))
	{
		if (NFD_ChooseFile(fragmentShaderFilename))
			RecompileShaderProgram();
	}

	ImGui::ColorEdit3("Color", &color[0]);

	ImGui::End();
}

// TODO: Move shader selection inside ShaderProgram
void Material::RecompileShaderProgram()
{
	shaderProgram.SetShaders(vertexShaderFilename, fragmentShaderFilename);
}