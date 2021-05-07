#pragma once

// Main
#include "../Registries/ResourceManager.h"

// Types to hold
#include "../GPU/Context.h"
#include "../Resources/Mesh.h"
#include "../Resources/Image.h"
#include "../Resources/Material.h"

namespace vn
{
	class AssetManager
	{
	public:
		AssetManager()
		{

		}

		void addTexture(vn::vk::Texture& texture, short&& id) noexcept
		{
			m_textures.addAsset(texture, id);
		}

		void addModel(vn::vk::Model& model, std::string&& id) noexcept
		{
			m_models.addAsset(model, id);
		}

		const vn::vk::Texture& getTexture(short&& id)
		{
			return m_textures.getAsset(std::forward<short>(id));
		}

		size_t getNumTextures() noexcept
		{
			return m_textures.getMap().size();
		}
		
		std::vector<vn::vk::texture_t> getTextures()
		{
			std::vector<vn::vk::texture_t> textures;

			for(auto& [key, value] : m_textures.getMap())
			{
				textures.push_back(value.getAPITextureInfo());
			}
			
			/// OR ALTERNATIVE IMPL:
			/*
			for(auto& [key, value] : m_textures.getMap())
			{
				textures[key] = value.getAPITextureInfo();
			}
			*/

			return textures;
		}

		vn::vk::Model& getModel(std::string&& id)
		{
			return m_models.getAsset(std::forward<std::string>(id));
		}

		~AssetManager()
		{

		}

		VkDescriptorSet* pDescsetglobal;
	private:

		ResourceManager<vn::Image> m_images;
		ResourceManager<vn::Mesh> m_meshes;

		ResourceManager<vn::vk::Texture, short> m_textures;
		ResourceManager<vn::vk::Model> m_models;
		ResourceManager<vn::Material> m_materials;

		
	};

	extern AssetManager asset_manager;
}