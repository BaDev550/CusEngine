#pragma once
#include <Engine/Core/Core.h>

#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <queue>

namespace Tourqe::Engine { // TODO(0x): change this entair fucking system!
	enum class JobPriority {
		Low = 0,
		High
	};

	using JobFunc = std::function<void()>;
	class ENGINE_API JobSystem final {
	public:
		JobSystem();
		~JobSystem();
		
		void Wait();
		void Execute(const JobFunc& job, JobPriority priority);
	private:
		void WorkerLoop(JobPriority priority);
		void SetThreadPowerMode(JobPriority priority);

		std::vector<std::thread> _highPriorityThreads;
		std::vector<std::thread> _lowPriorityThreads;

		std::queue<JobFunc> _highQueue;
		std::queue<JobFunc> _lowQueue;

		std::mutex _queueMutex;
		std::condition_variable _cv;

		std::atomic<u32> _activeJob{ 0 };
		std::atomic<bool> _stopping{ false };
	};
}