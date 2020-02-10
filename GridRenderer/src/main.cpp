#define SDL_MAIN_HANDLED

#include <iostream>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <nfd/nfd.h>

#include "third_party/imgui/imgui.h"
#include "third_party/imgui/imgui_impl_sdl.h"
#include "third_party/imgui/imgui_impl_opengl3.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

bool running = true;

struct Grid
{
	bool loaded = false;
	nfdchar_t* filePath = nullptr;

	void OpenFileDialog()
	{
		if (loaded)
		{
			free(filePath);
			filePath = nullptr;
			loaded = false;
		}

		nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &filePath);

		if (result == NFD_OKAY)
		{
			loaded = true;
			std::cout << filePath << std::endl;
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

	bool IsLoaded()
	{
		return loaded;
	}
};

SDL_Window* window = nullptr;
SDL_GLContext glContext = nullptr;
ImGuiIO* imguiIO = nullptr;

Grid grid;

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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	imguiIO = &ImGui::GetIO();
	
	ImGui::StyleColorsDark();
	
	ImGui_ImplSDL2_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init("#version 450");

	return true;
}

void input()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);

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
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();

	static bool showDemoWindow = true;
	ImGui::ShowDemoWindow(&showDemoWindow);

	static bool showOptionsWindow = true;
	ImGui::Begin("Options", &showOptionsWindow);

	if (grid.IsLoaded())
		ImGui::Text(grid.filePath);
	else
		ImGui::Text("No file loaded");

	if (ImGui::Button("Load .dat"))
		grid.OpenFileDialog();

	ImGui::End();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	ImGui::Render();
	glViewport(0, 0, (int)imguiIO->DisplaySize.x, (int)imguiIO->DisplaySize.y);
	glClearColor(0.7f, 0.65f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(window);
}

void cleanup()
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