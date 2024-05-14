#pragma once

namespace Mule
{
	class Buffer
	{
	public:
		Buffer(){}
		~Buffer(){}

		Buffer(void* data, size_t bytes)
		{
			SetData(data, bytes);
		}

		Buffer(const Buffer& other)
		{
			SetData(other.mData, other.mBytes);
		}

		Buffer(size_t bytes)
		{
			mData = new char[bytes];
			mBytes = bytes;
		}

		void SetData(void* data, size_t bytes)
		{
			Release();
			mData = data;
			mBytes = bytes;
		}

		void Allocate(size_t bytes)
		{
			Release();
			mData = new char[bytes];
			mBytes = bytes;
		}

		void Release()
		{
			delete mData;
			mData = nullptr;
			mBytes = 0;
		}

		void* Data()
		{
			return mData;
		}

		void* Data() const
		{
			return mData;
		}

		size_t Bytes()
		{
			return mBytes;
		}

		size_t Bytes() const
		{
			return mBytes;
		}

		template<typename T>
		T* As()
		{
			return (T*)mData;
		}

		template<typename T>
		T* As() const
		{
			return (T*)mData;
		}

		template<typename T>
		size_t Count()
		{
			return mBytes / sizeof(T);
		}

		operator bool() {
			return mData != nullptr && mBytes > 0;
		}

		operator bool() const {
			return mData != nullptr && mBytes > 0;
		}

	protected:
		void* mData = nullptr;
		size_t mBytes = 0;
	};

	class ScopedBuffer : public Buffer
	{
		ScopedBuffer(void* data, size_t bytes)
		{
			SetData(data, bytes);
		}

		ScopedBuffer(const Buffer& other)
		{
			SetData(other.Data(), other.Bytes());
		}

		~ScopedBuffer()
		{
			Release();
		}
	};
}
