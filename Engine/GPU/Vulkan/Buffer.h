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
		//Number of elements
		size_t size = 0;
		//Number of bytes between elements
		size_t stride = 0;
		//Pointer to data
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
		// Uses buf desc size, copies the data in ``data`` to the buffer
		void writeBuffer(void* data);

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