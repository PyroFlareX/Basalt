#pragma once

#include "../../Resources/Mesh.h"

#include "Device.h"
#include <stdexcept>

namespace vn {
	enum BufferUsage {
		VERTEX_BUFFER,
		INDEX_BUFFER,
		UNIFORM_BUFFER,
		STORAGE_BUFFER,
		INDIRECT_BUFFER,

		BUFFER_TYPE_COUNT
	};
}

namespace vn::vk {

	struct BufferDescription {
		vn::Device *dev;
		vn::BufferUsage bufferType;
		unsigned int size = 0;
		unsigned int stride = 0;
		void *bufferData = nullptr;
	};


	class Buffer {
	public:
		Buffer(BufferDescription bufdesc);

		unsigned int getStride();

		unsigned int getSize();

		unsigned int getNumElements();

		void uploadBuffer();

		void setAPIResource(VkBuffer &buffer);

		VkBuffer &getAPIResource();

		void deleteBuffer();

		~Buffer();

	private:
		VkBuffer m_buffer;

		BufferDescription m_desc;

		VmaAllocation m_allocation;
	};
}