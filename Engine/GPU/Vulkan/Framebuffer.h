#include "Device.h"

#include "../../Types/Types.h"

namespace vn::vk {
	struct FramebufferData {
		std::vector<VkFramebuffer> handle;
		VkImageView imgView;
		vn::vec2 size;
	};


	class RenderTargetFramebuffer {
	public:
		RenderTargetFramebuffer(vn::Device device, VkRenderPass &renderPass, vn::vec2 extent);

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

		vn::vec2 m_size;
	};
}