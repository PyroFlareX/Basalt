#include "Image.h"

#include "stb_image.h"
#include "stb_image_write.h"

#include <iostream>
#include <algorithm>

namespace bs
{
	Image::Image()	:	m_size(0, 0)
	{

	}
	
	Image::Image(const vec2i& size)	:	m_size(size)
	{
		create(m_size.x, m_size.y, u8vec4(255, 255, 255, 255));
	}

	Image::~Image()
	{

	}

	//@TODO: Implement this ASAP
	void Image::copy(const Image& src, vec2i dest)
	{

	}

	// Create an image filled with color ``color``
	void Image::create(unsigned int x, unsigned int y, const u8vec4& color)
	{
		m_size = vec2(x, y);
		std::vector<u8vec4> newImage(x * y, color);
		m_pixels.swap(newImage);
	}

	void Image::create(unsigned int x, unsigned int y, const u8vec4* pixels)
	{
		m_size = vec2(x, y);

		std::vector<u8vec4> newPixels(pixels, pixels + x * y);
		m_pixels.swap(newPixels);
	}

	bool Image::loadFromFile(const std::string& filename)
	{
		int x;
		int y;
		int channels;
		stbi_uc* pixels;

		pixels = stbi_load(filename.c_str(), &x, &y, &channels, STBI_rgb_alpha);
		if (!pixels)
		{
			return false;
		}
		
		create(x, y, (u8vec4*)pixels);

		stbi_image_free(pixels);

		return true;
	}

	bool Image::loadFromMemory(const void* data, std::size_t size)
	{
		int x = 0;
		int y = 0;
		int channels = 0;
		
		stbi_uc* pixels = stbi_load_from_memory((stbi_uc*)data, size, &x, &y, &channels, STBI_rgb_alpha);

		if (!pixels)
		{
			return false;
		}
		
		create(x, y, (u8vec4*)pixels);

		return true;
	}

	bool Image::saveToFile(const std::string& filename)
	{
		if(stbi_write_bmp(filename.c_str(), m_size.x, m_size.y, STBI_rgb_alpha, m_pixels.data()) != 0)
		{
			return false; 
		}

		return true;
	}

	vec2i Image::getSize() const
	{
		return m_size;
	}

	u8vec4* Image::getPixelsPtr()
	{
		return &m_pixels[0];
	}

	void Image::setPixel(unsigned int x, unsigned int y, const u8vec4& color) noexcept
	{
		const auto index = toIndex(x, y);
		if(index == -1)
		{
			std::cout << "issue found\n";
		}
		m_pixels[index] = color;
	}

	u8vec4 Image::getPixel(unsigned int x, unsigned int y) const noexcept
	{
		return m_pixels.at(toIndex(x, y));
	}

	void Image::flipVertically()
	{
		if (!m_pixels.empty())
		{
			std::size_t rowSize = m_size.x;

			std::vector<bs::u8vec4>::iterator top = m_pixels.begin();
			std::vector<bs::u8vec4>::iterator bottom = m_pixels.end() - rowSize;

			for (std::size_t y = 0; y < m_size.y / 2; ++y)
			{
				std::swap_ranges(top, top + rowSize, bottom);

				top += rowSize;
				bottom -= rowSize;
			}
		}
	}

	void Image::flipHorizontally()
	{
		if (!m_pixels.empty())
		{
			std::size_t rowSize = m_size.x;

			for (std::size_t y = 0; y < m_size.y; ++y)
			{
				std::vector<bs::u8vec4>::iterator left = m_pixels.begin() + y * rowSize;
				std::vector<bs::u8vec4>::iterator right = m_pixels.begin() + (y + 1) * rowSize - 1;

				for (std::size_t x = 0; x < m_size.x / 2; ++x)
				{
					std::swap_ranges(left, left + 1, right);

					left += 1;
					right -= 1;
				}
			}
		}
	}

	const int Image::toIndex(const unsigned int x, const unsigned int y) const noexcept
	{
		int index = 0;

		index = m_size.x * y + x;
		if (index >= (m_size.x * m_size.y))
		{
			return -1;
		}
		return index;
	}

}