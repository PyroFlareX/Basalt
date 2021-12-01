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
		Image(const vec2i& size, const u8vec4& color);
		Image(const vec2i& size, const u8vec4* pixels);

		~Image() = default;

		//Copy the img src to this 
		void copy(const Image& src, vec2i dest);

		//Create simg of solid color from color
		void create(u32 x, u32 y, const u8vec4& color);
		//Create img from array of pixels
		void create(u32 x, u32 y, const u8vec4* pixels);

		//Load img from file
		bool loadFromFile(const std::string& filename);
		//Load from a pointer
		bool loadFromMemory(const void* data, std::size_t size);

		//Save to bitmap file
		bool saveToFile(const std::string& filename) const;

		//Get size of image
		vec2i getSize() const;
		//Get array of pixels
		u8vec4* getPixelsPtr();
		const u8vec4* getPixelsPtr() const;

		//Set pixel at destination
		void setPixel(u32 x, u32 y, const u8vec4& color) noexcept;
		//Get pixel at destination
		u8vec4 getPixel(u32 x, u32 y) const noexcept;
		//Flip everything along the horzontal axis, from up to down
		void flipVertically();
		//Flip everything along the vertical axis, from left to right
		void flipHorizontally();

	private:
		vec2i m_size;
		std::vector<u8vec4> m_pixels;

		const int toIndex(u32 x, u32 y) const noexcept;
	};
}