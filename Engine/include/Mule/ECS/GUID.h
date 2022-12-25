#pragma once

#include <random>

namespace Mule
{
	static std::random_device rd;
	static std::mt19937_64 gen(rd());
	static std::uniform_int_distribution<uint64_t> dis;

	class GUID
	{
	public:
		GUID() : mGUID(dis(gen)) {}
		GUID(uint64_t guid) : mGUID(guid) {}

		GUID(const GUID&) = default;
		GUID& operator=(const GUID&) = default;

		bool operator==(const GUID& rhs)
		{
			return mGUID == rhs.mGUID;
		}

		bool operator!=(const GUID& rhs)
		{
			return mGUID != rhs.mGUID;
		}

		uint64_t operator()()
		{
			return mGUID;
		}

	private:
		uint64_t mGUID;
	};
}