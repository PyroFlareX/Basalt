#include "AssetManager.h"

namespace bs
{
    AssetManager* asset_manager;

    AssetManager::AssetManager()
	{

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

	void AssetManager::addBuffer(bs::vk::Buffer* buffer, const std::string& id) noexcept
	{
		auto bufferptr = std::shared_ptr<bs::vk::Buffer>(buffer);
		m_buffers.addAsset(bufferptr, id);
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

	bs::vk::Buffer* AssetManager::getBuffer(const std::string& id)
	{
		return m_buffers.getAsset(id).get();
	}

	bs::Image& AssetManager::getImage(const std::string& id)
	{
		return m_images.getAsset(id);
	}
}