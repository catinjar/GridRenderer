#include "App.h"

#include <iostream>

#include <SDL2/SDL.h>

#include "third_party/imgui/imgui.h"
#include "third_party/imgui/imgui_impl_sdl.h"
#include "third_party/imgui/imgui_impl_opengl3.h"

#include "NFDHelper.h"

App::App(SDL_Window* window)
	: m_window(window)
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
	return m_isRunning;
}

void App::Input()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);

		if (event.type == SDL_QUIT || event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE)
		{
			m_isRunning = false;
		}

		if (event.type == SDL_MOUSEWHEEL)
		{
			m_camera.Scroll(event.wheel.y);
		}
	}

	if (ImGui::GetIO().WantCaptureMouse ||
		ImGui::GetIO().WantCaptureKeyboard)
	{
		return;
	}

	m_camera.Input();
}

void App::UI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_window);
	ImGui::NewFrame();

	static bool showDemoWindow = false;
	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);

	static bool showOptionsWindow = true;
	ImGui::Begin("Options", &showOptionsWindow);

	DrawFileUI();
	m_grid.DrawUI();

	ImGui::End();
}

void App::DrawFileUI()
{
	if (ImGui::Button("Load .dat"))
	{
		std::string filePath;
		if (NFD_ChooseFile(filePath))
			m_grid.Load(filePath);
	}
}

void App::Update()
{
	m_camera.Update(m_grid);

#ifdef _DEBUG
	m_defaultShaderProgram.HotloadChanges();
#endif
}

void App::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0f, 191.0f / 255.0f, 1.0f, 1.0f);

	m_defaultShaderProgram.Use();
	m_camera.ApplyUniforms(m_defaultShaderProgram);
	m_grid.Draw(m_defaultShaderProgram);

	auto imguiIO = &ImGui::GetIO();
	glViewport(0, 0, (int)imguiIO->DisplaySize.x, (int)imguiIO->DisplaySize.y);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(m_window);
}