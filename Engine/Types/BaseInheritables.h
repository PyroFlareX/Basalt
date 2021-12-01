#pragma once

#include "nlohmann/json.h"

namespace bs
{
	//some typedefs
	using json = nlohmann::json;

	//Traits: These are traits that can be applied to objects, and have "some" default behavior that is meant to be overriden
	//List:
	//      Serializable
	//      Deserializable
	//		Serde (serial / deserial)
	//      NonCopyable
	//      NonMovable
	//      Singleton

	//Serializable: Trait that is used to serialize the data of an object into a JSON
	class Serializable
	{
	public:
		virtual json serialize() const = 0;
	};

	//Deserializable: Trait that is used to deserialize the data of an object from a JSON
	class Deserializable
	{
	public:
		virtual void deserialize(const json& j) = 0;
	};

	//Serde: Trait that is used to both serialize and deserialize the data of an object to/from a JSON
	class Serde : public Serializable, public Deserializable
	{

	};

	//NonCopyable
	class NonCopyable
	{
	public:
		NonCopyable() = default;
		NonCopyable(const NonCopyable&) = delete;
		NonCopyable& operator=(const NonCopyable&) = delete;

		NonCopyable(NonCopyable&&) = default;
		NonCopyable& operator=(NonCopyable&&) = default;

		~NonCopyable() = default;
	};

	//NonMovable
	class NonMovable
	{
	public:
		NonMovable() = default;
		NonMovable(NonMovable&&) = delete;
		NonMovable& operator=(NonMovable&&) = delete;

		NonMovable(const NonMovable&) = default;
		NonMovable& operator=(const NonMovable&) = default;

		~NonMovable() = default;
	};

	//Singleton
	class Singleton
	{
	public:
		Singleton() = default;
		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;
		Singleton(Singleton&&) = delete;
		Singleton& operator=(Singleton&&) = delete;

		~Singleton() = default;
	};
}