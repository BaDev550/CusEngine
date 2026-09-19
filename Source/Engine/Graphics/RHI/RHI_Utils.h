#pragma once

#include "RHI.h"

extern "C" {
	namespace CusEngine::RHI {
		namespace Utils {
			[[nodiscard]] __forceinline const char* BackendToString(GraphicsBackend backend) noexcept {
				switch (backend)
				{
				case GraphicsBackend::None: return "None";
				case GraphicsBackend::Vulkan: return "Vulkan";
				case GraphicsBackend::OpenGL: return "OpenGL";
				default: return "Unknown api";
				}
			}

			[[nodiscard]] __forceinline const char* FormatToString(ImageFormat format) noexcept {
				switch (format)
				{
				case ImageFormat::Undefined: return "Undefined";
				case ImageFormat::RG8: return "RG8";
				case ImageFormat::RGB8: return "RGB8";
				case ImageFormat::RGBA8: return "RGBA8";
				case ImageFormat::RGBA16: return "RGBA16";
				case ImageFormat::RGBA: return "RGBA";
				case ImageFormat::D32_SFLOAT: return "D32 SFloat";
				case ImageFormat::D24_UNORM_S8_UINT: return "D24 Unorm S8 UInt";
				case ImageFormat::D16_UNORM: return "D16 Unorm";
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

			[[nodiscard]] __forceinline bool IsFormatDepthStencil(ImageFormat format) noexcept {
				switch (format)
				{
				case ImageFormat::D24_UNORM_S8_UINT: return true;
				default: return false;
				}
			}

			[[nodiscard]] __forceinline bool IsFormatDepth(ImageFormat format) noexcept {
				switch (format)
				{
				case ImageFormat::D16_UNORM: return true;
				case ImageFormat::D24_UNORM_S8_UINT: return true;
				case ImageFormat::D32_SFLOAT: return true;
				default: return false;
				}
			}

			[[nodiscard]] __forceinline u32 GetFormatSize(ImageFormat format) noexcept {
				switch (format)
				{
				case ImageFormat::Undefined: return 0;
				case ImageFormat::RG8: return 2;
				case ImageFormat::RGB8: return 3;
				case ImageFormat::RGBA8: return 4;
				case ImageFormat::RGBA16: return 8;
				case ImageFormat::RGBA: return 16;
				default: return 0;
				}
			}
		}
	}
}