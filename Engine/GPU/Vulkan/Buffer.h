#pragma once

#include "../../Resources/Mesh.h"

#include "Device.h"

namespace bs::vk 
{
	//Type of buffer
	typedef enum BufferUsage
	{
		VERTEX_BUFFER = 1 << 0,
		INDEX_BUFFER = 1 << 1,
		UNIFORM_BUFFER = 1 << 2,
		STORAGE_BUFFER = 1 << 3,
		INDIRECT_BUFFER = 1 << 4,
		TRANSFER_BUFFER = 1 << 5
	} BufferUsage;

	//Describes the layout of the buffer
	struct BufferDescription
	{
		//Device that the buffer is on
		bs::Device* dev;
		//Buffer Type
		BufferUsage bufferType;
		//Number of bytes
		u64 size = 0;
		//Number of bytes between elements
		u64 stride = 0;
		//Pointer to data
		void* bufferData = nullptr;

		//BUFFER Alloc USAGE
		VmaMemoryUsage usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
	};

	//Wrapper for Vulkan Buffers
	class Buffer
	{
	public:
		Buffer(const BufferDescription bufdesc);
		~Buffer();

		//For size of subtype
		u64 getStride() const;
		//In bytes
		u64 getSize() const;
		//Get number of items in the buffer based on size and stride
		u64 getNumElements() const;

		//Change description values | Does not reallocate buffer
		void setMaxElements(const u64 numElements);
		//Change description values | Does not reallocate buffer
		void setAllocationSize(const u64 numBytes);

		//Upload Buffer to allocated space in vulkan memory, reallocates if needed
		void allocateBuffer();
		// Uses buf desc size, copies the data in ``data`` to the buffer
		void writeBuffer(const void* data, u64 size = 0, u64 offset = 0);

		//Obvious, deletes previous buffer if it is allocated
		void setAPIResource(VkBuffer& buffer);
		//Get buffer handle
		VkBuffer& getAPIResource();
		VmaAllocation& getAllocation();

		//Deallocate buffer
		void deleteBuffer();
	private:
		VkBuffer m_buffer;
		BufferDescription m_desc;
		VmaAllocation m_allocation;
	};
}