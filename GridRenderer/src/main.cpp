#define SDL_MAIN_HANDLED

#include <iostream>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "third_party/imgui/imgui.h"
#include "third_party/imgui/imgui_impl_sdl.h"
#include "third_party/imgui/imgui_impl_opengl3.h"

#include "App.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

SDL_Window* window = nullptr;
SDL_GLContext glContext = nullptr;

bool InitSDL()
{
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

	return true;
}

void InitGlew()
{
	glewExperimental = true;
	glewInit();
}

void InitOpenGL()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_PROGRAM_POINT_SIZE);
}

void InitImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init("#version 450");
}

bool Init()
{
	if (!InitSDL())
		return false;
	
	InitGlew();
	InitOpenGL();
	InitImGui();

	return true;
}

void Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(glContext);
	glContext = nullptr;

	SDL_DestroyWindow(window);
	window = nullptr;

	SDL_Quit();
}

int main(void)
{
	if (!Init())
	{
		std::cout << "Failed to initialize!" << std::endl;
		return -1;
	}

	App app(window);

	while (app.IsRunning())
	{
		app.Frame();
	}

	Shutdown();

	return 0;
}