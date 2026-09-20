#pragma once
#include "Core/Core.h"
#include "Core/Memory.h"

#include <glm/glm.hpp>
#include <functional>

struct GLFWwindow;

namespace CusEngine {
	class RenderObject;
	class Texture2D;

	namespace RHI {
		class RenderContext;
		class RenderCommands;
		class Swapchain;
		class Image;
		class Buffer;
		class Pipeline;

		using CommandFunc = std::function<void()>;

		enum class GraphicsBackend : u8 {
			None = 0,
			Vulkan = 1,
			OpenGL = 2
		};

		enum class ImageFormat : u8 {
			Undefined = 0,
			RG8,
			RGB8,
			RGBA8,
			RGBA16,
			RGBA,

			D32_SFLOAT,
			D16_UNORM,
			D24_UNORM_S8_UINT
		};

		enum class ImageTileMode : u8 {
			Undefined = 0,
			Repeat,
			Mirror,
			ClampToEdge,
			ClampToBorder,
			Optimal
		};

		enum class ImageUsage : u8 {
			None = 0,
			Sampled = BIT(0),
			Storage = BIT(1),
			ColorAttachment = BIT(2),
			DepthStencilAttachment = BIT(3),
			TransferSrc = BIT(4),
			TransferDst = BIT(5)
		};
		CORE_DEFINE_ENUM_FLAG_OPERATORS(ImageUsage);

		enum class BufferUsage : u8 {
			None = 0,
			Uniform = BIT(0),
			Storage = BIT(1),
			TransferSrc = BIT(2),
			TransferDst = BIT(3),
			DeviceAddress = BIT(4),
			Index = BIT(5),
			Vertex = BIT(6)
		};
		CORE_DEFINE_ENUM_FLAG_OPERATORS(BufferUsage);

		enum class MemoryUsage : u8 {
			Auto,
			GPU,
			CPU,
			CPUToGPU
		};

		enum class AllocationFlagBits : u8 {
			None = 0,
			HostAccessSequentialWrite = BIT(0),
			CreateMapped = BIT(1)
		};
		CORE_DEFINE_ENUM_FLAG_OPERATORS(AllocationFlagBits);

		enum class ImageLayout : u8 {
			Undefined = 0,
			PresentSrc,
			TransferDst,
			TransferSrc,
			ColorAttachment,
			DepthAttachment,
			ShaderReadOnly
		};

		enum class ShaderStage : u8 {
			Vertex,
			Fragment,
			Compute
		};

		struct ImageDesc {
			u32 width = 0;
			u32 height = 0;
			ImageFormat format = ImageFormat::Undefined;
			ImageUsage usage = ImageUsage::None;
			ImageTileMode tileMode = ImageTileMode::Repeat;
			ImageLayout layout = ImageLayout::Undefined;
		};

		struct BufferDesc {
			usize size = 0;
			BufferUsage usage = BufferUsage::None;
			MemoryUsage memoryUsage = MemoryUsage::CPU;
			AllocationFlagBits allocationFlags = AllocationFlagBits::None;
		};

		struct VertexInputAttributeDesc {
			std::vector<ImageFormat> inputs;

			VertexInputAttributeDesc() = default;
			VertexInputAttributeDesc(const std::initializer_list<ImageFormat>& formatInputs) : inputs(formatInputs) {}
		};

		struct ShaderByteCode {
			ShaderStage stage;
			std::vector<u32> spriv;
		};

		struct PipelineDesc {
			bool depthTest = true;
			bool blending = true;
			ShaderByteCode* vertexShader = nullptr;
			ShaderByteCode* fragmentShader = nullptr;
			VertexInputAttributeDesc attribDesc;
			std::vector<ImageFormat> colorFormats;
			std::vector<ImageFormat> depthFormats;
		};

		struct GPUFeatures {
			bool dynamicRendering = false;
			bool synchronization2 = false;
			bool timelineSemaphore = false;
			bool bufferDeviceAddress = false;
			bool descriptorIndexing = false;
			bool runtimeDescriptorArray = false;
			bool robustBufferAccess = false;
			bool samplerAnisotropy = false;
			bool rayTracing = false;
			bool meshShader = false;
		};

		struct RenderContextDesc {
			GLFWwindow* windowHandle = nullptr;
			GPUFeatures features;
		};

		struct SwapchainDesc {
			u32 width;
			u32 height;
			bool vsync = true;
		};
	}
}