#include "App.h"

#include <iostream>

#include <SDL2/SDL.h>
#include <nfd/nfd.h>

#include "third_party/imgui/imgui.h"
#include "third_party/imgui/imgui_impl_sdl.h"
#include "third_party/imgui/imgui_impl_opengl3.h"

App::App(SDL_Window* window)
	: m_window(window)
{
}

App::~App()
{

}

void App::frame()
{
	input();
	ui();
	update();
	render();
}

bool App::isRunning()
{
	return m_isRunning;
}

void App::input()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);

		if (event.type == SDL_QUIT ||
			event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE)
		{
			m_isRunning = false;
		}
	}

	if (ImGui::GetIO().WantCaptureMouse ||
		ImGui::GetIO().WantCaptureKeyboard)
	{
		return;
	}
}

void App::ui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_window);
	ImGui::NewFrame();

	static bool showDemoWindow = false;
	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);

	static bool showOptionsWindow = true;
	ImGui::Begin("Options", &showOptionsWindow);

	auto gridFilePath = m_grid.getFilePath();

	if (gridFilePath.length() > 0)
		ImGui::Text(gridFilePath.c_str());
	else
		ImGui::Text("No file loaded");

	if (ImGui::Button("Load .dat"))
	{
		nfdchar_t* filePath = nullptr;
		nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &filePath);

		if (result == NFD_OKAY)
		{
			std::cout << "NFD: File path is " << filePath << std::endl;
			m_grid.load(filePath);
		}
		else if (result == NFD_CANCEL)
		{
			std::cout << "NFD: User pressed cancel" << std::endl;
		}
		else
		{
			std::cout << "NFD Error: " << NFD_GetError() << std::endl;
		}
	}

	ImGui::End();
}

void App::update()
{
#ifdef _DEBUG
	m_pointsShaderProgram.hotloadChanges();
#endif
}

void App::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.7f, 0.65f, 0.9f, 1.0f);

	m_pointsShaderProgram.use();
	m_grid.draw();

	auto imguiIO = &ImGui::GetIO();
	glViewport(0, 0, (int)imguiIO->DisplaySize.x, (int)imguiIO->DisplaySize.y);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(m_window);
}