#include "ThumbnailManager.h"

#include "Util.h"

ImTextureID ThumbnailManager::GetThumbnail(Mule::AssetHandle handle)
{
	std::lock_guard<std::mutex> lock(mMutex);

	auto iter = mThumbnails.find(handle);
	if (iter != mThumbnails.end())
		return iter->second->GetImGuiID();

	Mule::AssetManager& manager = Mule::AssetManager::Get();
	auto asset = manager.GetAsset<Mule::Asset>(handle);
	if (asset)
	{
		mAssetsToRender.push(asset->Handle());
		return mFileTexture->GetImGuiID();
	}

	return mFileTexture->GetImGuiID();
}

ImTextureID ThumbnailManager::GetThumbNail(const fs::path& filepath)
{
	if (fs::is_directory(filepath))
		return mFolderTexture->GetImGuiID();

	std::lock_guard<std::mutex> lock(mMutex);

	Mule::AssetManager& manager = Mule::AssetManager::Get();
	auto asset = manager.GetAssetByFilepath<Mule::Asset>(filepath);
	if (asset)
	{
		auto handle = asset->Handle();

		auto iter = mThumbnails.find(handle);
		if (iter != mThumbnails.end())
			return iter->second->GetImGuiID();
		else
		{
			auto& ctn = mAssetsToRender._Get_container();
			if (std::find(ctn.begin(), ctn.end(), handle) == ctn.end())
				mAssetsToRender.push(handle);
			return mFileTexture->GetImGuiID();
		}
	}

	return mFileTexture->GetImGuiID();
}

ThumbnailManager::ThumbnailManager(Mule::Ref<Mule::GraphicsDevice> device, Mule::RenderContext context)
	:
	mMutex(std::mutex()),
	mContext(context),
	mDevice(device)
{
	mRunning = true;
	mRenderThread = std::thread(&ThumbnailManager::Render, this);

	// TODO: finis	
	mFileTexture = Mule::Texture::Create(mDevice, mContext, "C:\\Dev\\Mule\\Resources\\Textures\\file.png");
	mFolderTexture = Mule::Texture::Create(mDevice, mContext, "C:\\Dev\\Mule\\Resources\\Textures\\folder.png");
}

ThumbnailManager::~ThumbnailManager()
{
	mRunning = false;
	mRenderThread.join();
}

void ThumbnailManager::Render()
{
	while (mRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		std::lock_guard<std::mutex> lock(mMutex);

		Mule::AssetHandle handle = Mule::Asset::NullHandle;
		
		if (mAssetsToRender.empty())
		{
			continue;
		}

		handle = mAssetsToRender.front();
		mAssetsToRender.pop();

		Mule::AssetManager& manager = Mule::AssetManager::Get();
		Mule::Ref<Mule::Asset> asset = manager.GetAsset<Mule::Asset>(handle);

		if (asset == nullptr)
		{
			MULE_LOG_ERROR("Asset missing", "");
			continue;
		}
		// do rendering here

		Mule::AssetType type = asset->Type();
		switch (type)
		{
		case Mule::AssetType::Scene:
			break;
		case Mule::AssetType::Texture:
			mThumbnails[handle] = asset;
			break;
		case Mule::AssetType::Mesh:
			break;
		case Mule::AssetType::Model:
			break;
		case Mule::AssetType::Shader:
			break;
		case Mule::AssetType::RenderPass:
			break;
		case Mule::AssetType::Material:
			break;
		case Mule::AssetType::None:
		default:
			break;
		}
	}
}
