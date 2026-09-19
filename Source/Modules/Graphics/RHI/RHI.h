#pragma once
#include "Core/Core.h"
#include "Memory/Memory.h"

#include <glm/glm.hpp>
#include <functional>

struct GLFWwindow;

namespace Graphics {
	using RHI_ContextHandle = void*;
	using RHI_DeviceHandle = void*;
	using RHI_SwapchainHandle = void*;
	using RHI_ImageHandle = void*; // TODO(0x): make this ones a RHI class or just delete it
	using RHI_BufferHandle = void*;
	using RHI_PipelineHandle = void*;
	using RHI_CommandBufferHandle = void*;
	using RHI_CommandFunc = std::function<void(RHI_CommandBufferHandle cmd)>;

	class RHI_RenderContext;
	class RHI_Object;
	class RHI_Image;
	class RHI_RenderCommands;
	class RHI_Swapchain;
	class RHI_Buffer;
	class RHI_Pipeline;

	enum class RHI_Backend : u8 {
		None = 0,
		Vulkan = 1,
		DirectX12 = 2,
		OpenGL = 3
	};

	enum class RHI_Format : u8 {
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

	enum class RHI_ImageTileMode : u8 {
		Undefined = 0,
		Repeat,
		Mirror,
		ClampToEdge,
		ClampToBorder,
		Optimal
	};

	enum class RHI_ImageUsage : u8 {
		None = 0,
		Sampled = BIT(0),
		Storage = BIT(1),
		ColorAttachment = BIT(2),
		DepthStencilAttachment = BIT(3),
		TransferSrc = BIT(4),
		TransferDst = BIT(5)
	};
	CORE_DEFINE_ENUM_FLAG_OPERATORS(RHI_ImageUsage);

	enum class RHI_BufferUsage : u8 {
		None = 0,
		Uniform = BIT(0),
		Storage = BIT(1),
		TransferSrc = BIT(2),
		TransferDst = BIT(3),
		DeviceAddress = BIT(4),
		Index = BIT(5),
		Vertex = BIT(6)
	};
	CORE_DEFINE_ENUM_FLAG_OPERATORS(RHI_BufferUsage);

	enum class RHI_MemoryUsage : u8 {
		Auto,
		GPU,
		CPU,
		CPUToGPU
	};

	enum class RHI_AllocationFlagBits : u8 {
		None = 0,
		HostAccessSequentialWrite = BIT(0),
		CreateMapped = BIT(1)
	};
	CORE_DEFINE_ENUM_FLAG_OPERATORS(RHI_AllocationFlagBits);

	enum class RHI_ImageLayout : u8 {
		Undefined = 0,
		PresentSrc,
		TransferDst,
		TransferSrc,
		ColorAttachment,
		DepthAttachment,
		ShaderReadOnly
	};

	enum class RHI_ShaderStage : u8 {
		Vertex,
		Fragment,
		Compute
	};

	struct RHI_ShaderByteCode { // Not a RHI struct but need to keep the naming clear
		RHI_ShaderStage Stage;
		std::vector<u32> spriv;
	};

	struct RHI_ImageDesc {
		u32 Width = 0;
		u32 Height = 0;
		RHI_Format Format = RHI_Format::Undefined;
		RHI_ImageUsage Usage = RHI_ImageUsage::None;
		RHI_ImageTileMode TileMode = RHI_ImageTileMode::Repeat;
		RHI_ImageLayout Layout = RHI_ImageLayout::Undefined;
	};

	struct RHI_BufferDesc {
		size_t Size = 0;
		RHI_BufferUsage Usage = RHI_BufferUsage::None;
		RHI_MemoryUsage MemoryUsage = RHI_MemoryUsage::CPU;
		RHI_AllocationFlagBits AllocationFlags = RHI_AllocationFlagBits::None;
	};

	struct RHI_VertexInputAttributeDesc {
		std::vector<RHI_Format> Inputs;

		RHI_VertexInputAttributeDesc() = default;
		RHI_VertexInputAttributeDesc(const std::initializer_list<RHI_Format>& inputs) : Inputs(inputs) {}
	};

	struct RHI_PipelineDesc {
		bool DepthTest = true;
		bool Blending = true;
		RHI_ShaderByteCode* VertexShader = nullptr;
		RHI_ShaderByteCode* FragmentShader = nullptr;
		RHI_VertexInputAttributeDesc AttribDesc;
		std::vector<RHI_Format> ColorFormats;
		std::vector<RHI_Format> DepthFormats;
	};

	struct RHI_GPUFeatures {
		bool DynamicRendering = false;
		bool Synchronization2 = false;
		bool TimelineSemaphore = false;
		bool BufferDeviceAddress = false;
		bool DescriptorIndexing = false;
		bool RuntimeDescriptorArray = false;
		bool RobustBufferAccess = false;
		bool SamplerAnisotropy = false;
		bool RayTracing = false;
		bool MeshShader = false;
	};

	struct RHI_RenderContextDesc {
		GLFWwindow* WindowHandle = nullptr; // Or a PAL_Handle for corect PAL
	};

	struct RHI_SwapchainDesc {
		u32 Width;
		u32 Height;
		bool vsync = true;
	};

	extern "C" {
		ENGINE_API [[nodiscard]] RHI_RenderContext* RHI_CreateRenderContext(const RHI_RenderContextDesc& desc);
		ENGINE_API void RHI_DestroyRenderContext(RHI_RenderContext* context);
		
		ENGINE_API [[nodiscard]] RHI_RenderCommands* RHI_CreateRenderCommands(RHI_RenderContext* context, RHI_Swapchain* swapchain);
		ENGINE_API void RHI_DestroyRenderCommands(RHI_RenderCommands* commands);

		ENGINE_API [[nodiscard]] RHI_Swapchain* RHI_CreateSwapchain(RHI_RenderContext* context, const RHI_SwapchainDesc& desc);
		ENGINE_API void RHI_DestroySwapchain(RHI_Swapchain* swapchain);

		ENGINE_API [[nodiscard]] RHI_Image* RHI_CreateImage(RHI_RenderCommands* commands, const RHI_ImageDesc& desc);
		ENGINE_API [[nodiscard]] RHI_Buffer* RHI_CreateBuffer(RHI_RenderCommands* commands, const RHI_BufferDesc& desc);
		ENGINE_API [[nodiscard]] RHI_Pipeline* RHI_CreatePipeline(RHI_RenderCommands* commands, const RHI_PipelineDesc& desc);
	}
}