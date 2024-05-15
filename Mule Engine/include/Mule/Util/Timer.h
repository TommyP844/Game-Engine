#pragma once

#include <chrono>

namespace Mule {
	class Timer
	{
	public:
		Timer() {}
		~Timer() {}

		void Start()
		{
			mStart = std::chrono::high_resolution_clock::now();
		}

		float Stop()
		{
			auto elapsed = std::chrono::high_resolution_clock::now() - mStart;
			float seconds = (float)elapsed.count() * 1e-9f;
			return seconds;
		}

		float Elapsed()
		{
			auto elapsed = std::chrono::high_resolution_clock::now() - mStart;
			float seconds = (float)elapsed.count() * 1e-9f;
			return seconds;
		}

	private:
		std::chrono::high_resolution_clock::time_point mStart;
	};
}