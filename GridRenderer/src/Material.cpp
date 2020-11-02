#include "Material.h"

#include "third_party/imgui/imgui.h"

#include "NFDHelper.h"
#include "Misc.h"

void Material::Update()
{
	if (isHotloadEnabled)
	{
		if (shaderProgram.HotloadChanges())
			shaderMetaData.HotloadChanges();
	}
}

void Material::Render(const CameraController& camera, const Grid& grid) const
{
	shaderProgram.Use();

	const auto view = camera.GetView();
	const auto projection = camera.GetProjection();

	glUniformMatrix4fv(shaderProgram["view"], 1, false, &view[0][0]);
	glUniformMatrix4fv(shaderProgram["projection"], 1, false, &projection[0][0]);

	shaderMetaData.ApplyUniforms(shaderProgram);
	ApplyDefaultUniforms();
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

	shaderMetaData.DrawUI();

	ImGui::End();
}

// TODO: Move shader selection inside ShaderProgram
void Material::RecompileShaderProgram()
{
	shaderProgram.SetShaders(vertexShaderFilename, fragmentShaderFilename);
	shaderMetaData = ShaderMetaData(vertexShaderFilename, fragmentShaderFilename);
}

void Material::ApplyDefaultUniforms() const
{
	double seconds = misc::GetCurrentTimeInSeconds();

	glUniform4d(shaderProgram["time"], seconds, seconds / 2.0, seconds * 2, seconds * 3);
	glUniform4f(shaderProgram["sinTime"], glm::sin(seconds), glm::sin(seconds / 2.0), glm::sin(seconds / 4.0), glm::sin(seconds / 8.0));
	glUniform4f(shaderProgram["cosTime"], glm::cos(seconds), glm::cos(seconds / 2.0), glm::cos(seconds / 4.0), glm::cos(seconds / 8.0));
}