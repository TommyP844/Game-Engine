#pragma once


namespace Mule::Internal::ThreadPool
{
	void Init();

	void Shutdown();

	void SubmitTask(std::function<void()> func);

	int GetNumThreads();
}