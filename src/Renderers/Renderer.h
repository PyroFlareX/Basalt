#ifndef RENDERER_H
#define RENDERER_H

#include "../Camera.h"
#include "GeneralRenderer.h"
#include "ComputeRenderer.h"
#include "../../Engine/Engine.h"


class Renderer
{
    public:
        Renderer(vn::Device* device);

		void drawObject(vn::GameObject& entity);
		void doCompute();
		void render(Camera& cam);
		void finish(vn::vk::FramebufferData& fbo);

		void clearQueue();

        ~Renderer();

		GeneralRenderer* m_generalRenderer;
		ComputeRenderer* m_computeRenderer;
		
		void pushGPUData();
    protected:

    private:
		VkRenderPass renderpassdefault;

		std::vector<VkCommandBuffer> m_primaryBuffers;
		VkCommandPool m_pool;

		//std::vector<vn::vk::RenderTargetFramebuffer> m_framebuffers;

		vn::Device* device;
		
};

#endif // RENDERER_H
