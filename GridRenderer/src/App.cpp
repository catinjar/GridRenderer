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
	grids.push_back(std::make_unique<Grid>());

	materialEditor.Init(grids.back()->GetMaterial(), grids.back()->GetNodeGraph());

	Input();
}

App::~App()
{
	materialEditor.Shutdown();
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

	/*if (ImGui::GetIO().WantCaptureMouse ||
		ImGui::GetIO().WantCaptureKeyboard)
	{
		return;
	}*/

	camera.Input();
}

void App::UI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();

	DrawOptionsWindow();
	DrawGridUI();

	if (isImguiDemoEnabled)
		ImGui::ShowDemoWindow(&isImguiDemoEnabled);

	materialEditor.Draw();
}

void App::DrawOptionsWindow()
{
	ImGui::Begin("Options");

	DrawSettings();
	DrawGridList();

	ImGui::End();
}

void App::DrawSettings()
{
	ImGui::Checkbox("Show ImGui Demo", &isImguiDemoEnabled);
}

void App::DrawGridList()
{
	if (ImGui::Button("Add"))
		grids.push_back(std::make_unique<Grid>());

	if (grids.size() > 1)
	{
		ImGui::SameLine();

		if (ImGui::Button("Delete"))
		{
			grids.erase(grids.begin() + selectedGridIndex);

			if (selectedGridIndex >= grids.size())
				selectedGridIndex = grids.size() - 1;
		}
	}

	std::vector<const char*> gridNames;
	for (const auto& grid : grids)
		gridNames.push_back(grid->GetName());

	if (ImGui::ListBox("Grids", &selectedGridIndex, &gridNames[0], gridNames.size(), 5))
	{
		materialEditor.SetMaterial(grids[selectedGridIndex]->GetMaterial(), grids[selectedGridIndex]->GetNodeGraph());
	}
}

void App::DrawGridUI()
{
	grids[selectedGridIndex]->DrawUI();
}

void App::Update()
{
	camera.Update(*grids[selectedGridIndex]);
	grids[selectedGridIndex]->Update();
}

void App::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0f, 191.0f / 255.0f, 1.0f, 1.0f);

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	glViewport(0, 0, (int)displayMode.w, (int)displayMode.h);

	for (const auto& grid : grids)
	{
		grid->Draw(camera);
	}

	auto imguiIO = &ImGui::GetIO();
	glViewport(0, 0, (int)imguiIO->DisplaySize.x, (int)imguiIO->DisplaySize.y);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(window);
}