#include <Engine/MT/JobSystem.h>

#include <Runtime/Definitions/Logger.h>

#include <algorithm>
#include <exception>
#include <memory>
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

namespace Tourqe::Engine {

	JobSystem::JobSystem() {
		const u32 hw = std::max(1u, std::thread::hardware_concurrency());
		const u32 workers = hw > 1 ? hw - 1 : 1;

		const u32 highCount = std::max(1u, (workers * 7) / 10);
		const u32 lowCount = workers > highCount ? workers - highCount : 0;

		_highPriorityThreads.reserve(highCount);
		_lowPriorityThreads.reserve(lowCount);

		for (u32 i = 0; i < highCount; ++i) {
			_highPriorityThreads.emplace_back([this, i]() {
				ConfigureCurrentThread(JobPriority::High, i);
				WorkerLoop(JobPriority::High);
				});
			Logger::Info("JobSystem", "Performance core attached");
		}

		for (u32 i = 0; i < lowCount; ++i) {
			_lowPriorityThreads.emplace_back([this, i]() {
				ConfigureCurrentThread(JobPriority::Low, i);
				WorkerLoop(JobPriority::Low);
				});
			Logger::Info("JobSystem", "Efficiency core attached");
		}
	}

	JobSystem::~JobSystem() {
		Wait();

		{
			std::lock_guard<std::mutex> lock(_queueMutex);
			_stopping = true;
		}
		_workCv.notify_all();

		for (std::thread& worker : _highPriorityThreads) { if (worker.joinable()) worker.join(); }
		for (std::thread& worker : _lowPriorityThreads) { if (worker.joinable()) worker.join(); }
	}

	void JobSystem::Wait() {
		std::unique_lock<std::mutex> lock(_idleMutex);
		_idleCv.wait(lock, [this]() {
			return _pendingJobs.load(std::memory_order_acquire) == 0;
			});
	}

	void JobSystem::Execute(JobFunc job, JobPriority priority) {
		if (!job) return;

		{
			std::lock_guard<std::mutex> lock(_queueMutex);
			if (_stopping) return;

			_pendingJobs.fetch_add(1, std::memory_order_acq_rel);

			if (priority == JobPriority::High) _highQueue.push(std::move(job));
			else                               _lowQueue.push(std::move(job));
		}

		_workCv.notify_all();
	}

	void JobSystem::ParallelFor(u32 count, u32 batchSize,
		const std::function<void(u32, u32)>& body, JobPriority priority) {
		if (count == 0 || !body) return;
		batchSize = std::max(1u, batchSize);

		struct Group {
			std::atomic<u32> remaining{ 0 };
			std::mutex mutex;
			std::condition_variable cv;
		};

		const u32 batches = (count + batchSize - 1) / batchSize;
		auto group = std::make_shared<Group>();
		group->remaining.store(batches, std::memory_order_release);

		for (u32 b = 0; b < batches; ++b) {
			const u32 begin = b * batchSize;
			const u32 end = std::min(count, begin + batchSize);

			Execute([group, &body, begin, end]() {
				struct Done {
					Group& g;
					~Done() {
						if (g.remaining.fetch_sub(1, std::memory_order_acq_rel) == 1) {
							std::lock_guard<std::mutex> lock(g.mutex);
							g.cv.notify_all();
						}
					}
				} done{ *group };
				body(begin, end);
				}, priority);
		}

		std::unique_lock<std::mutex> lock(group->mutex);
		group->cv.wait(lock, [&]() { return group->remaining.load(std::memory_order_acquire) == 0; });
	}

	void JobSystem::FinishJob() {
		if (_pendingJobs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
			std::lock_guard<std::mutex> lock(_idleMutex);
			_idleCv.notify_all();
		}
	}

	void JobSystem::WorkerLoop(JobPriority priority) {
		const bool isHigh = (priority == JobPriority::High);

		while (true) {
			JobFunc job;

			{
				std::unique_lock<std::mutex> lock(_queueMutex);

				_workCv.wait(lock, [&]() {
					return _stopping
						|| !_lowQueue.empty()
						|| (isHigh && !_highQueue.empty());
					});

				if (isHigh && !_highQueue.empty()) {
					job = std::move(_highQueue.front());
					_highQueue.pop();
				}
				else if (!_lowQueue.empty()) {
					job = std::move(_lowQueue.front());
					_lowQueue.pop();
				}
				else if (_stopping) {
					return;
				}
			}

			if (!job) continue;

			try {
				job();
			}
			catch (const std::exception& e) {
				Logger::Error("JobSystem", "Job threw exception: {}", e.what());
			}
			catch (...) {
				Logger::Error("JobSystem", "Job threw unknown exception");
			}

			FinishJob();
		}
	}

	void JobSystem::ConfigureCurrentThread(JobPriority priority, u32 index) {
#ifdef _WIN32
		HANDLE hThread = GetCurrentThread();

		const std::wstring name = (priority == JobPriority::High ? L"Tourqe Job High " : L"Tourqe Job Low ")
			+ std::to_wstring(index);
		SetThreadDescription(hThread, name.c_str());

		THREAD_POWER_THROTTLING_STATE throttling{};
		throttling.Version = THREAD_POWER_THROTTLING_CURRENT_VERSION;
		throttling.ControlMask = THREAD_POWER_THROTTLING_EXECUTION_SPEED;
		throttling.StateMask = (priority == JobPriority::High) ? 0 : THREAD_POWER_THROTTLING_EXECUTION_SPEED;
		SetThreadInformation(hThread, ThreadPowerThrottling, &throttling, sizeof(throttling));

		SetThreadPriority(hThread, priority == JobPriority::High
			? THREAD_PRIORITY_NORMAL
			: THREAD_PRIORITY_BELOW_NORMAL);
#else
		(void)priority; (void)index;
#endif
	}
}