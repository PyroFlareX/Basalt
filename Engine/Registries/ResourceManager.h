#pragma once

#include <unordered_map>
#include <map>
#include <string>
#include <mutex>
#include <iostream>

namespace bs
{
	template <typename T, typename keytype = std::string>
	class ResourceManager
	{	
		using MapType = std::unordered_map<keytype, T>;
	public:
		ResourceManager() = default;
		~ResourceManager() = default;

		void addAsset(const T& a, const keytype& key) noexcept
		{
			std::lock_guard<std::mutex> mutguard(m_lock);
			m_assetMap.emplace(key, a);
		}

		void addAsset(T&& a, const keytype& key) noexcept
		{
			std::lock_guard<std::mutex> mutguard(m_lock);
			m_assetMap.emplace(key, std::forward<T>(a));
		}

		bool doesAssetExist(const keytype& key) const noexcept
		{
			return m_assetMap.contains(key);
		}

		bool doesAssetExist(keytype&& key) const noexcept
		{
			return m_assetMap.contains(std::forward<T>(key));
		}

		T& getAsset(const keytype& key)
		{
			std::lock_guard<std::mutex> mutguard(m_lock);
			const auto searched = m_assetMap.find(key);
			if(searched != m_assetMap.end())
			{
				return searched->second;
			}
			else
			{
				puts("Tried to retrieve an asset that does not exist!\n");
				std::cout << "Type of asset is: " << typeid(T).name() << " | and the value of the key is: " << key << "\n";
				throw;
			}
		}

		T& getAsset(keytype&& key)
		{
			std::lock_guard<std::mutex> mutguard(m_lock);
			if(doesAssetExist(std::forward<keytype>(key)))
			{
				return m_assetMap.at(key);
			}
			else
			{
				// error!!!
				puts("Tried to retrieve an asset that does not exist!\n");
				std::cout << "Type of asset is: " << typeid(T).name() << " | and the value of the key is: " << key << "\n";
				throw;
			}   
		}

		const T& getAsset(const keytype& key) const
		{	
			const auto searched = m_assetMap.find(key);
			if(searched != m_assetMap.end())
			{
				return searched->second;
			}
			else
			{
				puts("Tried to retrieve an asset that does not exist!\n");
				std::cout << "Type of asset is: " << typeid(T).name() << " | and the value of the key is: " << key << "\n";
				throw;
			}
		}

		const T& getAsset(keytype&& key) const
		{
			const auto searched = m_assetMap.find(key);
			if(searched != m_assetMap.end())
			{
				return searched->second;
			}
			else
			{
				puts("Tried to retrieve an asset that does not exist!\n");
				std::cout << "Type of asset is: " << typeid(T).name() << " | and the value of the key is: " << key << "\n";
				throw;
			}
		}

		void removeAsset(keytype&& key)
		{
			std::lock_guard<std::mutex> mutguard(m_lock);
			m_assetMap.erase(key);
		}

		MapType& getMap() noexcept
		{
			return m_assetMap;
		}

		const MapType& getMap() const noexcept
		{
			return m_assetMap;
		}

	private:
		MapType m_assetMap;
		std::mutex m_lock;
	};

	template <typename T, typename keytype = short>
	class ResourceMap
	{
		using MapType = std::map<keytype, T>;
	public:
		ResourceMap() = default;
		~ResourceMap() = default;

		void addAsset(T& a, keytype& key) noexcept
		{
			std::lock_guard<std::mutex> mutguard(m_lock);
			m_assetMap.emplace(key, a);
		}

		const bool doesAssetExist(keytype&& key) const noexcept
		{
			return m_assetMap.contains(key);
		}

		T& getAsset(keytype&& key)
		{
			std::lock_guard<std::mutex> mutguard(m_lock);
			if(doesAssetExist(std::forward<keytype>(key)))
			{
				return m_assetMap.at(key);
			}
			else
			{
				// error!!!
				puts("Tried to retrieve an asset that does not exist!\n");
				
				std::cout << "Type of asset is: " << typeid(T).name() << " | and the value of the key is: " << key << "\n";
				throw;
			}   
		}

		const T& getAsset(keytype&& key) const
		{
			//std::lock_guard<std::mutex> mutguard(m_lock);
			if(doesAssetExist(std::forward<keytype>(key)))
			{
				return m_assetMap.at(key);
			}
			else
			{
				// error!!!
				puts("Tried to retrieve an asset that does not exist!\n");
				
				std::cout << "Type of asset is: " << typeid(T).name() << " | and the value of the key is: " << key << "\n";
				throw;
			}   
		}

		void removeAsset(keytype&& key)
		{
			std::lock_guard<std::mutex> mutguard(m_lock);
			m_assetMap.erase(key);
		}

		MapType& getMap() noexcept
		{
			return m_assetMap;
		}

		const MapType& getMap() const noexcept
		{
			return m_assetMap;
		}

	private:
		MapType m_assetMap;
		std::mutex m_lock;
	};
}