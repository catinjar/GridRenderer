#pragma once

#include <string>

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
	void DrawFileUI();
	void DrawSettings();

	void DrawGridWindow();

	void DrawMaterialWindow();
	void RecompileShaderProgram();

	SDL_Window* window;

	bool isRunning = true;

	Grid grid;
	CameraController camera;

	bool isHotloadEnabled = true;
	bool isImguiDemoEnabled = false;

	std::string vertexShaderFilename = "shaders\\default.vert";
	std::string fragmentShaderFilename = "shaders\\default.frag";

	ShaderProgram shaderProgram = ShaderProgram(vertexShaderFilename, fragmentShaderFilename);
};