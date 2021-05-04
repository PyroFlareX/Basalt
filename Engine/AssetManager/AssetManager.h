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

        void addTexture(vn::vk::Texture&& texture, std::string&& id) noexcept
        {
            m_textures.addAsset(std::forward<vn::vk::Texture>(texture), std::forward<std::string>(id));
        }

        void addModel(vn::vk::Model&& model, std::string&& id) noexcept
        {
            m_models.addAsset(std::forward<vn::vk::Model>(model), std::forward<std::string>(id));
        }

        void addBuffer(vn::vk::Buffer&& buffer, std::string&& id) noexcept
        {
            m_buffers.addAsset(std::forward<vn::vk::Buffer>(buffer), std::forward<std::string>(id));
        }

        const vn::vk::Texture& getTexture(std::string&& id)
        {
            return m_textures.getAsset(std::forward<std::string>(id));
        }

        const vn::vk::Model& getModel(std::string&& id)
        {
            return m_models.getAsset(std::forward<std::string>(id));
        }

        const vn::vk::Buffer& getBuffer(std::string&& id)
        {
            return m_buffers.getAsset(std::forward<std::string>(id));
        }

        ~AssetManager()
        {

        }
    private:
        ResourceManager<vn::Image> m_images;
        ResourceManager<vn::Mesh> m_meshes;

        ResourceManager<vn::vk::Texture> m_textures;
        ResourceManager<vn::vk::Model> m_models;
        ResourceManager<vn::vk::Buffer> m_buffers;
        
    };

    extern AssetManager asset_manager;
}