#pragma once

#include "Mule/Ref.h"

#include <string>
#include <vector>
#include <filesystem>
#include <map>

#include "Mule/Util/Log.h"
#include "Mule/Ref.h"

namespace fs = std::filesystem;

namespace Mule
{
	typedef unsigned long long AssetID;
	constexpr AssetID NullAssetID = 0;

	enum AssetType
	{
		Asset_Invalid,
		Asset_Texture,
		Asset_Scene,
		Asset_Shader,
		Asset_MeshCollection,
		Asset_Material
	};

	class AssetManager
	{
	public:
		AssetManager(AssetManager& other) = delete;

		struct AssetEntry
		{
			AssetEntry() = default;
			AssetEntry(const AssetEntry& other) = default;
			std::string Name;
			fs::path Path;
			AssetType Type;
			Ref<void> Asset;
		};

		static void Init(const fs::path root);
		static void Shutdown();

		static AssetManager& Get()
		{
			return *sInstance;
		}

		template<typename T>
		Ref<T> GetAsset(const std::string& name);

		AssetEntry& GetAssetWithMetaData(const std::string& name);

		template<typename T>
		std::vector<AssetEntry> GetAssetsOfType(AssetType type);

		bool IsAssetTracked(const std::string& name);

		void TrackAsset(const std::string& name, Ref<void> asset, const fs::path& path, AssetType type);

		void UnloadAsset(const std::string& name);

		void ChangeAssetName(const std::string& old, const std::string& newName);

		std::vector<std::string> GetAssetNamesOfType(AssetType assetType);

	private:
		AssetManager(){}
		~AssetManager(){}

		AssetType GetTypeFromExtension(const std::string& extension);

		std::map<std::string, AssetEntry> mAssetDB;
		fs::path mRootDir;

		static AssetManager* sInstance;
	};
}

#include "AssetManager.inl"
