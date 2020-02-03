#define SDL_MAIN_HANDLED

#include <iostream>

#include <glew/glew.h>
#include <SDL2/SDL.h>

#include "third_party/imgui/imgui.h"
#include "third_party/imgui/imgui_impl_sdl_gl3.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

bool running = true;

SDL_Window* window = nullptr;
SDL_GLContext glContext = nullptr;

bool init()
{
	bool success = true;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}

	window = SDL_CreateWindow("Grid Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

	if (window == nullptr)
	{
		std::cout << "Failed to create window! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}
	
	glContext = SDL_GL_CreateContext(window);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetSwapInterval(1);

	glewExperimental = true;
	glewInit();

	glClearColor(0.7f, 0.65f, 0.9f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	ImGui::CreateContext();
	ImGui_ImplSdlGL3_Init(window);

	return true;
}

void input()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSdlGL3_ProcessEvent(&event);

		if (event.type == SDL_QUIT ||
			event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE)
		{
			running = false;
		}
	}

	if (ImGui::GetIO().WantCaptureMouse ||
		ImGui::GetIO().WantCaptureKeyboard)
	{
		return;
	}
}

void update()
{

}

void UI()
{
	ImGui_ImplSdlGL3_NewFrame(window);

	ImGui::Begin("Options");

	ImGui::End();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	ImGui::Render();
	ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(window);
}

void cleanup()
{
	ImGui_ImplSdlGL3_Shutdown();

	SDL_GL_DeleteContext(glContext);
	glContext = nullptr;

	SDL_DestroyWindow(window);
	window = nullptr;

	SDL_Quit();
}

int main(void)
{
	if (!init())
	{
		std::cout << "Failed to initialize!" << std::endl;
		return -1;
	}

	while (running)
	{
		input();
		UI();
		update();
		render();
	}

	cleanup();

	return 0;
}