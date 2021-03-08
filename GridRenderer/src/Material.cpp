#include "Material.h"

#include "third_party/imgui/imgui.h"

#include "NFDHelper.h"
#include "Misc.h"
#include "Grid.h"

void Material::Render(const CameraController& camera, const Grid& grid) const
{
	shaderProgram.Use();
	shaderMetaData.ApplyUniforms(shaderProgram);
	ApplyDefaultUniforms(camera, grid);
}

void Material::DrawUI()
{
	ImGui::Begin("Material");

	shaderMetaData.DrawUI();

	ImGui::End();
}

void Material::SetSourceCode(const std::string& vertexSourceCode, const std::string& fragmentSourceCode)
{
	this->vertexSourceCode = vertexSourceCode;
	this->fragmentSourceCode = fragmentSourceCode;

	shaderProgram.SetShaders(vertexSourceCode, fragmentSourceCode);
	shaderMetaData = ShaderMetaData(vertexSourceCode, fragmentSourceCode);
}

void Material::ApplyDefaultUniforms(const CameraController& camera, const Grid& grid) const
{
	const auto view = camera.GetView();
	const auto projection = camera.GetProjection();
	const auto model = grid.GetTransform();

	glUniformMatrix4fv(shaderProgram["view"], 1, false, &view[0][0]);
	glUniformMatrix4fv(shaderProgram["projection"], 1, false, &projection[0][0]);
	glUniformMatrix4fv(shaderProgram["model"], 1, false, &model[0][0]);

	float seconds = misc::GetCurrentTimeInSeconds();

	glUniform4f(shaderProgram["time"], seconds, seconds / 2.0, seconds * 2, seconds * 3);
	glUniform4f(shaderProgram["sinTime"], glm::sin(seconds), glm::sin(seconds / 2.0), glm::sin(seconds / 4.0), glm::sin(seconds / 8.0));
	glUniform4f(shaderProgram["cosTime"], glm::cos(seconds), glm::cos(seconds / 2.0), glm::cos(seconds / 4.0), glm::cos(seconds / 8.0));
}