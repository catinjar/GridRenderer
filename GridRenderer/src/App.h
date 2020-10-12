#pragma once

#include <string>

#include "Grid.h"
#include "Material.h"
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
	void DrawFileUI();
	void DrawSettings();

	void DrawGridWindow();

	void DrawMaterialWindow();
	void RecompileShaderProgram();

	SDL_Window* window;

	bool isRunning = true;

	Grid grid;
	Material material;
	CameraController camera;

	bool isImguiDemoEnabled = false;
};