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
		RenderTargetFramebuffer(bs::Device& device, VkRenderPass renderpass, bs::vec2i extent);
		~RenderTargetFramebuffer();
		//Return the handles to the framebuffer data
		FramebufferData getFramebufferData() const;
		//Recreate the framebuffer with the size specified
		void recreateFramebuffer(bs::vec2i size);

		VkImageView getColorImgView() const;
		VkImageView getDepthImgView() const;

	private:
		void createRenderImage();
		void createDepthAttachment();
		void createFramebuffer();

		void destroyHandles();
	
		VkFramebuffer m_handle;
		VkImage m_img;
		VkImageView m_imgView;

		VkImage m_depthImg;
		VkImageView m_depthImgView;

		VmaAllocation m_imgAllocation;
		VmaAllocation m_depthImageAllocation;

		bs::vec2i m_size;

		bs::Device& m_device;
		VkRenderPass m_renderpass;
	};
}