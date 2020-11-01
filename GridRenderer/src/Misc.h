#pragma once

#include <string>
#include <vector>

namespace misc
{
	void TrimSpacesL(std::string& str);
	void TrimWordL(std::string& str);
	void TrimNumberL(std::string& str);

	std::string ReadAllText(const std::string &filename);
	std::vector<std::string> Split(const std::string& s, char delimiter);
}