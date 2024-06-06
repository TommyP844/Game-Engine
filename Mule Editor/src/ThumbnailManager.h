#pragma once

#include "Mule.h"

#include "imgui.h"

#include <map>
#include <queue>
#include <mutex>
#include <atomic>

class ThumbnailManager : public Mule::Util::Singleton<ThumbnailManager>
{
public:
	ImTextureID GetThumbnail(Mule::AssetHandle handle);
	ImTextureID GetThumbNail(const fs::path& filepath);

private:
	friend class Mule::Util::Singleton<ThumbnailManager>;
	ThumbnailManager(Mule::Ref<Mule::GraphicsDevice> device, Mule::RenderContext context);
	Mule::Ref<Mule::GraphicsDevice> mDevice;
	Mule::RenderContext mContext;
	~ThumbnailManager();
	std::mutex mMutex;
	std::thread mRenderThread;
	std::atomic<bool> mRunning;
	std::map<Mule::AssetHandle, Mule::Ref<Mule::Texture>> mThumbnails;
	Mule::Ref<Mule::Texture> mFileTexture;
	Mule::Ref<Mule::Texture> mFolderTexture;

	std::queue<Mule::AssetHandle> mAssetsToRender;

	void Render();
};