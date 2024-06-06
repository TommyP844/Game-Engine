
#include "Ref.h"
#include "Graphics/ModelDescription.h"
#include "Graphics/GraphicsDevice.h"

#include <future>

namespace Mule
{
	std::future<Ref<ModelDescription>> DeserializeModelAsync(WeakRef<GraphicsDevice> device, const fs::path& filepath, AssetHandle handle = Asset::GenerateHandle());
	Ref<ModelDescription> DeserializeModel(WeakRef<GraphicsDevice> device, const fs::path& filepath, AssetHandle handle = Asset::GenerateHandle());
}