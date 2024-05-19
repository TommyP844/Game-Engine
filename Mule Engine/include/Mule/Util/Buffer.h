#pragma once

namespace Mule
{
	template<typename T = uint8_t>
	class Buffer
	{
	public:
		Buffer(){}
		~Buffer(){}

		const uint32_t TypeSize = sizeof(T);

		Buffer(void* data, size_t count)
		{
			SetData(data, count);
		}

		Buffer(const Buffer& other)
		{
			SetData(other.mData, other.mCount);
		}

		Buffer(size_t count)
		{
			mData = new T[count];
			mCount = count;
		}

		void SetData(void* data, size_t count)
		{
			Release();
			mData = data;
			mCount = count;
		}

		void Allocate(size_t count)
		{
			Release();
			mData = new T[count];
			mCount = count;
		}

		void Release()
		{
			delete mData;
			mData = nullptr;
			mCount = 0;
		}

		T* Data()
		{
			return mData;
		}

		T* Data() const
		{
			return mData;
		}

		size_t Count()
		{
			return mCount;
		}

		size_t Count() const
		{
			return mCount;
		}

		operator bool() {
			return mData != nullptr && mCount > 0;
		}

		operator bool() const {
			return mData != nullptr && mCount > 0;
		}

	protected:
		T* mData = nullptr;
		size_t mCount = 0;
	};
}
