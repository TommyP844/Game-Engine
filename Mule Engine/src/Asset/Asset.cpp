#include "Asset/Asset.h"
#include "Log.h"

namespace Mule
{
	void Asset::SetDirectory(const fs::path& path)
	{
#ifdef _DEBUG
		if (!fs::is_directory(path))
		{
			MULE_LOG_ERROR("Failed to set path for asset: {0}\n\tPath is not a valid directory: {1}", mName, path);
			return;
		}
#endif // _DEBUG
		SetModified();
		mDirectory = path;
	}
}