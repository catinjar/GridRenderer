#include "NodeLibrary.h"

#include <sstream>
#include <filesystem>

#include "Misc.h"
#include "NodeGraph.h"

namespace fs = std::filesystem;

NodeLibrary* NodeLibrary::instance = nullptr;

NodeLibrary* NodeLibrary::GetInstance()
{
	if (instance == nullptr)
		instance = new NodeLibrary();
	return instance;
}

void NodeLibrary::Load()
{
	nodes.clear();

	const char* libraryPath = "nodes\\";
	const char* nodeFileExtension = ".node";

	for (auto& directoryEntry : fs::recursive_directory_iterator(libraryPath))
	{
		if (directoryEntry.path().extension() == nodeFileExtension)
			LoadNode(directoryEntry.path().string());
	}
}

void NodeLibrary::LoadNode(const std::string& path)
{
	std::istringstream in(std::move(misc::ReadAllText(path)));
	std::string line;

	NodeData nodeData;

	std::getline(in, line);
	nodeData.name = line;

	while (in.peek() != EOF)
	{
		std::getline(in, line);
		const auto tokens = misc::Split(line, ' ');

		if (tokens[0] == "in")
		{
			PinData pinData;
			pinData.kind = PinKind::Input;
			pinData.type = GetShaderType(tokens[1]);
			pinData.name = tokens[2];
			nodeData.inputs.push_back(pinData);
		}
		else if (tokens[0] == "out")
		{
			PinData pinData;
			pinData.kind = PinKind::Output;
			pinData.type = GetShaderType(tokens[1]);
			pinData.name = tokens[2];
			nodeData.outputs.push_back(pinData);
		}
		else
		{
			nodeData.expression += line;

			while (in.peek() != EOF)
			{
				std::getline(in, line);
				nodeData.expression += line;
			}

			break;
		}

	}

	nodes.push_back(nodeData);
}