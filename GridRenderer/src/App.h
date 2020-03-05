#pragma once

#include "Grid.h"
#include "ShaderProgram.h"

struct SDL_Window;

class App
{
public:
	App(SDL_Window* window);
	~App();

	void frame();
	bool isRunning();

private:
	void input();
	void ui();
	void update();
	void render();
	
	SDL_Window* m_window;

	bool m_isRunning = true;

	Grid m_grid;
	ShaderProgram m_pointsShaderProgram = ShaderProgram("shaders\\points.vert", "shaders\\points.frag");
};