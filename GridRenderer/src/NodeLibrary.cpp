#include "NodeLibrary.h"

#include <sstream>

#include "Misc.h"

void NodeLibrary::Load()
{
	nodes.clear();

	const char* nodeLibraryPath = "nodes\\MultiplyVec4.node";

	std::istringstream in(std::move(misc::ReadAllText(nodeLibraryPath)));
	std::string line;

	NodeData nodeData;

	std::getline(in, line);
	nodeData.name = line;

	std::getline(in, line);
	const auto tokens = misc::Split(line, ' ');

	while (in.peek() != EOF)
	{
		std::getline(in, line);

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
			while (in.peek() != EOF)
			{
				std::getline(in, line);
				nodeData.expression += line;
			}

			break;
		}
	}
}