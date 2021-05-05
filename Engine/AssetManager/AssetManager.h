#pragma once

// Main
#include "../Registries/ResourceManager.h"

// Types to hold
#include "../GPU/Context.h"
#include "../Resources/Mesh.h"
#include "../Resources/Image.h"

namespace vn
{
    class AssetManager
    {
    public:
        AssetManager()
        {

        }

        void addTexture(vn::vk::Texture& texture, std::string&& id) noexcept
        {
            m_textures.addAsset(texture, id);
        }

        void addModel(vn::vk::Model& model, std::string&& id) noexcept
        {
            m_models.addAsset(model, id);
        }

        const vn::vk::Texture& getTexture(std::string&& id)
        {
            return m_textures.getAsset(std::forward<std::string>(id));
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

        ResourceManager<vn::vk::Texture> m_textures;
        ResourceManager<vn::vk::Model> m_models;

        
    };

    extern AssetManager asset_manager;
}