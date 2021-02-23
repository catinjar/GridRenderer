#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Grid.h"
#include "CameraController.h"
#include "MaterialEditor.h"

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
	void DrawSettings();
	void DrawGridList();
	void DrawGridUI();

	SDL_Window* window;

	bool isRunning = true;

	std::vector<std::unique_ptr<Grid>> grids;
	int32_t selectedGridIndex = 0;

	CameraController camera;
	MaterialEditor materialEditor;

	bool isImguiDemoEnabled = false;
};