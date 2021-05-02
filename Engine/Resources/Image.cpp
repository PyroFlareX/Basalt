#include "Image.h"

#include "stb_image.h"

namespace vn
{
	Image::Image()	:	m_size(0, 0)
	{

	}

	Image::~Image()
	{

	}

	// Not yet implemented
	void Image::copy(const Image& src, vec2 dest)
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
		return true;
	}

	bool Image::loadFromMemory(const void* data, std::size_t size)
	{
		//	@TODO	Use stb_image
		return false;
	}

	bool Image::saveToFile(const std::string& filename)
	{
		//	@TODO	Use stb_image to save image
		return false;
	}

	vec2 Image::getSize() const
	{
		return m_size;
	}

	const u8vec4* Image::getPixelsPtr() const
	{
		return m_pixels.data();
	}

	void Image::setPixel(unsigned int x, unsigned int y, const u8vec4& color) noexcept
	{
		m_pixels[toIndex(x, y)] = color;
	}

	u8vec4 Image::getPixel(unsigned int x, unsigned int y) const noexcept
	{
		return m_pixels.at(toIndex(x, y));
	}

	void Image::flipVertically()
	{

	}

	void Image::flipHorizontally()
	{

	}

	const int Image::toIndex(const unsigned int x, const unsigned int y) const noexcept
	{
		int index = 0;

		index = m_size.x * x + y;
		if (index >= (m_size.x * m_size.y))
		{
			return -1;
		}
		return index;
	}

}