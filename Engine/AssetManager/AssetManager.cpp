#include "AssetManager.h"

namespace bs
{
    AssetManager* asset_manager;

    AssetManager::AssetManager()
	{

	}

	AssetManager::~AssetManager()
	{
		for(auto& [ID, texture] : m_textures.getMap())
		{
			texture.destroy();
		}

		for(auto& [ID, buffer] : m_buffers.getMap())
		{
			// std::cout << "Buffer Destroyed: " << ID << "\n";
			buffer.reset();
		}
	}

	void AssetManager::addTexture(bs::vk::Texture& texture, short id) noexcept
	{
		m_textures.addAsset(texture, id);
	}

	void AssetManager::addModel(bs::vk::Model& model, const std::string& id) noexcept
	{
		m_models.addAsset(model, id);
	}

	void AssetManager::addModel(bs::vk::Model&& model, const std::string& id) noexcept
	{
		m_models.addAsset(std::forward<bs::vk::Model>(model), id);
	}

	void AssetManager::addBuffer(std::shared_ptr<bs::vk::Buffer> buffer, const std::string& id) noexcept
	{
		m_buffers.addAsset(buffer, id);
		// std::cout << "Buffer Added: " << id << "\n";
	}

	void AssetManager::addImg(bs::Image& img, const std::string& id) noexcept
	{
		m_images.addAsset(img, id);
	}

	const bs::vk::Texture& AssetManager::getTexture(short id) const
	{
		return m_textures.getAsset(std::forward<short>(id));
	}

	bs::vk::Texture& AssetManager::getTextureMutable(short id)
	{
		return m_textures.getAsset(std::forward<short>(id));
	}

	size_t AssetManager::getNumTextures() noexcept
	{
		return m_textures.getMap().size();
	}
	
	//List of textures for pushing data to the gpu
	const std::vector<bs::vk::texture_t>& AssetManager::getTextures()
	{
		static std::vector<bs::vk::texture_t> textures;
		textures.clear();

		for(const auto& [key, value] : m_textures.getMap())
		{
			textures.emplace_back(value.getAPITextureInfo());
		}

		return textures;
	}

	bs::vk::Model& AssetManager::getModel(const std::string& id)
	{
		return m_models.getAsset(id);
	}

	std::shared_ptr<bs::vk::Buffer> AssetManager::getBuffer(const std::string& id)
	{
		return m_buffers.getAsset(id);
	}

	bs::Image& AssetManager::getImage(const std::string& id)
	{
		return m_images.getAsset(id);
	}
}