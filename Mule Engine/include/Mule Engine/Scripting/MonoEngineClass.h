#pragma once

#include <unordered_map>
#include <map>
#include <string>
#include <functional>

#include <mono/metadata/assembly.h>

namespace Mule
{


	class MonoEngineClass
	{
	public:
		MonoEngineClass(size_t handle, const std::string& className)
			:
			mHandle(handle),
			mClassName(className)
		{}

		const std::string& ClassName() const { return mClassName; }




	private:
		size_t mHandle;
		const std::string& mClassName;
	};
}

