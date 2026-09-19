#pragma once

#include "Graphics/RHI/RHI.h"
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

namespace Graphics::Utils {
	constexpr [[nodiscard]] VkFormat RHI_GetVkFormat(RHI_Format format) noexcept {
		switch (format)
		{
		case RHI_Format::Undefined: return VK_FORMAT_UNDEFINED;
		case RHI_Format::RG8: return VK_FORMAT_R8G8_UNORM;
		case RHI_Format::RGB8: return VK_FORMAT_R8G8B8_UNORM;
		case RHI_Format::RGBA8: return VK_FORMAT_R8G8B8A8_SRGB;
		case RHI_Format::RGBA16: return VK_FORMAT_R16G16B16_SFLOAT;
		case RHI_Format::RGBA: return VK_FORMAT_R32G32B32A32_SFLOAT;
		case RHI_Format::D32_SFLOAT: return VK_FORMAT_D32_SFLOAT;
		case RHI_Format::D16_UNORM: return VK_FORMAT_D16_UNORM;
		case RHI_Format::D24_UNORM_S8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;
		default:
			Logger::Error("Vulkan RHI Utils", "Unknown format");
			return VK_FORMAT_UNDEFINED;
		}
	}

	constexpr [[nodiscard]] VkImageLayout RHI_GetVkImageLayout(RHI_ImageLayout layout) noexcept {
		switch (layout)
		{
		case RHI_ImageLayout::Undefined: return VK_IMAGE_LAYOUT_UNDEFINED;
		case RHI_ImageLayout::PresentSrc: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		case RHI_ImageLayout::ColorAttachment: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		case RHI_ImageLayout::DepthAttachment: return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		case RHI_ImageLayout::ShaderReadOnly: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		case RHI_ImageLayout::TransferDst: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		case RHI_ImageLayout::TransferSrc: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		default:
			Logger::Error("Vulkan RHI Utils", "Unknown image layout");
			return VK_IMAGE_LAYOUT_UNDEFINED;
		}
	}

	constexpr [[nodiscard]] VkBufferUsageFlags RHI_GetVkBufferUsage(RHI_BufferUsage usage) noexcept {
		VkBufferUsageFlags flags = 0;
		if ((usage & RHI_BufferUsage::Uniform) != RHI_BufferUsage::None) flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		if ((usage & RHI_BufferUsage::Storage) != RHI_BufferUsage::None) flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		if ((usage & RHI_BufferUsage::TransferSrc) != RHI_BufferUsage::None) flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		if ((usage & RHI_BufferUsage::TransferDst) != RHI_BufferUsage::None) flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		if ((usage & RHI_BufferUsage::DeviceAddress) != RHI_BufferUsage::None) flags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		if ((usage & RHI_BufferUsage::Index) != RHI_BufferUsage::None) flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		if ((usage & RHI_BufferUsage::Vertex) != RHI_BufferUsage::None) flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		return flags;
	}

	constexpr [[nodiscard]] VmaMemoryUsage RHI_GetVkMemoryUsage(RHI_MemoryUsage usage) noexcept {
		switch (usage)
		{
		case RHI_MemoryUsage::Auto: return VMA_MEMORY_USAGE_AUTO;
		case RHI_MemoryUsage::GPU: return VMA_MEMORY_USAGE_GPU_ONLY;
		case RHI_MemoryUsage::CPU: return VMA_MEMORY_USAGE_CPU_ONLY;
		case RHI_MemoryUsage::CPUToGPU: return VMA_MEMORY_USAGE_CPU_TO_GPU;
		default:
			Logger::Error("Vulkan RHI Utils", "Unknown memory usage fallback 'auto' used");
			return VMA_MEMORY_USAGE_AUTO;
		}
	}

	constexpr [[nodiscard]] VmaAllocationCreateFlags RHI_GetVkAllocationFlags(RHI_AllocationFlagBits flagBits) noexcept {
		VmaAllocationCreateFlags flags = 0;
		if ((flagBits & RHI_AllocationFlagBits::HostAccessSequentialWrite) != RHI_AllocationFlagBits::None) flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		if ((flagBits & RHI_AllocationFlagBits::CreateMapped) != RHI_AllocationFlagBits::None) flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
		return flags;
	}

	constexpr [[nodiscard]] VkImageUsageFlags RHI_GetVkImageUsage(RHI_ImageUsage usage) noexcept {
		VkImageUsageFlags flags = 0;
		if ((usage & RHI_ImageUsage::ColorAttachment) != RHI_ImageUsage::None) flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if ((usage & RHI_ImageUsage::DepthStencilAttachment) != RHI_ImageUsage::None) flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		if ((usage & RHI_ImageUsage::Sampled) != RHI_ImageUsage::None) flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
		if ((usage & RHI_ImageUsage::Storage) != RHI_ImageUsage::None) flags |= VK_IMAGE_USAGE_STORAGE_BIT;
		if ((usage & RHI_ImageUsage::TransferDst) != RHI_ImageUsage::None) flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		if ((usage & RHI_ImageUsage::TransferSrc) != RHI_ImageUsage::None) flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		return flags;
	}

	constexpr [[nodiscard]] VkImageTiling RHI_GetVkImageTiling(RHI_ImageTileMode tile) noexcept { // FIXME
		switch (tile)
		{
		case RHI_ImageTileMode::ClampToBorder: return VK_IMAGE_TILING_OPTIMAL;
		case RHI_ImageTileMode::ClampToEdge: return VK_IMAGE_TILING_OPTIMAL;
		case RHI_ImageTileMode::Mirror: return VK_IMAGE_TILING_OPTIMAL;
		case RHI_ImageTileMode::Repeat: return VK_IMAGE_TILING_OPTIMAL;
		case RHI_ImageTileMode::Optimal: return VK_IMAGE_TILING_OPTIMAL;
		default:
			Logger::Error("Vulkan RHI Utils", "Unknown tiling mode");
			return VK_IMAGE_TILING_OPTIMAL;
		}
	}
}