#include "Device.h"

#include "../../Types/Types.h"

namespace vn::vk
{
	class RenderTargerFramebuffer
	{
	public:
		RenderTargerFramebuffer(vn::Device device, vn::vec2 extent);

	private:
		VkFramebuffer handle;
		VkImage img;
		VkImageView imgView;

		vn::vec2 m_size;
	};
}