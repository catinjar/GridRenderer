#pragma once

#include <string>

namespace misc
{
	void TrimSpacesL(std::string& str);
	void TrimWordL(std::string& str);
	void TrimNumberL(std::string& str);

	std::string ReadAllText(const std::string &filename);
}