#include "Loaders.h"

std::string vn::getFileString(const std::string & filePath)
{
	std::ifstream inFile(filePath);
	if (!inFile.is_open())
	{
		throw std::runtime_error("Unable to open file: " + filePath);
	}

	std::stringstream stream;

	stream << inFile.rdbuf();
	return stream.str();
}

std::vector<char> vn::readFile(const std::string & filePath)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Unable to open file: " + filePath);
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}
