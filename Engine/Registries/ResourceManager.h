#pragma once

#include <unordered_map>
#include <string>
#include <mutex>

namespace vn
{
    template <typename T, typename keytype = std::string>
    class ResourceManager
    {
    public:
        ResourceManager()
        {
            
        }

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
                throw;
                //return m_assetMap["null"];
            }
            
        }

        void removeAsset(keytype&& key)
        {
            std::lock_guard<std::mutex> mutguard(m_lock);

            m_assetMap.erase(key);
        }

        const std::unordered_map<keytype, T>& getMap()
        {
            return m_assetMap;
        }

        ~ResourceManager()
        {
            
        }

    private:
        std::unordered_map<keytype, T> m_assetMap;
        std::mutex m_lock;
    };
}