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
	
	void DrawOptionsWindow();
	void DrawGridWindow();

	void DrawFileUI();
	void DrawSettings();

	SDL_Window* window;

	bool isRunning = true;

	Grid grid;
	CameraController camera;

	bool isHotloadEnabled = true;
	bool isImguiDemoEnabled = false;

	ShaderProgram defaultShaderProgram = ShaderProgram("shaders\\points.vert", "shaders\\points.frag");
};