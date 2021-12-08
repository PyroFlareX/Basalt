#pragma once
#pragma warning (disable: 4267 4244 4005) // size_t to int conversion, to float conversion

#include <cctype>
#include <functional>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;
	
typedef int8_t		i8;
typedef int16_t		i16;
typedef int32_t		i32;
typedef int64_t		i64;

namespace bs
{
	typedef glm::vec2	vec2;
	typedef glm::vec3	vec3;
	typedef glm::vec4	vec4;

	typedef glm::ivec2	vec2i;
	typedef glm::ivec3	vec3i;
	typedef glm::ivec4	vec4i;

	typedef glm::u8vec2	u8vec2;
	typedef glm::u8vec3	u8vec3;
	typedef glm::u8vec4	u8vec4;
	
	typedef glm::mat4	mat4;
	typedef glm::mat3	mat3;

	typedef struct ray_t
	{
		vec3 start;
		vec3 direction;
	}	Ray;

	//A Transform, stores the information for 3d orientation
	class Transform
	{
	public:
		Transform() : pos(0.0f, 0.0f, 0.0f), rot(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), origin(0.0f, 0.0f, 0.0f)	{	}
		
		vec3 pos;
		vec3 rot;
		vec3 scale;
		vec3 origin;

		static Transform& translate(Transform& entity, const vec3& offset)
		{
			entity.pos += offset;
			return entity;
		}

		static Transform& rotate(Transform& entity, const vec3& angle)
		{
			entity.rot += angle;
			return entity;
		}

		static Transform& rescale(Transform& entity, const vec3& relativeScale)
		{
			entity.scale = entity.scale * relativeScale;
			return entity;
		}
	};
}

//For hashing (e.g. usage in std::maps)
namespace std
{
	//vec3
	template<>
	struct hash<bs::vec3>
	{
		size_t operator()(const bs::vec3& vect) const noexcept
		{
			std::hash<decltype(vect.x)> hasher;

			auto hash1 = hasher(vect.x);
			auto hash2 = hasher(vect.y);
			auto hash3 = hasher(vect.z);

			return std::hash<decltype(vect.x)>{}((hash1 ^ (hash2 << hash3) ^ hash3));
		}
	};

	//vec3i
	template<>
	struct hash<bs::vec3i>
	{
		size_t operator()(const bs::vec3i& vec) const noexcept
		{
			std::hash<decltype(vec.x)> hasher;

			auto hash1 = hasher(vec.x);
			auto hash2 = hasher(vec.y);
			auto hash3 = hasher(vec.z);

			return std::hash<decltype(vec.x)>{}((hash1 ^ (hash2 << hash3) ^ hash3));
		}
	};

	//vec2
	template<>
	struct hash<bs::vec2>
	{
		size_t operator()(const bs::vec2& vec) const noexcept
		{
			std::hash<decltype(vec.x)> hasher;

			auto hash1 = hasher(vec.x);
			auto hash2 = hasher(vec.y);

			return std::hash<decltype(vec.x)>{}(hash1 ^ (hash2 << 1));
		}
	};

	//vec2i
	template<>
	struct hash<bs::vec2i>
	{
		size_t operator()(const bs::vec2i& vec) const noexcept
		{
			std::hash<decltype(vec.x)> hasher;

			auto hash1 = hasher(vec.x);
			auto hash2 = hasher(vec.y);

			return std::hash<decltype(vec.x)>{}(hash1 ^ (hash2 << 1));
		}
	};
}	