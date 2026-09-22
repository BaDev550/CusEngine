#pragma once
#include <Engine/Core/Core.h>
#include <Engine/Subsystem/Subsystem.h>

#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <queue>

namespace CusEngine::MT { // TODO(0x): change this entair fucking system!
	enum class JobPriority {
		Low = 0,
		High
	};

	using JobFunc = std::function<void()>;
	class ENGINE_API JobSubsystem final : public Subsystem {
	public:
		virtual Result OnCreate(Engine* engine) override;
		virtual void OnUpdate() override;
		virtual void OnDestroy() override;

		virtual void GetDependencyGraph(DependencyGraph& graph) override;

		void Execute(const JobFunc& job, JobPriority priority);
	private:
		void Wait();
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