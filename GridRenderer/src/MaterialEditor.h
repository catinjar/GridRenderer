#pragma once

#include "NodeGraph.h"

class MaterialEditor
{
public:
	void Init();
	void Shutdown();
	void Draw();

private:
	void ShowLeftPane(float paneWidth);

	Node* FindNode(ed::NodeId id);
	Link* FindLink(ed::LinkId id);
	Pin* FindPin(ed::PinId id);
	Link* FindLinkByPin(ed::PinId id);
	bool IsPinLinked(ed::PinId id);

	Node* SpawnFragmentShaderOutputNode();
	Node* SpawnColorNode();
	Node* SpawnMultiplyVec4();
	Node* SpawnColorToVec4();
	Node* SpawnComment();

	void BuildNodes();

	Node* GetNodeByInput(const Pin* input);
	std::string GetPinVariableName(const Pin* pin);
	void ResolveNode(const Node* node);
	void GenerateMaterialCode();

	std::vector<Node> s_Nodes;
	std::vector<Link> s_Links;
};