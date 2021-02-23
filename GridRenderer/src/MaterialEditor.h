#pragma once

#include "Material.h"
#include "NodeGraph.h"

class MaterialEditor
{
public:
	void Init(Material* material, NodeGraph* graph);
	void SetMaterial(Material* material, NodeGraph* graph);
	void Shutdown();
	void Draw();

private:
	void ShowLeftPane(float paneWidth);

	Node* SpawnFragmentShaderOutputNode();
	Node* SpawnColorNode();
	Node* SpawnMultiplyVec4();
	Node* SpawnColorToVec4();
	Node* SpawnComment();

	void BuildNodes();

	Material* material;
	NodeGraph* graph;
};