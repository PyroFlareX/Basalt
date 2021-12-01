#include "UtilFunctions.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <algorithm>

#include <Engine.h>

namespace bs
{
	void* cast(u64 i)
	{
		return reinterpret_cast<void*>(i);
	}

	float toPercent(float input)
	{
		return input / 100.0f;
	}

	mat4 makeProjectionMatrix(const float fov, const vec2 contextSize) noexcept
	{
		return glm::perspective(glm::radians(fov), contextSize.x / contextSize.y, 0.001f, 1000.0f);
	}

	const mat4 makeViewMatrix(const Transform& camera) noexcept
	{
		mat4 matrix = mat4(1.0f);

		matrix = glm::rotate(matrix, glm::radians(camera.rot.x), { 1, 0, 0 });
		matrix = glm::rotate(matrix, glm::radians(camera.rot.y), { 0, 1, 0 });
		matrix = glm::rotate(matrix, glm::radians(camera.rot.z), { 0, 0, 1 });

		matrix = glm::translate(matrix, -camera.pos);

		return matrix;
	}

	const mat4 makeModelMatrix(const Transform& entity) noexcept
	{
		mat4 matrix = mat4(1.0f);

		matrix = glm::rotate(matrix, glm::radians(entity.rot.x), { 1, 0, 0 });
		matrix = glm::rotate(matrix, glm::radians(entity.rot.y), { 0, 1, 0 });
		matrix = glm::rotate(matrix, glm::radians(entity.rot.z), { 0, 0, 1 });

		matrix = glm::scale(matrix, entity.scale);

		matrix = glm::translate(matrix, entity.pos);


		return matrix;
	}

