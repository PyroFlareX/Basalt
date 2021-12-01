#pragma once

// Main
#include <Registries/ResourceManager.h>

// Types to hold
#include <GPU/Context.h>
#include <Resources/Mesh.h>
#include <Resources/Image.h>
#include <Resources/Material.h>


// @TODO: Possibly templatify this into 4 functions? Get, Add, GetMap, DoesExist?


namespace bs
{
	class AssetManager
	{
	public:
		AssetManager();

		void addTexture(bs::vk::Texture& texture, short id) noexcept;

		void addModel(bs::vk::Model& model, const std::string& id) noexcept;
		void addModel(bs::vk::Model&& model, const std::string& id) noexcept;

		void addBuffer(bs::vk::Buffer* buffer, const std::string& id) noexcept;

		void addImg(bs::Image& img, const std::string& id) noexcept;

		const bs::vk::Texture& getTexture(short id) const;

		bs::vk::Texture& getTextureMutable(short id);

		size_t getNumTextures() noexcept;
		
		//List of textures for pushing data to the gpu
		const std::vector<bs::vk::texture_t>& getTextures();

		bs::vk::Model& getModel(const std::string& id);

		bs::vk::Buffer* getBuffer(const std::string& id);

		bs::Image& getImage(const std::string& id);

		
		//Store globally necessary pointers
		VkDescriptorSet*	pDescsetglobal;
		
		bool				loaded = false;
		bool				loadedarray[8] = { false, false, false, false, false, false, false, false };

	private:

		ResourceManager<bs::Image> m_images;
		ResourceManager<bs::Mesh> m_meshes;

		ResourceMap<bs::vk::Texture> m_textures;	//Ordered Map
		ResourceManager<bs::vk::Model> m_models;
		ResourceManager<bs::Material> m_materials;
		ResourceManager<std::shared_ptr<bs::vk::Buffer>> m_buffers;

	};

	extern AssetManager* asset_manager;
}