#pragma once

#include <string>
#include <filesystem>
#include <random>
#include <cstdint>


namespace fs = std::filesystem;

namespace Mule
{
	enum class SerializationMode
	{
		Binary,
		Text
	};

	enum class AssetType
	{
		Scene,
		Texture,
		Mesh,
		Model,
		Shader,
		RenderPass,
		Material,
		None
	};

	typedef size_t AssetHandle;

	class Asset
	{
	public:
		static const AssetHandle NullHandle = 0;

		Asset(AssetHandle handle, const std::string& name, const fs::path& directory, AssetType type)
			:
			mHandle(handle),
			mFileName(name),
			mDirectory(directory),
			mType(type),
			mAssetVersion(0)
		{
			if (fs::exists(mDirectory))
			{
				mName = directory.filename().replace_extension().string();
				mFileName = directory.filename().string();
				mAssetVersion = 0ull;
			}
		}

		Asset(AssetHandle handle, const fs::path& filepath, AssetType type)
			:
			mHandle(handle),
			mType(type),
			mAssetVersion(0)
		{
			if (fs::exists(mDirectory))
			{
				mDirectory = filepath.parent_path();
				mFileName = filepath.filename().string();
				mName = filepath.filename().replace_extension().string();
				mAssetVersion = fs::last_write_time(mDirectory).time_since_epoch().count();
			}
		}

		virtual ~Asset(){}

		AssetHandle Handle() const { return mHandle; }
		void SetHandle(AssetHandle handle) { mHandle = handle; }
		const std::string& Name() const { return mName; }
		const std::string& FileName() const { return mFileName; }
		const fs::path& Directory() const { return mDirectory; }
		fs::path Filepath() const { return mDirectory / mFileName; }
		AssetType Type() const { return mType; }
		uint64_t LastWriteTime() const { return mAssetVersion; }

		void ClearModified() { mModified = false; }
		void SetModified() { mModified = true; }
		bool Modified() const { return mModified; }

		void SetLoaded() { mLoaded = true; }
		bool IsLoaded() const { return mLoaded; }

		void ChangeFilename(const std::string& name)
		{
			mName = name;
			mFileName = name + mFileName.substr(mFileName.find_last_of("."));
		}

		void ChangeDirectory(const fs::path& directory)
		{
			mDirectory = directory;
		}

		static size_t GenerateHandle()
		{
			std::random_device rd;
			std::mt19937_64 gen(rd());
			std::uniform_int_distribution<size_t> dis;

			AssetHandle handle = dis(gen);

			if (handle == NullHandle)
				return GenerateHandle();

			return handle;
		}

	protected:
		// for async loading
		std::atomic<bool> mLoaded;
	private:
		AssetHandle mHandle;
		std::string mName;
		std::string mFileName;
		fs::path mDirectory;
		AssetType mType;
		uint64_t mAssetVersion;
		bool mModified = false;
	};
}