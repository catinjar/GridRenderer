#pragma once

#include "Material.h"
#include "NodeGraph.h"
#include "NodeLibrary.h"

class MaterialEditor
{
public:
	void Init(Material* material, NodeGraph* graph);
	void SetMaterial(Material* material, NodeGraph* graph);
	void Shutdown();
	void Draw();

private:
	void BuildNodes();

	void ShowLeftPane(float paneWidth);

	Node* DrawInputNodesMenu();
	Node* DrawInputNodeMenuItem(const char* name, ShaderDataType type);

	Node* SpawnVertexShaderOutputNode();
	Node* SpawnFragmentShaderOutputNode();
	Node* SpawnInputNode(const char* name, ShaderDataType type);
	Node* SpawnMultiplyVec4();
	Node* SpawnColorToVec4();
	Node* SpawnComment();

	Material* material;
	NodeGraph* graph;

	NodeLibrary nodeLibrary;
};