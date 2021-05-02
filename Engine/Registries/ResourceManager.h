#pragma once

#include <unordered_map>
#include <string>
#include <mutex>

namespace vn
{
    template <typename T>
    class AssetManager
    {
    public:
        AssetManager()
        {
            
        }

        void addAsset(T&& a, std::string&& key) noexcept
        {
            std::lock_guard<std::mutex> mutguard(m_lock);

            m_assetMap.insert(std::forward<std::string>(key), std::forward<T>(a));
        }

        const bool doesAssetExist(const std::string& key) const noexcept
        {
            return m_assetMap.contains(key);
        }

        T& getAsset(const std::string& key)
        {
            std::lock_guard<std::mutex> mutguard(m_lock);
            if(doesAssetExist(key))
            {
                return m_assetMap.at(key);
            }
            else
            {
                // error!!!
                puts("Tried to retrieve an asset that does not exist!\n");
                return m_assetMap["null"];
            }
            
        }

        void removeAsset(std::string& key)
        {
            std::lock_guard<std::mutex> mutguard(m_lock);

            m_assetMap.erase(key);
        }

        ~AssetManager()
        {
            
        }

    private:
        std::unordered_map<std::string, T> m_assetMap;
        std::mutex m_lock;
    };
}