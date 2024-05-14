#pragma once

#include <string>
#include <unordered_map>

#include "Util/Singleton.h"
#include "Log.h"

namespace Mule
{
	enum class CSType
	{
		None,

		// Native Types
		Int,
		Single,
		Double,
		Boolean,
		Char,
		String,
		Object,
		Void,
		Array,
		Enum,
		Struct,
		Class,
		IntPtr,
		UIntPtr,

		// Internal Types

		Entity
	};

	class CSTypeMapper : public Util::Singleton<CSTypeMapper>
	{
	public:
		CSTypeMapper()
		{
			mTypeMap = {
				// Native Types
				{"System.Int32", CSType::Int},
				{"System.Single", CSType::Single},
				{"System.Double", CSType::Double},
				{"System.Boolean", CSType::Boolean},
				{"System.Char", CSType::Char},
				{"System.String", CSType::String},
				{"System.Object", CSType::Object},
				{"System.Void", CSType::Void},
				{"System.Array", CSType::Array},
				{"System.Enum", CSType::Enum},
				{"System.Struct", CSType::Struct},
				{"System.Class", CSType::Class},
				{"System.IntPtr", CSType::IntPtr},
				{"System.UIntPtr", CSType::UIntPtr},

				// Engine Types
				{"Mule.Entity", CSType::Entity}
			};
		}
		~CSTypeMapper(){}

		CSType GetType(const std::string& name)
		{
			if (mTypeMap.find(name) != mTypeMap.end())
			{
				return mTypeMap[name];
			}
			MULE_LOG_ERROR("Failed to find type: {0}", name);

			return CSType::None;
		}

	private:
		std::unordered_map<std::string, CSType> mTypeMap;
	};

}