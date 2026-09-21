#include <Engine/Core/Logger.h>
#include <Engine/MT/JobSubsystem.h>
#include <Engine/Window/WindowSubsystem.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

namespace CusEngine::MT {
	bool JobSubsystem::OnCreate(Engine* engine) {
		Subsystem::OnCreate(engine);

		u32 totalCores = std::thread::hardware_concurrency();
		u32 highCoreCount = std::max(1u, (totalCores * 7) / 10);
		u32 lowCoreCount = std::max(1u, totalCores - highCoreCount);

		for (u32 i = 0; i < highCoreCount; i++) {
			_highPriorityThreads.emplace_back([this]() {
				SetThreadPowerMode(JobPriority::High);
				WorkerLoop(JobPriority::High);
				});
			Logger::Info("Performance Core", "Attached to thread");
		}

		for (u32 i = 0; i < lowCoreCount; i++) {
			_lowPriorityThreads.emplace_back([this]() {
				SetThreadPowerMode(JobPriority::Low);
				WorkerLoop(JobPriority::Low);
				});
			Logger::Info("Efficiency Core", "Attached to thread");
		}

		return true;
	}

	void JobSubsystem::OnUpdate() {
		Wait();
	}

	void JobSubsystem::OnDestroy() {
		Subsystem::OnDestroy();

		Wait();

		{
			std::lock_guard<std::mutex> lock(_queueMutex);
			_stopping = true;
		}

		_cv.notify_all();

		for (std::thread& worker : _highPriorityThreads) { if (worker.joinable()) worker.join(); }
		for (std::thread& worker : _lowPriorityThreads) { if (worker.joinable()) worker.join(); }

		_highPriorityThreads.clear();
		_lowPriorityThreads.clear();
	}

	void JobSubsystem::GetDependencyGraph(DependencyGraph& graph) {
		graph.Require<WindowSubsystem>(DependencyOrder::After);
	}

	void JobSubsystem::Wait() {
		while (_activeJob.load() > 0) {
			std::this_thread::yield();
		}
	}

	void JobSubsystem::Execute(const JobFunc& job, JobPriority priority) {
		{
			std::lock_guard<std::mutex> lock(_queueMutex);
			if (priority == JobPriority::High) {
				_highQueue.push(job);
			}
			else {
				_lowQueue.push(job);
			}
			_activeJob++;
		}
		_cv.notify_all();
	}

	void JobSubsystem::WorkerLoop(JobPriority priority) {
		while (true) {
			JobFunc job;

			{
				std::unique_lock<std::mutex> lock(_queueMutex);
				_cv.wait(lock, [this]() {
					return !_highQueue.empty() || !_lowQueue.empty() || _stopping;
					});

				if (_stopping && _highQueue.empty() && _lowQueue.empty())
					return;

				if (priority == JobPriority::High) {
					if (!_highQueue.empty()) {
						job = std::move(_highQueue.front());
						_highQueue.pop();
					}
					else if (!_lowQueue.empty()) {
						job = std::move(_lowQueue.front());
						_lowQueue.pop();
					}
				}
				else {
					if (!_lowQueue.empty()) {
						job = std::move(_lowQueue.front());
						_lowQueue.pop();
					}
					else if (!_highQueue.empty()) {
						job = std::move(_highQueue.front());
						_highQueue.pop();
					}
				}
			}

			if (job) {
				job();
				_activeJob--;
			}
		}
	}

	void JobSubsystem::SetThreadPowerMode(JobPriority priority) {
#ifdef _WIN32
		HANDLE hThread = GetCurrentThread();
		THREAD_POWER_THROTTLING_STATE throttling{};
		throttling.Version = THREAD_POWER_THROTTLING_CURRENT_VERSION;
		throttling.ControlMask = THREAD_POWER_THROTTLING_EXECUTION_SPEED;

		if (priority == JobPriority::High) {
			throttling.StateMask = 0;
		}
		else {
			throttling.StateMask = THREAD_POWER_THROTTLING_EXECUTION_SPEED;
		}
		SetThreadInformation(hThread, ThreadPowerThrottling, &throttling, sizeof(throttling));
#endif
	}
}