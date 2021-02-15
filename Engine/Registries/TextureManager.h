#pragma once

#include "../Resources/Image.h"

#include <vector>
#include <unordered_map>
#include <array>

namespace vn
{
	class TextureManager
	{
	public:
		TextureManager();

		void addImg(Image img, const std::string& texName);

		Image& getSheet();
		std::array<float, 8> getTexCoords(const std::string& texName);

		~TextureManager();
	private:
		void buildTextureSheet();

		std::unordered_map<std::string, std::array<float, 8>> m_blockTexCoordsMap;
		std::vector<std::pair<Image, std::string>> imgs;
		Image sheet;
	};


}