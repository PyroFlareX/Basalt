#include "Loaders.h"

#include "../Resources/Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../Resources/tiny_obj_loader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "../Types/Types.h"

std::string bs::getFileString(const std::string & filePath)
{
	std::ifstream inFile(filePath);
	if (!inFile.is_open())
	{
		throw std::runtime_error("Unable to open file: " + filePath);
	}

	std::stringstream stream;

	stream << inFile.rdbuf();
	return stream.str();
}

std::vector<char> bs::readFile(const std::string& filePath)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Unable to open file: " + filePath);
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	/*std::ofstream f(filePath + ".h");
	f.clear();
	f << "static char " << "array[]" << " = {\n\t";
	int i = 0;
	bool start = true;
	for(const auto c : buffer)
	{
		if(start)
		{
			start = false;
			f << (int)c;
			continue;
		}
		else
		{
			f << ", ";
		}

		f << (int)c;
		i += 1;
		if(i >= 5)
		{
			i = 0;
			f << "\n\t";
		}
	}
	f << std::dec << "\n};\n";
	f.close();*/

	return buffer;
}

bs::Mesh bs::loadMeshFromObj(const std::string& filepath)
	{
		{
			tinyobj::attrib_t attributes;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
			std::string errors;
			Mesh objMesh;
			
			if (!tinyobj::LoadObj(&attributes, &shapes, &materials, &errors, filepath.c_str()))
			{
				std::cout << errors << std::endl;
			}
			/*unsigned int indices = 0;
			for (const auto& shape : shapes)
			{
				for (const auto& index : shape.mesh.indices)
				{
					Vertex vert;
					
					vert.position = 
					{
						attributes.vertices[3 * index.vertex_index + 0],
						attributes.vertices[3 * index.vertex_index + 1],
						attributes.vertices[3 * index.vertex_index + 2]
					};
					vert.uv =
					{
						attributes.texcoords[2 * index.texcoord_index + 0],
						attributes.texcoords[2 * index.texcoord_index + 1]
					};
					
					vert.normal =
					{
						attributes.normals[3 * index.normal_index + 0],
						attributes.normals[3 * index.normal_index + 1],
						attributes.normals[3 * index.normal_index + 2]
					};
					
					objMesh.vertices.push_back(vert);
					objMesh.indicies.push_back(++indices);
					indices++;
				}
			}*/

			// Loop over shapes
			for (size_t s = 0; s < shapes.size(); s++) {
				// Loop over faces(polygon)
				size_t index_offset = 0;
				for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
					int fv = shapes[s].mesh.num_face_vertices[f];

					// Loop over vertices in the face.
					for (size_t v = 0; v < fv; v++) {
						// access to vertex

						tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

						Vertex vert;

						vert.position = vec3
						{
						attributes.vertices[3 * idx.vertex_index + 0],
						attributes.vertices[3 * idx.vertex_index + 1],
						attributes.vertices[3 * idx.vertex_index + 2] 
						};

						if (!attributes.normals.empty())
						{
							vert.normal = vec3
							{
							attributes.normals[3 * idx.normal_index + 0],
							attributes.normals[3 * idx.normal_index + 1],
							attributes.normals[3 * idx.normal_index + 2]
							};
						}
						else
						{
							vert.normal = vec3(0.0f);
						}

						if (!attributes.texcoords.empty())
						{
							vert.uv = vec2
							{
							attributes.texcoords[2 * idx.texcoord_index + 0],
							attributes.texcoords[2 * idx.texcoord_index + 1]
							};
						}
						else
						{
							vert.uv = vec2(0.0f);
						}

						objMesh.vertices.push_back(vert);
						objMesh.indicies.push_back(objMesh.indicies.size());

						// Optional: vertex colors
						// tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
						// tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
						// tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
					}
					index_offset += fv;

					// per-face material
					//shapes[s].mesh.material_ids[f];
				}
			}

			return objMesh;
		}
	}