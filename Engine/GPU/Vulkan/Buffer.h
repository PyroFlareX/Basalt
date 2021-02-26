#pragma once

#include "../../Resources/Mesh.h"

#include "Device.h"
#include <stdexcept>

namespace vn::vk {
    struct BufferDescription
    {
        vn::Mesh m_mesh;

        vn::Device dev;
    };

    


    class Buffer
    {
    public:
        Buffer(BufferDescription bufdesc);

        unsigned int getStride();
        unsigned int getSize();
        unsigned int getNumElements();

        void uploadMesh();

        void setAPIResource(VkBuffer buffer);
        VkBuffer& getAPIResource();

        VkBuffer m_index;

        ~Buffer();
    private:
        VkBuffer m_buffer;
        //VkBuffer m_index;

        BufferDescription m_desc;

        VmaAllocation m_allocation;
        VmaAllocation m_indexAlloc;
    };
}