	bool isNumber(const char c)
	{
		switch (c)
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				return true;
				break;
			default:
				return false;
		}
	}

	const std::string WHITESPACE = " \n\r\t\f\v";
 
	std::string ltrim(const std::string &s)
	{
		size_t start = s.find_first_not_of(WHITESPACE);
		return (start == std::string::npos) ? "" : s.substr(start);
	}
	
	std::string rtrim(const std::string &s)
	{
		size_t end = s.find_last_not_of(WHITESPACE);
		return (end == std::string::npos) ? "" : s.substr(0, end + 1);
	}
	
	std::string trim(const std::string &s) {
		return rtrim(ltrim(s));
	}

	std::string trimChar(const std::string& inputStr, const char* chars)
	{
		std::string str = inputStr;
		for (int i = 0; i <= strlen(chars); ++i) {
			str.erase(std::remove(str.begin(), str.end(), chars[i]), str.end());
		}
		return str;
	}

	std::vector<std::string> splitString(const std::string& str, std::string delim)
	{
		std::vector<std::string> strArr;

		for (uint16_t j = 0; j < delim.length(); ++j) {

			uint16_t pos = 0;
			uint16_t prevPos = 0;
			char prevChar = '0';

			for (uint16_t i = 0; i <= str.length(); ++i) {
				if (str[i] == delim[j] || (i == str.length() && prevPos > 0)) {
					// if (prevChar == delim[j] && str[i] == delim[j]) {
					// 	pos++; 
					// 	prevChar++;
					// 	prevChar = str[i];
					// 	continue;
					// }
					strArr.push_back(trim(str.substr(prevPos, pos)));
					prevPos = i+1;
					pos = 0;
					prevChar = str[i];
					continue;
				}
				else {
					pos++;
					prevChar = str[i];
					continue;
				}
			}
		}

		return strArr;
	}

	void splitString(const std::string& str, const char& delim, std::vector<std::string>* ptr)
	{
		uint8_t pos = 0;
		uint8_t prevPos = 0;

		for (int i = 0; i <= str.length(); ++i) {
			if (str[i] == delim || (i == str.length() && prevPos > 0)) {
				ptr->push_back(str.substr(prevPos, pos));
				prevPos = i+1;
				pos = 0;
			}
			else {
				pos++;
				continue;
			}
		}
	}

	std::vector<std::string> splitStringAfterBracket(const std::string& str, std::string delim)
	{
		std::string bracketed = "";
		if (str.find('{') != str.npos) {
			bracketed = trim(str.substr(str.find('{')+1, str.length()-str.find('{')));
		}
		else {
			bracketed = str;
		}

		std::vector<std::string> strArr;

		for (uint16_t j = 0; j < delim.length(); ++j) {

			uint16_t pos = 0;
			uint16_t prevPos = 0;
			char prevChar = '0';

			for (uint16_t i = 0; i <= bracketed.length(); ++i) {
				if (bracketed[i] == delim[j] || (i == str.length() && prevPos > 0)) {
					// if (prevChar == delim[j] && bracketed[i] == delim[j]) {
					// 	pos++; 
					// 	prevChar++;
					// 	prevChar = bracketed[i];
					// 	continue;
					// }
					strArr.push_back(trim(bracketed.substr(prevPos, pos)));
					prevPos = i+1;
					pos = 0;
					prevChar = bracketed[i];
					continue;
				}
				else {
					pos++;
					prevChar = bracketed[i];
					continue;
				}
			}
		}

		return strArr;
	}

	std::vector<std::string> splitStringBetweenBrackets(const std::string& str, std::string delim)
	{
		std::string bracketed = trim(str.substr(str.find('{')+1, str.find('}')-str.find('{')-1));

		std::vector<std::string> strArr;

		for (uint16_t j = 0; j < delim.length(); ++j) 
		{
			uint16_t pos = 0;
			uint16_t prevPos = 0;
			char prevChar = '0';
			
			for (uint16_t i = 0; i <= bracketed.length(); ++i) 
			{
				if (bracketed[i] == delim[j] || i == bracketed.length()) {
					// if (prevChar == delim[j] && bracketed[i] == delim[j]) {
					// 	pos++; 
					// 	prevChar++;
					// 	prevChar = bracketed[i];
					// 	continue;
					// }
					strArr.push_back(trim(bracketed.substr(prevPos, pos)));
					std::cout << "LINE: " << bracketed.substr(prevPos, pos) << std::endl;
					prevPos = i+1;
					pos = 0;
					prevChar = bracketed[i];
					continue;
				}
				else 
				{
					pos++;
					prevChar = bracketed[i];
					continue;
				}
			}
		}
		return strArr;
	}

	// void splitStringBetweenBrackets(const std::string& str, const char* delim, char a, char b, char c, char d) //backup
	// {
	// 	uint8_t pos = 0;
	// 	uint8_t prevPos = 0;
	// 	char prevChar = '0';

	// 	std::vector<std::string> strArr;

	// 	std::string bracketed = trim(str.substr(str.find('{')+1, str.find('}')-str.find('{')-1));

	// 	for (int i = 0; i <= bracketed.length(); ++i) { //temp delim 0 till i can regain my sanity to rewrite this
	// 		if (bracketed[i] == delim[0] || i == bracketed.length()) {
	// 			if (prevChar == delim[0] && bracketed[i] == delim[0]) {
	// 				pos++; 
	// 				prevChar++;
	// 				prevChar = bracketed[i];
	// 				continue;
	// 			}

	// 			strArr.push_back(trim(bracketed.substr(prevPos, pos)));
	// 			prevPos = i+1;
	// 			pos = 0;
	// 		}
	// 		else {
	// 			pos++;
	// 			prevChar = bracketed[i];
	// 			continue;
	// 		}
	// 		prevChar = bracketed[i];
	// 	}

	// 	//return strArr;
	// }


	const void printVec(const vec2& vec, const std::string prefix)
	{
		std::cout << prefix << "X: " << vec.x << "Y: " << vec.y << std::endl;
	}

	const void printVec(const vec3& vec, const std::string prefix)
	{
		std::cout << prefix << "X: " << vec.x << "Y: " << vec.y << "Z: " << vec.z << std::endl;
	}

	const void printVec(const vec4& vec, const std::string prefix)
	{
		std::cout << prefix << "X: " << vec.x << "Y: " << vec.y << "Z: " << vec.z << "W:" << vec.w << std::endl;
	}

}