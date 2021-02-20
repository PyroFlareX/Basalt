#pragma once

#include "../glad/glad/glad.h"
#include "Image.h"

namespace vn
{
	class Texture
	{
	public:
		Texture()
		{
			glGenTextures(1, &ID);
			glBindTexture(GL_TEXTURE_2D, ID);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		Texture(const std::string& filepath)
		{
			glGenTextures(1, &ID);
			glBindTexture(GL_TEXTURE_2D, ID);
			glBindTexture(GL_TEXTURE_2D, 0);

			this->loadFromFile(filepath);
		}

		void loadFromFile(const std::string& filepath)
		{
			Image i;
			i.loadFromFile(filepath);

			bind();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i.getSize().x, i.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, i.getPixelsPtr());

			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1);
		}

		void loadFromImage(const Image i)
		{
			bind();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i.getSize().x, i.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, i.getPixelsPtr());

			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1);
		}

		const void bind()
		{
			glBindTexture(GL_TEXTURE_2D, ID);
		}

		~Texture()
		{
			glDeleteTextures(1, &ID);
		}

		unsigned int ID = 0;
	private:

	};


}