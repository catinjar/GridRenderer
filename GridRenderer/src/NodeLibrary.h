#pragma once

#include <string>
#include <vector>

#include "UniformParam.h"

enum PinKind;

struct PinData
{
	std::string name;
	PinKind kind;
	ShaderDataType type;
};

struct NodeData
{
	std::string id;
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

	NodeLibrary(NodeLibrary& other) = delete;

	void operator=(const NodeLibrary&) = delete;

	static NodeLibrary* GetInstance();

	NodeData* Get(std::string id)
	{
		for (auto& node : nodes)
		{
			if (node.id == id)
				return &node;
		}

		return nullptr;
	}

private:
	NodeLibrary()
	{
		Load();
	}
	
	static NodeLibrary* instance;
};

