#pragma once

#include <memory>

namespace Mule
{
	namespace Util
	{
		template<typename T>
		class Singleton
		{
		public:
			template<typename ...Args>
			static T& Initialize(Args&&... args)
			{
				mInstance = new T(std::forward<Args>(args)...);
				return *mInstance;
			}

			static void Shutdown()
			{
				if (mInstance)
				{
					delete mInstance;
				}
			}

			static T& Get()
			{
				return *mInstance;
			}

		private:
			static T* mInstance;
		};


		template<typename T>
		T* Singleton<T>::mInstance = nullptr;
	}
}