#pragma once

#include <vector>
#include <string>
#include "../Types/Types.h"

namespace bs
{
	class Image
	{
	public:
		Image();
		Image(const vec2i& size);

		~Image();

		//Copy the img src to this 
		void copy(const Image& src, vec2i dest);

		//Create simg of solid color from color
		void create(unsigned int x, unsigned int y, const u8vec4& color);
		//Create img from array of pixels
		void create(unsigned int x, unsigned int y, const u8vec4* pixels);

		//Load img from file
		bool loadFromFile(const std::string& filename);
		//Load from a pointer
		bool loadFromMemory(const void* data, std::size_t size);

		//Save to bitmap file
		bool saveToFile(const std::string& filename);

		//Get size of image
		vec2i getSize() const;
		//Get array of pixels
		u8vec4* getPixelsPtr();

		//Set pixel at destination
		void setPixel(unsigned int x, unsigned int y, const u8vec4& color) noexcept;
		//Get pixel at destination
		u8vec4 getPixel(unsigned int x, unsigned int y) const noexcept;

		void flipVertically();
		void flipHorizontally();

	private:
		vec2i m_size;
		std::vector<u8vec4> m_pixels;

		const int toIndex(const unsigned int x, const unsigned int y) const noexcept;
	};

}