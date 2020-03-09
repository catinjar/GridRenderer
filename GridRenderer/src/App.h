#pragma once

#include "Grid.h"
#include "CameraController.h"
#include "ShaderProgram.h"

struct SDL_Window;

class App
{
public:
	App(SDL_Window* window);
	~App();

	void Frame();
	bool IsRunning();

private:
	void Input();
	void UI();
	void Update();
	void Render();
	
	void DrawFileUI();

	SDL_Window* m_window;

	bool m_isRunning = true;

	Grid m_grid;
	CameraController m_camera;

	ShaderProgram m_pointsShaderProgram = ShaderProgram("shaders\\points.vert", "shaders\\points.frag");
};