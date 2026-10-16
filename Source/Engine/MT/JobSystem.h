#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <Runtime/Definitions/Types.h>

namespace Tourqe::Engine {
	using JobFunc = std::function<void()>;

	enum class JobPriority : u8 {
		High,
		Low
	};

	class JobSystem {
	public:
		JobSystem();
		~JobSystem();
		JobSystem(const JobSystem&) = delete;
		JobSystem& operator=(const JobSystem&) = delete;
		JobSystem(JobSystem&&) = delete;
		JobSystem& operator=(JobSystem&&) = delete;

		void Execute(JobFunc job, JobPriority priority = JobPriority::High);
		void ParallelFor(u32 count, u32 batchSize, const std::function<void(u32 begin, u32 end)>& body, JobPriority priority = JobPriority::High);

		void Wait();
		bool IsBusy() const { return _pendingJobs.load(std::memory_order_acquire) > 0; }

		u32 GetHighWorkerCount() const { return static_cast<u32>(_highPriorityThreads.size()); }
		u32 GetLowWorkerCount() const { return static_cast<u32>(_lowPriorityThreads.size()); }
	private:
		void WorkerLoop(JobPriority priority);
		void FinishJob();
		static void ConfigureCurrentThread(JobPriority priority, u32 index);

		std::vector<std::thread> _highPriorityThreads;
		std::vector<std::thread> _lowPriorityThreads;

		std::queue<JobFunc> _highQueue;
		std::queue<JobFunc> _lowQueue;

		std::mutex _queueMutex;
		std::condition_variable _workCv;
		bool _stopping = false;

		std::atomic<u32> _pendingJobs{ 0 };
		std::mutex _idleMutex;
		std::condition_variable _idleCv;
	};
}