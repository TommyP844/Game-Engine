#pragma once

namespace Mule
{

	enum class Type
	{
		INT_16,
		UINT_16,

		INT_32,
		UINT_32,
		
		INT_64,
		UINT_64,

		FLOAT_32,
		FLOAT_64,

		VEC2_F32,
		VEC3_F32,
		VEC4_F32,

		VEC2_UI32,
		VEC3_UI32,
		VEC4_UI32,

		VEC2_I32,
		VEC3_I32,
		VEC4_I32,

		MAT2X2_32F,
		MAT3X3_32F,
		MAT4X4_32F
	};

	static uint64_t GetTypeSize(Type type)
	{
		switch (type)
		{
		case Type::INT_16:
		case Type::UINT_16:
			return 2ull;
			break;
		case Type::INT_32:
		case Type::UINT_32:
		case Type::FLOAT_32:
			return 4ull;
			break;
		case Type::VEC2_I32:
		case Type::VEC2_UI32:
		case Type::VEC2_F32:
			return 8ull;
			break;
		case Type::INT_64:
		case Type::UINT_64:
		case Type::FLOAT_64:
			return 8ull;
			break;
		case Type::VEC3_I32:
		case Type::VEC3_UI32:
		case Type::VEC3_F32:
			return 12ull;
			break;
		case Type::VEC4_I32:
		case Type::VEC4_UI32:
		case Type::VEC4_F32:
			return 16ull;
			break;
		case Type::MAT2X2_32F:
			return 16ull;
			break;
		case Type::MAT3X3_32F:
			return 36ull;
			break;
		case Type::MAT4X4_32F:
			return 64ull;
			break;			
		}
		return -1;
	}
}