#pragma once

#include <string>
#include <vector>

namespace bs
{
	struct Mesh;

	//Load file as a string
	std::string getFileString(const std::string& filePath);
	
	//Read file in bytes, used mainly for loading SPIR-V shaders
	std::vector<char> readFile(const std::string& filePath);
	
	//Load Mesh from obj file| not yet impled to load materials and such
	Mesh loadMeshFromObj(const std::string& filepath);

}