#include "NFDHelper.h"

#include <iostream>

#include <nfd/nfd.h>

bool NFD_ChooseFile(std::string& outFilePath)
{
	nfdchar_t* filePath = nullptr;
	nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &filePath);

	if (result == NFD_OKAY)
	{
		std::cout << "NFD: Selected file " << filePath << std::endl;
		outFilePath = filePath;
		return true;
	}
	
	if (result == NFD_CANCEL)
	{
		std::cout << "NFD: Pressed cancel" << std::endl;
		return false;
	}
	
	std::cout << "NFD Error: " << NFD_GetError() << std::endl;
	return false;
}

bool NFD_SaveFile(std::string& outFilePath)
{
	nfdchar_t* filePath = nullptr;
	nfdresult_t result = NFD_SaveDialog(nullptr, nullptr, &filePath);

	if (result == NFD_OKAY)
	{
		std::cout << "NFD: Saved file " << filePath << std::endl;
		outFilePath = filePath;
		return true;
	}

	if (result == NFD_CANCEL)
	{
		std::cout << "NFD: Pressed cancel" << std::endl;
		return false;
	}

	std::cout << "NFD Error: " << NFD_GetError() << std::endl;
	return false;
}