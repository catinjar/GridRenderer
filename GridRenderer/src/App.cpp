#include "App.h"

#include <iostream>

#include <SDL2/SDL.h>

#include "third_party/imgui/imgui.h"
#include "third_party/imgui/imgui_impl_sdl.h"
#include "third_party/imgui/imgui_impl_opengl3.h"

#include "NFDHelper.h"

App::App(SDL_Window* window)
	: window(window)
{
}

App::~App()
{
}

void App::Frame()
{
	Input();
	UI();
	Update();
	Render();
}

bool App::IsRunning()
{
	return isRunning;
}

void App::Input()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);

		if (event.type == SDL_QUIT || event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE)
		{
			isRunning = false;
		}

		if (event.type == SDL_MOUSEWHEEL)
		{
			camera.Scroll(event.wheel.y);
		}
	}

	if (ImGui::GetIO().WantCaptureMouse ||
		ImGui::GetIO().WantCaptureKeyboard)
	{
		return;
	}

	camera.Input();
}

void App::UI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();

	DrawOptionsWindow();
	DrawGridWindow();
	DrawMaterialWindow();

	if (isImguiDemoEnabled)
		ImGui::ShowDemoWindow(&isImguiDemoEnabled);
}

void App::DrawOptionsWindow()
{
	ImGui::Begin("Options");

	DrawFileUI();
	DrawSettings();

	ImGui::End();
}

void App::DrawSettings()
{
	ImGui::Checkbox("Shader Hotloading", &isHotloadEnabled);
	ImGui::Checkbox("Show ImGui Demo", &isImguiDemoEnabled);
}

void App::DrawFileUI()
{
	if (ImGui::Button("Load .dat"))
	{
		std::string filePath;
		if (NFD_ChooseFile(filePath))
			grid.Load(filePath);
	}
}

void App::DrawGridWindow()
{
	ImGui::Begin("Grid");

	grid.DrawUI();

	ImGui::End();
}

void App::DrawMaterialWindow()
{
	ImGui::Begin("Material");

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

	ImGui::End();
}

// TODO: Move shader selection inside ShaderProgram
void App::RecompileShaderProgram()
{
	shaderProgram.SetShaders(vertexShaderFilename, fragmentShaderFilename);
}

void App::Update()
{
	camera.Update(grid);

	if (isHotloadEnabled)
		shaderProgram.HotloadChanges();
}

void App::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0f, 191.0f / 255.0f, 1.0f, 1.0f);

	shaderProgram.Use();
	camera.ApplyUniforms(shaderProgram);
	grid.Draw(shaderProgram);

	auto imguiIO = &ImGui::GetIO();
	glViewport(0, 0, (int)imguiIO->DisplaySize.x, (int)imguiIO->DisplaySize.y);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(window);
}