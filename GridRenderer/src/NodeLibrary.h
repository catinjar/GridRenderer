#pragma once

#include <string>
#include <vector>

#include "UniformParam.h"
#include "NodeGraph.h"

struct PinData
{
	std::string name;
	PinKind kind;
	ShaderDataType type;
};

struct NodeData
{
	std::string name;
	std::string expression;
	std::vector<PinData> inputs;
	std::vector<PinData> outputs;
};

struct NodeLibrary
{
	void Load();
	void LoadNode(const std::string& path);

	std::vector<NodeData> nodes;
};