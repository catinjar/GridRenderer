#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>

namespace tecplot
{
	struct TecplotZone
	{
		uint32_t I, J, K;
		std::vector<glm::vec3> vertices;
		std::vector<float> param1;
		std::vector<float> param2;
		std::vector<float> param3;
	};

	struct TecplotGrid
	{
		std::vector<TecplotZone> zones;
	};

	TecplotGrid Parse(const std::string& filename);
}