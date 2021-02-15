#pragma once

#include "../../Texture/Texture.h"
#include <vector>
#include <unordered_map>
#include <array>
//#include "../../mesh.h"
//#include "../World/Block/Block.h"

class TextureManager
{
public:
	TextureManager();

	void addImg(sf::Image img, const std::string& texName);

	sf::Image& getSheet();
	std::array<float, 8> getTexCoords(const std::string& texName);

	~TextureManager();
private:
	void buildTextureSheet();

	std::unordered_map<std::string, std::array<float, 8>> m_blockTexCoordsMap;
	std::vector<std::pair<sf::Image, std::string>> imgs;
	sf::Image sheet;
};

/*class BlockManager
{
public:
	BlockManager();

	void addBlock(Block& block, const std::string& blockName);

	Block& getBlock(const std::string& blockName);

	~BlockManager();
private:

	std::unordered_map<std::string, Block> m_blockMap;
};
*/
#ifndef _res
#define _res
namespace resources
{
	extern TextureManager TexManager;
	//extern BlockManager BlockDatabase;
}
#endif