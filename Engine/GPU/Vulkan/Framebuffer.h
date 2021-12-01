#include "Device.h"

#include "../../Types/Types.h"

namespace bs::vk
{
	struct FramebufferData
	{
		std::vector<VkFramebuffer> handle;
		VkImageView imgView;
		bs::vec2i size;
	};


	class RenderTargetFramebuffer
	{
	public:
		RenderTargetFramebuffer(bs::Device device, VkRenderPass& renderPass, bs::vec2i extent);

		FramebufferData getFramebufferData();

		~RenderTargetFramebuffer();
	private:
		VkFramebuffer handle;
		VkImage img;
		VkImageView imgView;

		VkImage depthImg;
		VkImageView depthImgView;

		VmaAllocation imgAllocation;
		VmaAllocation depthImageAllocation;

		bs::vec2 m_size;
	};
}