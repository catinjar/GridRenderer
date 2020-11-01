#include "Misc.h"

#include <fstream>
#include <sstream>

namespace misc
{
	void TrimSpacesL(std::string& str)
	{
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int c) { return !std::isspace(c); }));
	}

	void TrimWordL(std::string& str)
	{
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int c) { return std::isspace(c) || std::isdigit(c); }));
	}

	void TrimNumberL(std::string& str)
	{
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int c) { return !std::isdigit(c); }));
	}


	std::string ReadAllText(const std::string &filename)
	{
		std::ifstream in(filename, std::ios::in | std::ios::binary);
		if (in)
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			return(contents);
		}
		throw(errno);
	}

	std::vector<std::string> Split(const std::string& s, char delimiter)
	{
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream tokenStream(s);
		while (std::getline(tokenStream, token, delimiter))
		{
			tokens.push_back(token);
		}
		return tokens;
	}
}