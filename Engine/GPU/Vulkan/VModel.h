#pragma once

#include "Buffer.h"

namespace vn::vk
{
	// This is a renderable 3D mesh
	class Model
	{
	public:
		Model(vn::Mesh& mesh, vn::Device* dev)
		{
			m_mesh = mesh;
			addData(m_mesh, dev);
		}

		void addData(vn::Mesh& mesh, vn::Device* dev)
		{
			BufferDescription vertexBuffer{};
			vertexBuffer.bufferType = vn::BufferUsage::VERTEX_BUFFER;
			vertexBuffer.dev = dev;
			vertexBuffer.size = m_mesh.vertices.size();
			vertexBuffer.stride = sizeof(vn::Vertex);
			vertexBuffer.bufferData = m_mesh.vertices.data();

			BufferDescription indexBuffer{};
			indexBuffer.bufferType = vn::BufferUsage::INDEX_BUFFER;
			indexBuffer.dev = dev;
			indexBuffer.size = m_mesh.indicies.size();
			indexBuffer.stride = sizeof(unsigned int);
			indexBuffer.bufferData = m_mesh.indicies.data();
			
			m_modelbuffers.push_back(new Buffer(vertexBuffer));
			m_modelbuffers.push_back(new Buffer(indexBuffer));

			uploadMesh();
		}

		void uploadMesh()
		{
			for (auto* buffer : m_modelbuffers)
			{
				buffer->uploadBuffer();
			}
		}

		vn::vk::Buffer* getVertexBuffer()
		{
			
			return m_modelbuffers.at(0);
		}

		vn::vk::Buffer* getIndexBuffer()
		{
			return m_modelbuffers.at(1);
		}

		~Model()
		{
			for (auto& buffer : m_modelbuffers)
			{
				delete buffer;
			}
		}

	private:
		std::vector<Buffer*> m_modelbuffers;
		vn::Mesh m_mesh;
	};
}