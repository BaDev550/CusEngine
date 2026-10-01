#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace Runtime::RHI {
	class Image;

	struct ColorAttachment {
		Image* image = nullptr;
		glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);

		ColorAttachment() = default;
		ColorAttachment(Image* img, const glm::vec4& clearCol = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)) : image(img), clearColor(clearCol) {}
	};

	struct RenderingSubmitInfo {
		std::vector<ColorAttachment> colorAttachments;
		Image* depthAttachment;

		glm::vec2 extent;
	};
}