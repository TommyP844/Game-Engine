#include "mulepch.h"

#include "Mule/Internal/ThreadPool.h"

#include "Mule/Util/Log.h"

namespace Mule::Internal::ThreadPool
{
	struct ThreadPoolData
	{
		std::queue<std::function<void()>> TaskQueue;
		std::vector<std::thread> Threads;
		std::mutex Mutex;
		int NumThreads = 0;
		bool Running = false;
	};

	static void ThreadFunc();

	static ThreadPoolData sThreadData;

	void Init()
	{
		sThreadData.NumThreads = std::thread::hardware_concurrency();
		LOG_MSG(L"Thread Pool Started, {0} Threads running", sThreadData.NumThreads);
		sThreadData.Running = true;
		for (int i = 0; i < sThreadData.NumThreads; i++)
		{
			sThreadData.Threads.push_back(std::thread(ThreadFunc));
			LOG_MSG(L"Thread {0} created", i);
		}
		
	}

	void Shutdown()
	{
		sThreadData.Running = false;
		for (int i = 0; i < sThreadData.NumThreads; i++)
		{
			sThreadData.Threads[i].join();
			LOG_MSG(L"Joining Thread {0}", i);
		}
		LOG_MSG(L"Thread Pool Shutdown");
	}

	void SubmitTask(std::function<void()> func)
	{
		std::lock_guard<std::mutex> lock(sThreadData.Mutex);
		sThreadData.TaskQueue.push(func);
		LOG_MSG(L"Adding task to thread pool");
	}

	int GetNumThreads()
	{
		return sThreadData.NumThreads;
	}

	void ThreadFunc()
	{		
		while (sThreadData.Running)
		{
			std::lock_guard<std::mutex> lock(sThreadData.Mutex);
			if (sThreadData.TaskQueue.size() > 0)
			{
				std::function<void()> task;
				{
					task = sThreadData.TaskQueue.front();
					sThreadData.TaskQueue.pop();
				}
				task();
			}
		}
	}

}