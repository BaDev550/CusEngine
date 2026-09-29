#pragma once

#include <Runtime/RHI/Buffer/RHIBufferUsage.h>
#include <Runtime/RHI/Buffer/RHIMemoryUsage.h>

#include <Runtime/RHI/Image/RHIImageLayout.h>
#include <Runtime/RHI/Image/RHIImageTileMode.h>
#include <Runtime/RHI/Image/RHIImageUsage.h>

#include <Runtime/RHI/Common/RHIFormat.h>

namespace CusEngine::RHI {
	namespace Utils {
		[[nodiscard]] __forceinline const char* FormatToString(Format format) noexcept {
			switch (format)
			{
			case Format::Undefined: return "Undefined";
			case Format::RG8: return "RG8";
			case Format::RGB8: return "RGB8";
			case Format::RGBA8: return "RGBA8";
			case Format::RGBA16: return "RGBA16";
			case Format::RGBA: return "RGBA";
			case Format::D32_SFLOAT: return "D32 SFloat";
			case Format::D24_UNORM_S8_UINT: return "D24 Unorm S8 UInt";
			case Format::D16_UNORM: return "D16 Unorm";
			case Format::BC3: return "BC3";
			default: return "Unknown format";
			}
		}

		[[nodiscard]] __forceinline const char* ImageTileModeToString(ImageTileMode tileMode) noexcept {
			switch (tileMode)
			{
			case ImageTileMode::Undefined: return "Undefined";
			case ImageTileMode::Repeat: return "Repeat";
			case ImageTileMode::Mirror: return "Mirror";
			case ImageTileMode::ClampToEdge: return "ClampToEdge";
			case ImageTileMode::ClampToBorder: return "ClampToBorder";
			case ImageTileMode::Optimal: return "Optimal";
			default: return "Unknown tile mode";
			}
		}

		[[nodiscard]] __forceinline const char* ImageUsageToString(ImageUsage usage) noexcept {
			switch (usage)
			{
			case ImageUsage::None: return "None";
			case ImageUsage::Sampled: return "Sampled";
			case ImageUsage::Storage: return "Storage";
			case ImageUsage::ColorAttachment: return "ColorAttachment";
			case ImageUsage::DepthStencilAttachment: return "DepthStencilAttachment";
			case ImageUsage::TransferSrc: return "TransferSrc";
			case ImageUsage::TransferDst: return "TransferDst";
			default: return "Unknown usage";
			}
		}

		[[nodiscard]] __forceinline bool IsFormatDepthStencil(Format format) noexcept {
			switch (format)
			{
			case Format::D24_UNORM_S8_UINT: return true;
			default: return false;
			}
		}

		[[nodiscard]] __forceinline bool IsFormatDepth(Format format) noexcept {
			switch (format)
			{
			case Format::D16_UNORM: return true;
			case Format::D24_UNORM_S8_UINT: return true;
			case Format::D32_SFLOAT: return true;
			default: return false;
			}
		}

		[[nodiscard]] __forceinline u32 GetFormatSize(Format format) noexcept {
			switch (format)
			{
			case Format::Undefined: return 0;
			case Format::RG8: return 2;
			case Format::RGB8: return 3;
			case Format::RGBA8: return 4;
			case Format::RGBA16: return 8;
			case Format::RGBA: return 16;
			default: return 0;
			}
		}
	}
}