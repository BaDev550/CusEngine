#pragma once

#include <Runtime/RHI/Queue/RHIQueueType.h>

namespace Runtime::RHI {
	struct QueueDesc {
		QueueType type = QueueType::Graphics;
	};
}