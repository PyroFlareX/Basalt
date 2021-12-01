#pragma once

#include "../../Resources/Mesh.h"

#include "Device.h"

#include <stdexcept>
#include <memory>

namespace bs::vk 
{
	//Type of buffer
	enum BufferUsage
	{
		VERTEX_BUFFER,
		INDEX_BUFFER,
		UNIFORM_BUFFER,
		STORAGE_BUFFER,
		INDIRECT_BUFFER,
		TRANSFER_BUFFER,

		BUFFER_TYPE_COUNT
	};

	//Describes the layout of the buffer
	struct BufferDescription
	{
		//Device that the buffer is on
		bs::Device* dev;
		//Buffer Type
		BufferUsage bufferType;
		//Number of bytes
		size_t size = 0;
		//Number of bytes between elements
		size_t stride = 0;
		//Pointer to data
		void* bufferData = nullptr;
	};

	//Wrapper for Vulkan Buffers
	class Buffer
	{
	public:
		Buffer(BufferDescription bufdesc);

		size_t getStride();	//For size of subtype
		size_t getSize(); //In bytes
		size_t getNumElements();	//Number of indices

		//Upload Buffer to allocated space in vulkan memory
		void uploadBuffer(bool write = true);
		// Uses buf desc size, copies the data in ``data`` to the buffer
		void writeBuffer(void* data, size_t size = 0, size_t offset = 0);

		//Obvious
		void setAPIResource(VkBuffer& buffer);
		//Get buffer
		VkBuffer& getAPIResource();

		//Deallocate buffer
		void deleteBuffer();

		//Change description values | Does not reallocate buffer
		void setMaxElements(size_t numElements);

		~Buffer();
	private:
		VkBuffer m_buffer;

		BufferDescription m_desc;

		VmaAllocation m_allocation;
	};
}