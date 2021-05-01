#pragma once

#include "../../Resources/Mesh.h"

#include "Device.h"
#include <stdexcept>

namespace vn
{
	enum BufferUsage
	{
		VERTEX_BUFFER,
		INDEX_BUFFER,
		UNIFORM_BUFFER,
		STORAGE_BUFFER,
		INDIRECT_BUFFER,

		BUFFER_TYPE_COUNT
	};
}

namespace vn::vk {

    struct BufferDescription
    {
		vn::Device* dev;
		vn::BufferUsage bufferType;
		size_t size = 0;
		size_t stride = 0;
		void* bufferData = nullptr;
    };


    class Buffer
    {
    public:
        Buffer(BufferDescription bufdesc);

        size_t getStride();
        size_t getSize();
        size_t getNumElements();

        void uploadBuffer();

        void setAPIResource(VkBuffer& buffer);
        VkBuffer& getAPIResource();

		void deleteBuffer();

        ~Buffer();
    private:
        VkBuffer m_buffer;

        BufferDescription m_desc;

        VmaAllocation m_allocation;
    };
}