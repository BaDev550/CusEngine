#pragma once

#include <Runtime/RHI/Common/RHIFormat.h>
#include <Runtime/RHI/Image/RHIImageView.h>
#include <Runtime/RHI/Common/RHIUtils.h>

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

namespace CusEngine::RHI::Utils {
	constexpr [[nodiscard]] VkFormat GetVkFormat(Format format) noexcept {
		switch (format)
		{
		case Format::Undefined: return VK_FORMAT_UNDEFINED;
		case Format::RG8: return VK_FORMAT_R8G8_UNORM;
		case Format::RGB8: return VK_FORMAT_R8G8B8_UNORM;
		case Format::RGBA8: return VK_FORMAT_R8G8B8A8_SRGB;
		case Format::RGBA16: return VK_FORMAT_R16G16B16_SFLOAT;
		case Format::RGBA: return VK_FORMAT_R32G32B32A32_SFLOAT;
		case Format::D32_SFLOAT: return VK_FORMAT_D32_SFLOAT;
		case Format::D16_UNORM: return VK_FORMAT_D16_UNORM;
		case Format::D24_UNORM_S8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;
		default:
			Logger::Error("Vulkan RHI Utils", "Unknown format");
			return VK_FORMAT_UNDEFINED;
		}
	}

	constexpr [[nodiscard]] VkImageLayout GetVkImageLayout(ImageLayout layout) noexcept {
		switch (layout)
		{
		case ImageLayout::Undefined: return VK_IMAGE_LAYOUT_UNDEFINED;
		case ImageLayout::PresentSrc: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		case ImageLayout::ColorAttachment: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		case ImageLayout::DepthAttachment: return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		case ImageLayout::ShaderReadOnly: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		case ImageLayout::TransferDst: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		case ImageLayout::TransferSrc: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		default:
			Logger::Error("Vulkan RHI Utils", "Unknown image layout");
			return VK_IMAGE_LAYOUT_UNDEFINED;
		}
	}

	constexpr [[nodiscard]] VkBufferUsageFlags GetVkBufferUsage(BufferUsage usage) noexcept {
		VkBufferUsageFlags flags = 0;
		if ((usage & BufferUsage::Uniform) != BufferUsage::None) flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		if ((usage & BufferUsage::Storage) != BufferUsage::None) flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		if ((usage & BufferUsage::TransferSrc) != BufferUsage::None) flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		if ((usage & BufferUsage::TransferDst) != BufferUsage::None) flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		if ((usage & BufferUsage::DeviceAddress) != BufferUsage::None) flags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		if ((usage & BufferUsage::Index) != BufferUsage::None) flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		if ((usage & BufferUsage::Vertex) != BufferUsage::None) flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		return flags;
	}

	constexpr [[nodiscard]] VmaMemoryUsage GetVkMemoryUsage(MemoryUsage usage) noexcept {
		switch (usage)
		{
		case MemoryUsage::Auto: return VMA_MEMORY_USAGE_AUTO;
		case MemoryUsage::GPU: return VMA_MEMORY_USAGE_GPU_ONLY;
		case MemoryUsage::CPU: return VMA_MEMORY_USAGE_CPU_ONLY;
		case MemoryUsage::CPUToGPU: return VMA_MEMORY_USAGE_CPU_TO_GPU;
		default:
			Logger::Error("Vulkan RHI Utils", "Unknown memory usage fallback 'auto' used");
			return VMA_MEMORY_USAGE_AUTO;
		}
	}

	constexpr [[nodiscard]] VmaAllocationCreateFlags GetVkAllocationFlags(AllocationFlagBits flagBits) noexcept {
		VmaAllocationCreateFlags flags = 0;
		if ((flagBits & AllocationFlagBits::HostAccessSequentialWrite) != AllocationFlagBits::None) flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		if ((flagBits & AllocationFlagBits::CreateMapped) != AllocationFlagBits::None) flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
		return flags;
	}

	constexpr [[nodiscard]] VkImageUsageFlags GetVkImageUsage(ImageUsage usage) noexcept {
		VkImageUsageFlags flags = 0;
		if ((usage & ImageUsage::ColorAttachment) != ImageUsage::None) flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if ((usage & ImageUsage::DepthStencilAttachment) != ImageUsage::None) flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		if ((usage & ImageUsage::Sampled) != ImageUsage::None) flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
		if ((usage & ImageUsage::Storage) != ImageUsage::None) flags |= VK_IMAGE_USAGE_STORAGE_BIT;
		if ((usage & ImageUsage::TransferDst) != ImageUsage::None) flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		if ((usage & ImageUsage::TransferSrc) != ImageUsage::None) flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		return flags;
	}

	constexpr [[nodiscard]] VkImageTiling GetVkImageTiling(ImageTileMode tile) noexcept { // FIXME
		switch (tile)
		{
		case ImageTileMode::ClampToBorder: return VK_IMAGE_TILING_OPTIMAL;
		case ImageTileMode::ClampToEdge: return VK_IMAGE_TILING_OPTIMAL;
		case ImageTileMode::Mirror: return VK_IMAGE_TILING_OPTIMAL;
		case ImageTileMode::Repeat: return VK_IMAGE_TILING_OPTIMAL;
		case ImageTileMode::Optimal: return VK_IMAGE_TILING_OPTIMAL;
		default:
			Logger::Error("Vulkan RHI Utils", "Unknown tiling mode");
			return VK_IMAGE_TILING_OPTIMAL;
		}
	}

	constexpr [[nodiscard]] VkImageViewType GetVkImageViewType(ImageViewType type) noexcept {
		switch (type)
		{
		case ImageViewType::Image2D: return VK_IMAGE_VIEW_TYPE_2D;
		case ImageViewType::Image3D: return VK_IMAGE_VIEW_TYPE_3D;
		default:
			break;
		}
	}
}