#pragma once

#include "../Camera.h"
#include "GeneralRenderer.h"
#include "UIRenderer.h"

#include <memory>

#include <Engine.h>


class Renderer
{
    public:
        Renderer(bs::Device* device);

		void drawObject(bs::GameObject& entity);
		//NOT DONE DO NOT USE
		void drawText();

		//Pass tell the subrenderers to generate list queues
		void render(Camera& cam);

		//Render to the Framebuffer
		void finish(bs::vk::FramebufferData& fbo, int index);

		//Empty drawqueues
		void clearQueue();

        ~Renderer();

		std::unique_ptr<GeneralRenderer>	m_generalRenderer;
		std::unique_ptr<UIRenderer>			m_UIRenderer;
		
		//Push the uniform buffer and image descriptor to the gpu
		void pushGPUData(Camera& cam);
    protected:

    private:
		void initGUI();

		VkPipeline imguipipeline;
		VkPipelineLayout guilayout;

		VkRenderPass renderpassdefault;
		std::vector<VkCommandBuffer> m_primaryBuffers;
		VkCommandPool m_pool;

		bs::Device* device;

		VkDescriptorPool m_descpool;
	    VkDescriptorSet m_descsetglobal;
		VkDescriptorSetLayout desclayout;
};