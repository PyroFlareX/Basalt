#pragma once

#include "../Types/Types.h"

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace vn
{
	struct Mesh;

	
	std::string getFileString(const std::string& filePath);
	

	std::vector<char> readFile(const std::string& filePath);
	

	Mesh loadMeshFromObj(const std::string& filepath);

}