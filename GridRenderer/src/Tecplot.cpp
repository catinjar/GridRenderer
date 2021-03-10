#include "Tecplot.h"

#include "Misc.h"

#include <fstream>
#include <sstream>
#include <iostream>

namespace tecplot
{
	TecplotGrid Parse(const std::string& filename)
	{
		TecplotGrid grid;

		std::istringstream in(std::move(misc::ReadAllText(filename)));
		std::string line;

		int lineNumber = 0;

		while (in.peek() != EOF)
		{
			while (std::getline(in, line) && line.find("TITLE") == std::string::npos) {}

			if (in.peek() == EOF)
				break;

			// Read title
			std::cout << "TITLE" << std::endl;
			++lineNumber;

			// Read variables
			std::cout << "VARIABLES" << std::endl;
			std::getline(in, line);
			++lineNumber;

			// Read zone
			std::cout << "ZONE" << std::endl;
			std::getline(in, line);
			++lineNumber;

			misc::TrimSpacesL(line);
			misc::TrimWordL(line);
			misc::TrimSpacesL(line);
			misc::TrimWordL(line);
			misc::TrimSpacesL(line);

			uint32_t I, J, K;

			{
				std::vector<char> numberChars;
				for (char c : line)
				{
					if (!std::isdigit(c))
						break;

					numberChars.push_back(c);
				}

				I = std::stoi(std::string(numberChars.begin(), numberChars.end()));
			}

			misc::TrimNumberL(line);
			misc::TrimWordL(line);
			misc::TrimSpacesL(line);
			misc::TrimWordL(line);
			misc::TrimSpacesL(line);

			{
				std::vector<char> numberChars;
				for (char c : line)
				{
					if (!std::isdigit(c))
						break;

					numberChars.push_back(c);
				}

				J = std::stoi(std::string(numberChars.begin(), numberChars.end()));
			}

			misc::TrimNumberL(line);
			misc::TrimWordL(line);
			misc::TrimSpacesL(line);
			misc::TrimWordL(line);
			misc::TrimSpacesL(line);

			{
				std::vector<char> numberChars;
				for (char c : line)
				{
					if (!std::isdigit(c))
						break;

					numberChars.push_back(c);
				}

				K = std::stoi(std::string(numberChars.begin(), numberChars.end()));
			}

			std::vector<glm::vec3> vertices;
			std::vector<float> param1;
			std::vector<float> param2;
			std::vector<float> param3;

			int vertexCount = I * J * K;
			for (int index = 0; index < vertexCount; ++index)
			{
				std::getline(in, line);
				++lineNumber;

				float x, y, z;

				std::istringstream iss(line);

				if (!(iss >> x >> y >> z))
				{
					std::cout << "Can't parse line number " << lineNumber << " of file " << filename << std::endl;
					std::cout << line << std::endl;
					--index;
					continue;
				}

				vertices.push_back(glm::vec3(x, y, z));

				float p1;
				if (iss >> p1)
					param1.push_back(p1);

				float p2;
				if (iss >> p2)
					param2.push_back(p2);

				float p3;
				if (iss >> p3)
					param3.push_back(p3);
			}

			TecplotZone zone;
			zone.I = I;
			zone.J = J;
			zone.K = K;
			zone.vertices = std::move(vertices);
			zone.param1 = std::move(param1);
			zone.param2 = std::move(param2);
			zone.param3 = std::move(param3);

			grid.zones.push_back(zone);
		}

		return grid;
	}
}